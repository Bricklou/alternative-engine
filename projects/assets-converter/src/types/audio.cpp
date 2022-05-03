#include "audio.hpp"

#include "assetslib/assets/AssetFile.hpp"
#include "assetslib/assets/AudioInfo.hpp"
#include "sndfile.h"
#include <AL/al.h>
#include <AL/alext.h>
#include <corecrt_malloc.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vcruntime.h>

namespace Converter {

void convert_audio(const std::filesystem::path &input,
                   const std::filesystem::path &output) {
  SNDFILE *sndfile;
  SF_INFO sfinfo;
  ALenum format;
  std::vector<short> membuf;
  sf_count_t num_frames;

  // Open the audio file and check that it's usable
  sndfile = sf_open(input.string().c_str(), SFM_READ, &sfinfo);
  if (!sndfile) {
    throw std::runtime_error("Failed to load audio file: " + input.string() +
                             "\n" + sf_strerror(sndfile));
  }

  if (sfinfo.frames < 1 ||
      sfinfo.frames > (sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
    sf_close(sndfile);
    std::runtime_error("Bad sample count in audio file: " + input.string() +
                       " " + std::to_string(sfinfo.frames));
  }

  // Get the sound format, and figure out the OpenAL format
  format = AL_NONE;
  switch (sfinfo.channels) {
  case 1:
    format = AL_FORMAT_MONO16;
    break;
  case 2:
    format = AL_FORMAT_STEREO16;
    break;
  case 3:
    if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) ==
        SF_AMBISONIC_B_FORMAT) {

      format = AL_FORMAT_STEREO8;
      break;
    }
  case 4:
    if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) ==
        SF_AMBISONIC_B_FORMAT) {
      format = AL_FORMAT_BFORMAT3D_16;
      break;
    }
  }

  if (!format) {
    sf_close(sndfile);
    throw std::runtime_error("Unsupported audio format: " + input.string());
  }

  // Decode the whole audio file to a buffer
  membuf.resize((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short));

  num_frames = sf_readf_short(sndfile, membuf.data(), sfinfo.frames);
  if (num_frames < 1) {
    sf_close(sndfile);
    throw std::runtime_error("Failed to read audio samples : " +
                             std::to_string(num_frames));
  }

  int num_bytes = (int)(num_frames * sfinfo.channels) * (int)sizeof(short);

  assetslib::AudioInfo audioInfo;
  audioInfo.format = format;
  audioInfo.sample_rate = sfinfo.samplerate;
  audioInfo.channels_count = sfinfo.channels;
  audioInfo.original_file = input.string();
  audioInfo.audio_size = num_bytes;

  assetslib::AssetFile newAudio =
      assetslib::pack_audio(&audioInfo, membuf.data());

  sf_close(sndfile);

  assetslib::save_binaryfile(output.string(), newAudio);
  std::cout << "Audio saved to: " << output << std::endl;
}

} // namespace Converter