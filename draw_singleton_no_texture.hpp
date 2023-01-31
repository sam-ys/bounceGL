#pragma once

#ifndef DRAW_SINGLETON_NO_TEXTURE_HPP
#define DRAW_SINGLETON_NO_TEXTURE_HPP

#include "program.hpp"

//! class draw_generic_with_texture
/*! Program for drawing non-textured, monochromatic objects to screen
 */
class draw_singleton_no_texture : public program {
public:
    /// ctor.
    explicit draw_singleton_no_texture();
    /// @override
    void set_colour(const calc::vec4f& v);
    /// @override
    void set_scene(const calc::mat4f& lookAt, const calc::mat4f& perspective);
    /// @override
    void set_rotation(const calc::vec3f& v);
    /// @override
    void set_scale(const calc::vec3f& v);
    /// @override
    void set_translation(const calc::vec3f& v);
};

#endif
