#pragma once

namespace render {
    /// @return TAO
    unsigned load_texture_from_data(const unsigned char* data, int w, int h, int nchannels);
    /// @return TAO
    unsigned load_texture_from_file(const char* path);
    /// @return TAO
    unsigned load_texture_from_file(const char* path, int* width, int* height, int* nchannels);
}
