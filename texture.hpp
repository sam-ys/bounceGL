#pragma once

namespace render {
    /// @return TAO
    unsigned load_texture_from_data(unsigned char* data, int memlen);
    /// @return TAO
    unsigned load_texture_from_file(const char* path);
    /// @return TAO
    unsigned load_texture_from_file(const char* path, int* width, int* height, int* nchannels);
}
