#include "glad/glad.h"

#include "drawable.hpp"

void render::modify(vbo& refvbo, const float* mat, unsigned instanceIndex)
{
    static const unsigned nbytes = 16 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    unsigned off = instanceIndex * nbytes;
    glBufferSubData(GL_ARRAY_BUFFER, off, nbytes, mat);
}

void render::modify(vbo& refvbo, const float* mat, unsigned* instanceIndices, unsigned count)
{
    static const unsigned nbytes = 16 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    unsigned i = 0;
    for ( ; i != count; ++i)
    {
        unsigned off = instanceIndices[i] * nbytes;
        glBufferSubData(GL_ARRAY_BUFFER, off, nbytes, mat);
    }
}

void render::reset(vbo& refvbo, const float* mat, unsigned count)
{
    static const unsigned nbytes = 16 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    refvbo.instanceCount = count;
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * nbytes, mat);
}

void render::push_back(vbo& refvbo, const float* mat)
{
    static const unsigned nbytes = 16 * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    unsigned off = refvbo.instanceCount++ * nbytes;
    glBufferSubData(GL_ARRAY_BUFFER, off, nbytes, mat);
}

void render::push_back(vbo& refvbo, const float* mat, unsigned count)
{
    static const unsigned nbytes = 16 * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, refvbo.instance);

    unsigned offset = refvbo.instanceCount * nbytes;
    glBufferSubData(GL_ARRAY_BUFFER, offset, count * nbytes, mat);
    refvbo.instanceCount += count;
}
