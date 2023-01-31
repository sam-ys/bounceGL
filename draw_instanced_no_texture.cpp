#include "calc/matrix.hpp"

#include "draw_instanced_no_texture.hpp"

draw_instanced_no_texture::draw_instanced_no_texture()
{
    const vertex_shader sh1 = {
#include "shaders/instanced_no_texture.vs"
    };

    const fragment_shader sh2 = {
#include "shaders/instanced_no_texture.fs"
    };

    program::add_shader(sh1);
    program::add_shader(sh2);

    // Link program
    program::link();
    program::use();
}

void draw_instanced_no_texture::set_colour(const calc::vec4f& v)
{
    // Set projection matrix
    program::set_value_vec4("color", calc::data(v));
}

void draw_instanced_no_texture::set_scene(const calc::mat4f& lookAt, const calc::mat4f& projection)
{
    // Set projection matrix
    program::set_value_mat4x4("view", calc::data(lookAt));
    // Set view matrix
    program::set_value_mat4x4("projection", calc::data(projection));
}
