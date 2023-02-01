#pragma once

#ifndef _CALC_MATRIX_OPERATION_HPP
#define _CALC_MATRIX_OPERATION_HPP

namespace calc {

    /// @return pointer to the data
    template <typename T,
              unsigned N,
              unsigned M>
    inline T* data(matrix<T, N, M>& m) { return static_cast<T*>(m); }

    /// @return pointer to matrix data
    template <typename T,
              unsigned N,
              unsigned M>
    inline const T* data(const matrix<T, N, M>& m) { return static_cast<const T*>(m); }

    /// @return cross product
    template <typename T>
    inline matrix<T, 4, 1> cross(const matrix<T, 4, 1>& lhs, const matrix<T, 4, 1>& rhs)
    {
        matrix<T, 4, 1> out;

        out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1]; // AyBz - AzBy
        out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2]; // AzBx - AxBz
        out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0]; // AxBy - AyBx

        return out;
    }

    /// @return cross product
    template <typename T>
    inline matrix<T, 3, 1> cross(const matrix<T, 3, 1>& lhs, const matrix<T, 3, 1>& rhs)
    {
        matrix<T, 3, 1> out;

        out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1]; // AyBz - AzBy
        out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2]; // AzBx - AxBz
        out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0]; // AxBy - AyBx

        return out;
    }

    /// @return cross product
    template <typename T>
    inline matrix<T, 3, 1> cross(const matrix<T, 1, 3>& lhs, const matrix<T, 1, 3>& rhs)
    {
        matrix<T, 3, 1> out;

        out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1]; // AyBz - AzBy
        out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2]; // AzBx - AxBz
        out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0]; // AxBy - AyBx

        return out;
    }

    /// @return normalized vector
    template <typename T,
              unsigned N>
    inline matrix<T, N, 1> normal(const matrix<T, N, 1>& in)
    {
        const float* d = data(in);

        float mag = 0;
        for (unsigned i = 0; i != N; ++i)
            mag += (d[i] * d[i]);
        return in / std::sqrt(mag);
    }

    /// @return transposed matrix
    template <typename T,
              unsigned N,
              unsigned M>
    inline matrix<T, M, N> transpose(const matrix<T, N, M>& in)
    {
        matrix<T, M, N> out;

        std::size_t i = 0;
        for ( ; i != in.size(); ++i)
        {
            std::size_t r = i / N;
            std::size_t c = i % N;
            out(c, r) = in(r, c);
        }

        return out;
    }

    /// @return absolute-valued matrix
    template <typename T,
              unsigned N,
              unsigned M>
    inline matrix<T, N, M> abs(const matrix<T, N, M>& inp)
    {
        matrix<T, N, M> out;
        for (unsigned r = 0; r != N; ++r)
        {
            for (unsigned c = 0; c != M; ++c) {
                out(r, c) = std::abs(inp(r, c));
            }
        }

        return out;
    }

    template <unsigned N>
    inline matrix<float, N, 1> max(const matrix<float, N, 1>& lhs, const matrix<float, N, 1>& rhs)
    {
        matrix<float, N, 1> out;

        unsigned i = 0;
        for ( ; i != N; ++i)
            out[i] = (lhs[i] >= rhs[i]) ? lhs[i] : rhs[i];
        return out;
    }

    template <unsigned N>
    inline matrix<float, N, 1> min(const matrix<float, N, 1>& lhs, const matrix<float, N, 1>& rhs)
    {
        matrix<float, N, 1> out;

        unsigned i = 0;
        for ( ; i != N; ++i)
            out[i] = (lhs[i] <= rhs[i]) ? lhs[i] : rhs[i];
        return out;
    }

    /// @return dot product: lhs * rhs
    template <unsigned N>
    inline float dot(const matrix<float, N, 1>& lhs, const matrix<float, N, 1>& rhs)
    {
        float sum = 0;

        unsigned i = 0;
        for ( ; i != lhs.size(); ++i)
            sum += lhs[i] * rhs[i];
        return sum;
    }

    /// @return dot product: lhs * rhs
    template <unsigned N>
    inline float dot(const matrix<float, 0, 0>& lhs, const matrix<float, N, 1>& rhs)
    {
        /**/ assert(lhs.rows() == 1);
        /**/ assert(lhs.size() == rhs.size());

        float sum = 0;

        unsigned i = 0;
        for ( ; i != lhs.size(); ++i)
            sum += lhs[i] * rhs[i];
        return sum;
    }

    /// @return dot product: lhs * rhs
    inline float dot(const matrix<float, 0, 0>& lhs, const matrix<float, 0, 0>& rhs)
    {
        /**/ assert(lhs.rows() == 1);
        /**/ assert(lhs.size() == rhs.size());

        float sum = 0;

        unsigned i = 0;
        for ( ; i != lhs.size(); ++i)
            sum += lhs[i] * rhs[i];
        return sum;
    }
}

#endif
