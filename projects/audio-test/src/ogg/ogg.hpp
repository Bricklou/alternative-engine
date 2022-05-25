#pragma once

#include <AL/al.h>
#include <fstream>
#include <string>
#include <vector>
#include <vorbis/vorbisfile.h>

#define OGG_BUFFER_SIZE 65536

struct AudioData {
    std::ifstream file;
    std::string filename;
    ALsizei size_consumed = 0;
    ALsizei size;
    OggVorbis_File ogg_vorbis_file;

    std::uint8_t channels;
    std::int32_t sample_rate;
    std::uint8_t bits_per_sample;

    std::size_t duration;

    int ogg_current_section = 0;
};

void ogg_open(const char *filename, AudioData &audio_data);

void ogg_close(AudioData &audio_data);

uint32_t ogg_read(AudioData &audio_data, char *buffer, std::size_t buffer_size);