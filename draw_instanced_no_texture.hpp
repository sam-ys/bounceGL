#pragma once

#ifndef DRAW_INSTANCED_NO_TEXTURE_HPP
#define DRAW_INSTANCED_NO_TEXTURE_HPP

#include "program.hpp"

//! class DrawInstancedNoTexture
/*! Program for drawing untextured objects to screen
 */
class DrawInstancedNoTexture : public Program {
public:
    /// ctor.
    explicit DrawInstancedNoTexture();
    /// @override
    void set_colour(const calc::vec4f& v);
    /// @override
    void set_scene(const calc::mat4f& lookAt, const calc::mat4f& perspective);
};

#endif
