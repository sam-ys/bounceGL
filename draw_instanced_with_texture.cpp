#include "calc/matrix.hpp"

#include "draw_instanced_with_texture.hpp"

DrawInstancedWithTexture::DrawInstancedWithTexture()
{
    const vertex_shader sh1 = {
#include "shaders/instanced_with_texture.vs"
    };

    const fragment_shader sh2 = {
#include "shaders/instanced_with_texture.fs"
    };

    Program::add_shader(sh1);
    Program::add_shader(sh2);

    // Link program
    Program::link();
    Program::use();

    // Set textures
    Program::set_value("texture1", 0);
    Program::set_value("texture2", 1);

    // Set modelview
    Program::set_value_mat4x4("view", calc::data(calc::mat4f::identity()));
    // Set projection
    Program::set_value_mat4x4("projection", calc::data(calc::mat4f::identity()));
}

void DrawInstancedWithTexture::set_scene(const calc::mat4f& lookAt, const calc::mat4f& projection)
{
    // Set projection matrix
    Program::set_value_mat4x4("view", calc::data(lookAt));
    // Set view matrix
    Program::set_value_mat4x4("projection", calc::data(projection));
}
