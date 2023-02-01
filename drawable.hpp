#pragma once

#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

namespace render {

    /// struct tao
    /*! OpenGL textures
     */
    struct tao { unsigned tao[1024], size; };
    /// struct vbo
    /*! OpenGL vbos
     */
    struct vbo { unsigned mesh, instance, vertex, instanceCount; };

    //! class drawable
    /*! Abstract interface for instancing-based drawing of single object type;
     *! implemented by instanced objects that are passed to the render pipeline
     */
    class Drawable {
    public:

        /// dtor.
        virtual ~Drawable() {}
        /// Called by renderer to draw all stored object instances
        virtual void draw() const = 0;
        /// @param mat model matrix
        virtual void modify(const float* mat, unsigned  instanceIndex) = 0;
        /// @param mat array of model matrices
        /// @param size size of array
        virtual void modify(const float* mat, unsigned* instanceIndices, unsigned size) = 0;
        /// @param mat array of model matrices
        /// @param size size of array
        virtual void reset(const float* mat, unsigned size) = 0;
        /// @param mat model matrix
        virtual void push_back(const float* mat) = 0;
        /// @param mat array of model matrices
        /// @param size size of array
        virtual void push_back(const float* mat, unsigned size) = 0;
    };

    /// @impl
    void modify(vbo& refvbo, const float* mat, unsigned instanceIndex);
    /// @impl
    void modify(vbo& refvbo, const float* mat, unsigned* instanceIndices, unsigned count);

    /// @impl
    void reset(vbo& refvbo, const float* mat, unsigned count);

    /// @impl
    void push_back(vbo& refvbo, const float* mat);
    /// @impl
    void push_back(vbo& refvbo, const float* mat, unsigned count);
}

#endif
