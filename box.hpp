#pragma once

#ifndef BOX_HPP
#define BOX_HPP

#include "drawable.hpp"

namespace render {

    /// class box
    /*! Implements a textured box
     */
    class box : public drawable {
    public:
        /// ctor.
        box() {}
        /// ctor.
        /// @param taoSrc texture handle array
        /// @param taoCount taoSrc size
        /// @param instanceSizeMax the maximum # of instances to allocate
        box(const unsigned* taoSrc, unsigned taoCount, unsigned instanceSizeMax);
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
