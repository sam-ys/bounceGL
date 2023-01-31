#include "glad/glad.h"

#include "program.hpp"

program::build_exception::build_exception(int programHandle) {
    ::memset(message__, 0, (bufflen__ + 1));
    glGetProgramInfoLog(programHandle, bufflen__, &len__, &message__[0]);
}

const char* program::build_exception::what() const {
    return message__;
}

const char* program::build_exception::what(::size_t* len /* [out] */) const {
    return (*len = this->len__), message__;
}

program::program() {
    programHandle_ = glCreateProgram();
}

void program::use() {
    glUseProgram(programHandle_);
}

void program::link()
{
    // Link program
    glLinkProgram(programHandle_);

    // Check for compile errors
    int ret;
    glGetProgramiv(programHandle_, GL_LINK_STATUS, &ret);
    if (ret == GL_FALSE) {
        throw program::build_exception(programHandle_);
    }
}

void program::set_value(const char* name, const bool value) {
    glUniform1i(glGetUniformLocation(programHandle_, name), value);
}

void program::set_value(const char* name, const int value) {
    glUniform1i(glGetUniformLocation(programHandle_, name), value);
}

void program::set_value(const char* name, const float value) {
    glUniform1f(glGetUniformLocation(programHandle_, name), value);
}

void program::set_value_vec3(const char* name, const float* value) {
    glUniform3fv(glGetUniformLocation(programHandle_, name), 1, value);
}

void program::set_value_mat3x3(const char* name, const float* value) {
    glUniformMatrix3fv(glGetUniformLocation(programHandle_, name), 1, GL_FALSE, value);
}

void program::set_value_vec4(const char* name, const float* value) {
    glUniform4fv(glGetUniformLocation(programHandle_, name), 1, value);
}

void program::set_value_mat4x4(const char* name, const float* value) {
    glUniformMatrix4fv(glGetUniformLocation(programHandle_, name), 1, GL_FALSE, value);
}

namespace {

    // Helper
    inline void create_shader(const int programHandle, const char* src, const int type)
    {
        // Build and compile shader program
        const int shaderHandle = glCreateShader(type);

        glShaderSource(shaderHandle, 1, &src, NULL);
        glCompileShader(shaderHandle);

        // Check status
        int ret;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &ret);
        if (ret == GL_FALSE) {
            throw program::build_exception(programHandle);
        }

        glAttachShader(programHandle, shaderHandle);
        glDeleteShader(shaderHandle);
    }
}

void program::create_shader(const fragment_shader& s) {
    ::create_shader(programHandle_, s.src, GL_FRAGMENT_SHADER);
}

void program::create_shader(const vertex_shader& s) {
    ::create_shader(programHandle_, s.src, GL_VERTEX_SHADER);
}
