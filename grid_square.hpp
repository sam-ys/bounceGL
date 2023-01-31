#pragma once

#ifndef GRID_SQUARE_HPP
#define GRID_SQUARE_HPP

#include <initializer_list>

#include "drawable.hpp"

namespace render {

    class grid_square : public drawable {
    public:
        grid_square() {}
        /// ctor.
        /// @param instanceSizeMax the maximum # of instances to allocate
        explicit grid_square(unsigned instanceSizeMax);
        /// @override
        void draw() const;
        /// @override
        void modify(const float* mat, unsigned  instanceIndex);
        /// @override
        void modify(const float* mat, unsigned* instanceIndices, unsigned size);
        /// @override
        void reset(const float* mat, unsigned size);
        /// @override
        void push_back(const float* mat);
        /// @override
        void push_back(const float* mat, unsigned size);

    private:

        // Vertex handles
        vbo vbo_;
    };
}

#endif
