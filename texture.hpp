#pragma once

namespace render {
    /// @return TAO
    unsigned load_texture_from_data(unsigned char* data, int memlen, bool alpha, bool flipVertically = true);
    /// @return TAO
    unsigned load_texture_from_file(const char* path, bool alpha);
}
