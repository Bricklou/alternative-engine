#include "ogg.hpp"
#include "AL/al.h"
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ios>
#include <iostream>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <vector>

std::size_t read_ogg_callback(void *destination, std::size_t size1,
                              std::size_t size2, void *fileHandle) {
  AudioData *audioData = reinterpret_cast<AudioData *>(fileHandle);

  ALsizei length = size1 * size2;

  if (audioData->size_consumed + length > audioData->size)
    length = audioData->size - audioData->size_consumed;

  if (!audioData->file.is_open()) {
    audioData->file.open(audioData->filename, std::ios::binary);
    if (!audioData->file.is_open()) {
      std::cerr << "ERROR: Could not re-open streaming file \""
                << audioData->filename << "\"" << std::endl;
      return 0;
    }
  }

  char *moreData = new char[length];

  audioData->file.clear();
  audioData->file.seekg(audioData->size_consumed);
  if (!audioData->file.read(&moreData[0], length)) {
    if (audioData->file.eof()) {
      audioData->file.clear(); // just clear the error, we will resolve it later
    } else if (audioData->file.fail()) {
      std::cerr << "ERROR: OGG stream has fail bit set " << audioData->filename
                << std::endl;
      audioData->file.clear();
      return 0;
    } else if (audioData->file.bad()) {
      perror(
          ("ERROR: OGG stream has bad bit set " + audioData->filename).c_str());
      audioData->file.clear();
      return 0;
    }
  }
  audioData->size_consumed += length;

  std::memcpy(destination, &moreData[0], length);

  delete[] moreData;

  audioData->file.clear();

  return length;
}

std::int32_t seek_ogg_callback(void *fileHandle, ogg_int64_t to,
                               std::int32_t type) {
  AudioData *audioData = reinterpret_cast<AudioData *>(fileHandle);

  if (type == SEEK_CUR) {
    audioData->size_consumed += to;
  } else if (type == SEEK_END) {
    audioData->size_consumed = audioData->size - to;
  } else if (type == SEEK_SET) {
    audioData->size_consumed = to;
  } else
    return -1; // what are you trying to do vorbis?

  if (audioData->size_consumed < 0) {
    audioData->size_consumed = 0;
    return -1;
  }
  if (audioData->size_consumed > audioData->size) {
    audioData->size_consumed = audioData->size;
    return -1;
  }

  return 0;
}

long int tell_ogg_callback(void *fileHandle) {
  AudioData *audioData = reinterpret_cast<AudioData *>(fileHandle);
  return audioData->size_consumed;
}

void ogg_open(const char *filename, AudioData &audio_data) {
  audio_data.filename = filename;
  audio_data.file.open(filename, std::ios::binary);

  if (!audio_data.file.is_open()) {
    throw std::runtime_error("Could not open file");
  }

  audio_data.file.seekg(0, std::ios::beg);
  audio_data.file.ignore(std::numeric_limits<std::streamsize>::max());
  audio_data.size = audio_data.file.gcount();
  audio_data.file.clear();
  audio_data.file.seekg(0, std::ios::beg);
  audio_data.size_consumed = 0;

  ov_callbacks ogg_callbacks;
  ogg_callbacks.read_func = read_ogg_callback;
  ogg_callbacks.close_func = nullptr;
  ogg_callbacks.seek_func = seek_ogg_callback;
  ogg_callbacks.tell_func = tell_ogg_callback;

  if (ov_open_callbacks(reinterpret_cast<void *>(&audio_data),
                        &audio_data.ogg_vorbis_file, nullptr, -1,
                        ogg_callbacks) < 0) {
    throw std::runtime_error("Could not ov_open_callbacks");
  }

  vorbis_info *vorbis_info = ov_info(&audio_data.ogg_vorbis_file, -1);
  audio_data.channels = vorbis_info->channels;
  audio_data.bits_per_sample = 16;
  audio_data.sample_rate = vorbis_info->rate;

  audio_data.duration = ov_time_total(&audio_data.ogg_vorbis_file, -1);

  if (audio_data.file.eof()) {
    throw std::runtime_error(
        "Already reached end of file without loading data");
  } else if (audio_data.file.fail()) {
    throw std::runtime_error("Fail bit set");
  } else if (!audio_data.file) {
    throw std::runtime_error("File is false");
  }
}

void ogg_close(AudioData &audio_data) {
  // Close the file
  ov_clear(&audio_data.ogg_vorbis_file);
  if (audio_data.file.is_open()) {
    audio_data.file.close();
  }
}

uint32_t ogg_read(AudioData &audio_data, char *buffer,
                  std::size_t buffer_size) {

  std::int32_t dataSoFar = 0;
  while (dataSoFar < OGG_BUFFER_SIZE) {
    std::int32_t result = ov_read(
        &audio_data.ogg_vorbis_file, &buffer[dataSoFar],
        OGG_BUFFER_SIZE - dataSoFar, 0, 2, 1, &audio_data.ogg_current_section);

    if (result == OV_HOLE) {
      std::cerr << "ERROR: OV_HOLE found in initial read of buffer "
                << std::endl;
      break;
    } else if (result == OV_EBADLINK) {
      std::cerr << "ERROR: OV_EBADLINK found in initial read of buffer "
                << std::endl;
      break;
    } else if (result == OV_EINVAL) {
      std::cerr << "ERROR: OV_EINVAL found in initial read of buffer "
                << std::endl;
      break;
    } else if (result == 0) {
      std::cerr << "ERROR: EOF found in initial read of buffer " << std::endl;
      break;
    }

    dataSoFar += result;
  }

  return dataSoFar;
}
