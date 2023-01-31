R"(
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 scale;
uniform mat4 rotation;
uniform mat4 translation;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * translation * rotation * scale * vec4(aPos, 1.0);
}
)"
