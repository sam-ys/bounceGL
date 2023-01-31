#pragma once

#ifndef DRAW_INSTANCED_NO_TEXTURE_HPP
#define DRAW_INSTANCED_NO_TEXTURE_HPP

#include "program.hpp"

/// Draws objects to screen
class draw_instanced_no_texture : public program {
public:
    /// ctor.
    explicit draw_instanced_no_texture();
    /// @override
    void set_colour(const calc::vec4f& v);
    /// @override
    void set_scene(const calc::mat4f& lookAt, const calc::mat4f& perspective);
};

#endif
