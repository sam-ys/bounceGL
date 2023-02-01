#pragma once

#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "calc/matrix.hpp"

//! struct vertex_shader
/*! vertex shader source code 
 */
struct vertex_shader { const char* src; };

//! struct fragment_shader
/*! fragment shader source code 
 */
struct fragment_shader { const char* src; };

//! class program
/*! Encapsulates an opengl program
 */
class Program {
public:
    //! struct BuildException
    /*! Thrown on program creation error
     */
    struct BuildException;
    // dtor.
    virtual ~Program() {}
    // ctor.
    Program();
    /// Sets program to be used by subsequent calls
    void use();
    /// Links program (use during creation phase)
    void link();
    /// @set
    void set_value(const char* name, const bool value);
    /// @set
    void set_value(const char* name, const int  value);
    /// @set
    void set_value(const char* name, const float value);
    /// @set
    void set_value_vec3(const char* name, const float* value);
    /// @set
    void set_value_mat3x3(const char* name, const float* value);
    /// @set
    void set_value_vec4(const char* name, const float* value);
    /// @set
    void set_value_mat4x4(const char* name, const float* value);
    /// Adds shaders
    /// @param first shader
    /// @param args... additional shaders
    template <typename T,
              typename ... Args>
    void add_shader(const T first, const Args ... args) {
        create_shader(first);
        add(args ...);
    }
    /// Adds shader (add_shader() base case)
    /// @param first shader
    template <typename T>
    void add_shader(const T first) {
        create_shader(first);
    }

private:

    // Handle to shader program
    int programHandle_;
    // Helper
    // @param fragment shader source
    void create_shader(const fragment_shader& s);
    // Helper
    // @param vertex shader source
    void create_shader(const vertex_shader& s);
};

/// struct BuildException
/*! Thrown on program creation failure
 */
struct Program::BuildException {

    static const int bufflen__ = 1024;

    int len__;
    char message__[bufflen__ + 1];

    /// ctor.
    /// @param programHandle handle to program instance under construction
    explicit BuildException(int programHandle);
    /// @get
    /// @return null-terminated error message
    const char* what() const;
    /// @get
    /// @param len error message length
    /// @return null-terminated error message
    const char* what(::size_t* len /* [out] */) const;
};

#endif
