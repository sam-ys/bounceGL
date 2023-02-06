#include "calc/matrix.hpp"

#include "draw_instanced_no_texture.hpp"

DrawInstancedNoTexture::DrawInstancedNoTexture()
{
    const vertex_shader sh1 = {
#include "shaders/instanced_no_texture.vs"
    };

    const fragment_shader sh2 = {
#include "shaders/instanced_no_texture.fs"
    };

    Program::add_shader(sh1);
    Program::add_shader(sh2);

    // Link program
    Program::link();
    Program::use();
}

void DrawInstancedNoTexture::set_color(const calc::vec4f& v)
{
    // Set projection matrix
    Program::set_value_vec4("color", calc::data(v));
}

void DrawInstancedNoTexture::set_scene(const calc::mat4f& lookAt, const calc::mat4f& projection)
{
    // Set projection matrix
    Program::set_value_mat4x4("view", calc::data(lookAt));
    // Set view matrix
    Program::set_value_mat4x4("projection", calc::data(projection));
}
