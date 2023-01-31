#pragma once

#ifndef DRAW_INSTANCED_WITH_TEXTURE_HPP
#define DRAW_INSTANCED_WITH_TEXTURE_HPP

#include "program.hpp"

//! class draw_instanced_with_texture
/*! Program for drawing textured objects to screen
 */
class draw_instanced_with_texture : public program {
public:
    /// ctor.
    draw_instanced_with_texture();
    /// @override
    void set_scene(const calc::mat4f& lookAt, const calc::mat4f& perspective);
};

#endif
