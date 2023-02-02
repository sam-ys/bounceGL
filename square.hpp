#pragma once

#ifndef SQUARE_HPP
#define SQUARE_HPP

#include "drawable.hpp"

namespace render {
    /// class Square
    /*! Implements a textured square
     */
    class Square : public Drawable {
    public:
        /// ctor.
        Square() {}
        /// ctor.
        /// @param taoSrc texture handle array
        /// @param taoCount taoSrc size
        /// @param instanceSizeMax the maximum # of instances to allocate
        Square(const unsigned* taoSrc, unsigned taoCount, unsigned instanceSizeMax);
        /// @override
        void draw() const;
        /// @override
        void modify(const float* mat, unsigned  instanceIndex);
        /// @override
        void modify(const float* mat, unsigned* instanceIndices, unsigned count);
        /// @override
        void reset(const float* mat, unsigned count);
        /// @override
        void push_back(const float* mat);
        /// @override
        void push_back(const float* mat, unsigned count);

    private:

        // Texture handles
        tao tao_;
        // Vertex handles
        vbo vbo_;
    };
}

#endif
