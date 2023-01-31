#include "calc/matrix.hpp"

#include "draw_instanced_with_texture.hpp"

draw_instanced_with_texture::draw_instanced_with_texture()
{
    const vertex_shader sh1 = {
#include "shaders/instanced_with_texture.vs"
    };

    const fragment_shader sh2 = {
#include "shaders/instanced_with_texture.fs"
    };

    program::add_shader(sh1);
    program::add_shader(sh2);

    // Link program
    program::link();
    program::use();

    // Set textures
    program::set_value("texture1", 0);
    program::set_value("texture2", 1);

    // Set modelview
    program::set_value_mat4x4("view", calc::data(calc::mat4f::identity()));
    // Set projection
    program::set_value_mat4x4("projection", calc::data(calc::mat4f::identity()));
}

void draw_instanced_with_texture::set_scene(const calc::mat4f& lookAt, const calc::mat4f& projection)
{
    // Set projection matrix
    program::set_value_mat4x4("view", calc::data(lookAt));
    // Set view matrix
    program::set_value_mat4x4("projection", calc::data(projection));
}
