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
#if 0
        __m128 magnitude = { 0, 0, 0, 0 };

        std::size_t i = 0;
        for ( ; i < in.size(); i += 16 / sizeof(T))
        {
            __m128 vs1 = detail::load(data(in) + i);
            __m128 vs2 = _mm_mul_ps(vs1, vs1);
            magnitude = _mm_add_ps(magnitude, vs2);
        }

        magnitude = _mm_hadd_ps(magnitude, magnitude);
        magnitude = _mm_hadd_ps(magnitude, magnitude);
        return in / std::sqrt(magnitude[0]);
#endif

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

#if 0
    /// @return transposed matrix
    template <>
    inline matrix<float, 4, 4> transpose(const matrix<float, 4, 4>& inp)
    {
        const float* inpdat = data(inp);

        // Load & transpose RHS matrix
        const __m128 x0 = detail::load(inpdat);
        const __m128 x1 = detail::load(inpdat +  4);
        const __m128 x2 = detail::load(inpdat +  8);
        const __m128 x3 = detail::load(inpdat + 12);

        const __m128 y0 = _mm_unpacklo_ps(x0, x1);
        const __m128 y1 = _mm_unpackhi_ps(x0, x1);
        const __m128 y2 = _mm_unpacklo_ps(x2, x3);
        const __m128 y3 = _mm_unpackhi_ps(x2, x3);

        const __m128 vs0 = _mm_movelh_ps(y0, y2);
        const __m128 vs1 = _mm_movehl_ps(y2, y0);
        const __m128 vs2 = _mm_movelh_ps(y1, y3);
        const __m128 vs3 = _mm_movehl_ps(y3, y1);

        matrix<float, 4, 4> out;
        float* outdat = data(out);

        detail::store(outdat,      vs0);
        detail::store(outdat +  4, vs1);
        detail::store(outdat +  8, vs2);
        detail::store(outdat + 12, vs3);
        return out;
    }
#endif

    /// @return absolute-valued matrix
    template <typename T,
              unsigned N,
              unsigned M>
    inline matrix<T, N, M> abs(const matrix<T, N, M>& inp)
    {
        matrix<T, N, M> out;
#if 0
        std::size_t i = 0;
        for ( ; i < inp.size(); i += 16 / sizeof(T))
        {
            __m128 vs1 = detail::load(&inp[i]);
            __m128 vs2 = _mm_mul_ps(vs1, vs1);
            __m128 vs3 = _mm_sqrt_ps(vs2);
            detail::store(calc::data(out), vs3);
        }
#endif

        for (unsigned r = 0; r != N; ++r)
        {
            for (unsigned c = 0; c != M; ++c) {
                out(r, c) = std::abs(inp(r, c));
            }
        }

        return out;
    }

#if 0
    /// @return absolute-valued matrix
    template <>
    inline matrix<float, 0, 0> abs(const matrix<float, 0, 0>& inp)
    {
        matrix<float, 0, 0> out(inp.rows(), inp.cols());

        for (unsigned r = 0; r != inp.rows(); ++r)
        {
            for (unsigned c = 0; c != inp.rows(); ++c) {
                out(r, c) = std::abs(inp(r, c));
            }
        }

        return out;
    }
#endif
    template <unsigned N>
    inline matrix<float, N, 1> max(const matrix<float, N, 1>& lhs, const matrix<float, N, 1>& rhs)
    {
        matrix<float, N, 1> out;
#if 0
        __m128 vs1 = detail::load(data(lhs));
        __m128 vs2 = detail::load(data(rhs));
        __m128 vs3 = _mm_max_ps(vs1, vs2);

        detail::store(data(out), vs3);
#endif
        unsigned i = 0;
        for ( ; i != N; ++i)
            out[i] = (lhs[i] >= rhs[i]) ? lhs[i] : rhs[i];
        return out;
    }

    template <unsigned N>
    inline matrix<float, N, 1> min(const matrix<float, N, 1>& lhs, const matrix<float, N, 1>& rhs)
    {
        matrix<float, N, 1> out;
#if 0
        __m128 vs1 = detail::load(data(lhs));
        __m128 vs2 = detail::load(data(rhs));
        __m128 vs3 = _mm_min_ps(vs1, vs2);

        detail::store(data(out), vs3);
#endif

        unsigned i = 0;
        for ( ; i != N; ++i)
            out[i] = (lhs[i] <= rhs[i]) ? lhs[i] : rhs[i];
        return out;
    }

    /// @return dot product: lhs * rhs
    template <unsigned N>
    inline float dot(const matrix<float, N, 1>& lhs, const matrix<float, N, 1>& rhs)
    {
#if 0
        __m128 sum = { 0, 0, 0, 0 };
        unsigned i = 0;
        for ( ; i < lhs.size(); i += 16 / sizeof(float))
        {
            __m128 vs1 = detail::load(&lhs[i]);
            __m128 vs2 = detail::load(&rhs[i]);
            __m128 vs3 = _mm_mul_ps(vs1, vs2);
            sum = _mm_add_ps(sum, vs3);
        }

        sum = _mm_add_ps(sum, _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(2, 3, 0, 1)));
        sum = _mm_add_ps(sum, _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(0, 1, 2, 3)));
        return sum[0];
#endif

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
