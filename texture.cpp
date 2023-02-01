#include <cstring>
#include <new>

#include "glad/glad.h"

#include "stb/stb_image.h"

#include "calc/mem.hpp"

#include "texture.hpp"

namespace {

    struct texture {

        int width;
        int height;
        int nchannels;

        ::size_t buffsize;
        unsigned char* data;

        // @dtor.
        ~texture() {
            calc::delmap<unsigned char>(data, buffsize);
        }

        // @ctor.
        texture(int width, int height, int nchannels, const unsigned char* data) : width(width)
                                                                                 , height(height)
                                                                                 , nchannels(nchannels) {
            unsigned datasize = width * height * nchannels;
            // Copy data
            buffsize = datasize;
            if ((this->data = static_cast<unsigned char*>(calc::genmap<unsigned char>(buffsize))) == nullptr) {
                throw std::bad_alloc();
            }
            ::memset(this->data, 0, buffsize);
            ::memcpy(this->data, data, datasize);
        }
    };

    unsigned generate_texture(const texture& t)
    {
        // Generate texture
        unsigned tao;
        glGenTextures(1, &tao);
        glBindTexture(GL_TEXTURE_2D, tao);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // WebGL requirement
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     t.width,
                     t.height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     t.data);

        glGenerateMipmap(GL_TEXTURE_2D);
        return (glBindTexture(GL_TEXTURE_2D, 0), tao);
    }
}

unsigned render::load_texture_from_data(unsigned char* mem, int memlen, bool flipVertically)
{
    int width = 0;
    int height = 0;
    int nchannels = 0;

    stbi_set_flip_vertically_on_load(flipVertically);
    unsigned char* data = stbi_load_from_memory(mem, memlen, &width, &height, &nchannels, 0);

    texture t(width, height, nchannels, data);
    return (stbi_image_free(data), generate_texture(t));
}

unsigned render::load_texture_from_file(const char* path)
{
    // Load image, create texture and generate mipmaps
    int width = 0;
    int height = 0;
    int nchannels = 0;

    // Load image data
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nchannels, 0);

    texture t(width, height, nchannels, data);
    return (stbi_image_free(data), generate_texture(t));
}
