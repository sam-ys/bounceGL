#include "calc/matrix.hpp"

#include "draw_singleton_no_texture.hpp"

draw_singleton_no_texture::draw_singleton_no_texture()
{
    const vertex_shader sh1 = {
#include "shaders/singleton_no_texture.vs"
    };

    const fragment_shader sh2 = {
#include "shaders/singleton_no_texture.fs"
    };

    program::add_shader(sh1);
    program::add_shader(sh2);

    // Link program
    program::link();
    program::use();

    // Set modelview
    program::set_value_mat4x4("view", calc::data(calc::mat4f::identity()));

    // Set projection
    program::set_value_mat4x4("projection", calc::data(calc::mat4f::identity()));

    // Set model attribute
    set_rotation(calc::vec3f(0, 0, 0));

    // Set model attribute
    set_scale(calc::vec3f(1.0, 1.0, 1.0));

    // Set model attribute
    set_translation(calc::vec3f(0, 0, 0));
}

void draw_singleton_no_texture::set_colour(const calc::vec4f& v)
{
    // Set projection matrix
    program::set_value_vec4("color", calc::data(v));
}

void draw_singleton_no_texture::set_scene(const calc::mat4f& lookAt, const calc::mat4f& projection)
{
    // Set projection matrix
    program::set_value_mat4x4("view", calc::data(lookAt));
    // Set view matrix
    program::set_value_mat4x4("projection", calc::data(projection));
}

void draw_singleton_no_texture::set_rotation(const calc::vec3f& v)
{
    const float x = v[0];
    const float y = v[1];
    const float z = v[2];

    const calc::mat4f value = calc::mat4f::identity()
        * calc::rotate_4x(x)
        * calc::rotate_4y(y)
        * calc::rotate_4z(z);

    program::set_value_mat4x4("rotation", calc::data(value));
}

void draw_singleton_no_texture::set_scale(const calc::vec3f& v)
{
    const float x = v[0];
    const float y = v[1];
    const float z = v[2];

    const calc::mat4f value(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
    program::set_value_mat4x4("scale", calc::data(value));
}

void draw_singleton_no_texture::set_translation(const calc::vec3f& v)
{
    const float x = v[0];
    const float y = v[1];
    const float z = v[2];

    const calc::mat4f value(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1);
    program::set_value_mat4x4("translation", calc::data(value));
}
