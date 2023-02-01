#pragma once

#ifndef DRAW_INSTANCED_WITH_TEXTURE_HPP
#define DRAW_INSTANCED_WITH_TEXTURE_HPP

#include "program.hpp"

//! class DrawInstancedWithTexture
/*! Program for drawing textured objects to screen
 */
class DrawInstancedWithTexture : public Program {
public:
    /// ctor.
    DrawInstancedWithTexture();
    /// @override
    void set_scene(const calc::mat4f& lookAt, const calc::mat4f& perspective);
};

#endif
