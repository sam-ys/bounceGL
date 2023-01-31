#pragma once

#ifndef _CALC_SIMD_MATRIX_MUL_HPP
#define _CALC_SIMD_MATRIX_MUL_HPP

#include "common.hpp"

namespace calc {

    namespace detail {

        template <typename>
        struct matrix_mul_impl;

        template <>
        struct matrix_mul_impl<float> {

            static inline void mul(const __m128& r, const __m128& c, float* out) {

                __m128 tmp;

                tmp = _mm_mul_ps(r, c);
                tmp = _mm_hadd_ps(tmp, tmp);
                tmp = _mm_hadd_ps(tmp, tmp);

                *out = tmp[0];
            }
        };
    }

    template <typename,
              unsigned,
              unsigned,
              unsigned>
    struct matrix_mul;

    template <>
    struct matrix_mul<float, 0, 0, 1> {

        static inline void mul(const float* dat1,
                               const float* dat2,
                               float* out,
                               const std::size_t N0,
                               const std::size_t N1) {

            std::size_t registers0 = N1 / (16 / sizeof(float));
            std::size_t registers1 = 16 / sizeof(float);

            __m128* lhs = new __m128[N0 * registers0];
            __m128* rhs = new __m128[N1 * registers1];

            for (std::size_t i = 0; i != N1 * registers1; ++i)
            {
                std::size_t ii = i * sizeof(float);
                rhs[i] = detail::load(dat2 + ii);
            }

            for (std::size_t i = 0; i != N0 * registers0; ++i)
            {
                std::size_t ii = i * sizeof(float);
                lhs[i] = detail::load(dat1 + ii);
            }

            for (std::size_t i = 0; i != N0; ++i)
            {
                std::size_t ii =  i * registers0;
                std::size_t  j = 0;
                for ( ; j != N1; ++j)
                {
                    __m128 sum = { 0, 0, 0, 0 };

                    std::size_t jj = j * registers1;
                    std::size_t  k = 0;
                    for ( ; k != registers1; ++k)
                    {
                        __m128 tmp;
                        tmp = _mm_mul_ps(lhs[ii + k], rhs[jj + k]);
                        sum = _mm_add_ps(tmp, sum);
                    }

                    sum = _mm_hadd_ps(sum ,sum);
                    sum = _mm_hadd_ps(sum ,sum);
                    *out++ = sum[0];
                }
            }

            delete[] lhs;
            delete[] rhs;
        }
    };

    template <>
    struct matrix_mul<float, 0, 0, 0> {

        static inline void mul(const float* dat1,
                               const float* dat2,
                               float* out,
                               const std::size_t N0,
                               const std::size_t N1,
                               const std::size_t M1) {


            std::size_t registers0 = N1 / (16 / sizeof(float));
            std::size_t registers1 = M1 / (16 / sizeof(float));

            __m128* lhs  = new __m128[N0 * registers0];
            __m128* rhs  = new __m128[N1 * registers1];
            __m128* rhsfloat = new __m128[N1 * registers1];

            // floatranspose
            for (std::size_t i = 0; i != N1 * registers1; ++i)
            {
                std::size_t ii = i * sizeof(float);
                rhs[i] = detail::load(dat2 + ii);
            }

            for (std::size_t i = 0; i != N1; i += __stride__(sizeof(float)))
            {
                std::size_t ii1 =  i      * registers1;
                std::size_t ii2 = (i + 1) * registers1;
                std::size_t ii3 = (i + 2) * registers1;
                std::size_t ii4 = (i + 3) * registers1;

                for (std::size_t j = 0; j != registers1; ++j)
                {
                    std::size_t jj0 = ii1 + j;
                    std::size_t jj1 = ii2 + j;
                    std::size_t jj2 = ii3 + j;
                    std::size_t jj3 = ii4 + j;

                    __m128 y0 = _mm_unpacklo_ps(rhs[jj0], rhs[jj1]);
                    __m128 y1 = _mm_unpackhi_ps(rhs[jj0], rhs[jj1]);
                    __m128 y2 = _mm_unpacklo_ps(rhs[jj2], rhs[jj3]);
                    __m128 y3 = _mm_unpackhi_ps(rhs[jj2], rhs[jj3]);

                    rhsfloat[ii1] = _mm_movelh_ps(y0, y2);
                    rhsfloat[ii2] = _mm_movehl_ps(y2, y0);
                    rhsfloat[ii3] = _mm_movelh_ps(y1, y3);
                    rhsfloat[ii4] = _mm_movehl_ps(y3, y1);
                }
            }

            // Multiply
            for (std::size_t i = 0; i != N0 * registers0; ++i)
            {
                const std::size_t index = i * sizeof(float);

                const float* dat = dat1 + index;
                lhs[i] = _mm_load_ps(dat);
            }

            for (std::size_t i = 0; i != N0; ++i)
            {
                std::size_t ii =  i * registers0;
                std::size_t  j = 0;
                for ( ; j != N1; ++j)
                {
                    __m128 sum = { 0, 0, 0, 0 };

                    std::size_t jj = j * registers1;
                    std::size_t  k = 0;
                    for ( ; k != registers1; ++k)
                    {
                        __m128 tmp;
                        tmp = _mm_mul_ps(lhs[ii + k], rhsfloat[jj + k]);
                        sum = _mm_add_ps(tmp, sum);
                    }

                    sum = _mm_hadd_ps(sum ,sum);
                    sum = _mm_hadd_ps(sum ,sum);
                    *out++ = sum[0];
                }
            }

            delete[] lhs;
            delete[] rhs;
            delete[] rhsfloat;
        }
    };

    template <>
    struct matrix_mul<float, 4, 4, 1> {

        static inline void mul(const float* dat1, const float* dat2, float* out) {

            const __m128 c0 = _mm_load_ps(dat2);

            const __m128 r0 = _mm_load_ps(dat1);
            const __m128 r1 = _mm_load_ps(dat1 +  4);
            const __m128 r2 = _mm_load_ps(dat1 +  8);
            const __m128 r3 = _mm_load_ps(dat1 + 12);

            detail::matrix_mul_impl<float>::mul(r0, c0, out);
            detail::matrix_mul_impl<float>::mul(r1, c0, out + 1);
            detail::matrix_mul_impl<float>::mul(r2, c0, out + 2);
            detail::matrix_mul_impl<float>::mul(r3, c0, out + 3);
        }
    };

    template <>
    struct matrix_mul<float, 3, 3, 1> {

        static inline void mul(const float* dat1, const float* dat2, float* out) {

            const __m128 c0 = _mm_loadu_ps(dat2);

            const __m128 r0 = _mm_loadu_ps(dat1);
            const __m128 r1 = _mm_loadu_ps(dat1 + 3);
            const __m128 r2 = _mm_loadu_ps(dat1 + 6);

            detail::matrix_mul_impl<float>::mul(r0, c0, out);
            detail::matrix_mul_impl<float>::mul(r1, c0, out + 1);
            detail::matrix_mul_impl<float>::mul(r2, c0, out + 2);
        }
    };

    template <>
    struct matrix_mul<float, 4, 4, 4> {

        static inline void mul(const float* dat1, const float* dat2, float* out) {

            // detail::load & transpose RHS matrix
            const __m128 x0 = detail::load(dat2);
            const __m128 x1 = detail::load(dat2 +  4);
            const __m128 x2 = detail::load(dat2 +  8);
            const __m128 x3 = detail::load(dat2 + 12);

            const __m128 y0 = _mm_unpacklo_ps(x0, x1);
            const __m128 y1 = _mm_unpackhi_ps(x0, x1);
            const __m128 y2 = _mm_unpacklo_ps(x2, x3);
            const __m128 y3 = _mm_unpackhi_ps(x2, x3);

            const __m128 c0 = _mm_movelh_ps(y0, y2);
            const __m128 c1 = _mm_movehl_ps(y2, y0);
            const __m128 c2 = _mm_movelh_ps(y1, y3);
            const __m128 c3 = _mm_movehl_ps(y3, y1);

            // detail::load LHS matrix
            const __m128 r0 = detail::load(dat1);
            const __m128 r1 = detail::load(dat1 +  4);
            const __m128 r2 = detail::load(dat1 +  8);
            const __m128 r3 = detail::load(dat1 + 12);

            detail::matrix_mul_impl<float>::mul(r0, c0, out);
            detail::matrix_mul_impl<float>::mul(r0, c1, out +  1);
            detail::matrix_mul_impl<float>::mul(r0, c2, out +  2);
            detail::matrix_mul_impl<float>::mul(r0, c3, out +  3);

            detail::matrix_mul_impl<float>::mul(r1, c0, out +  4);
            detail::matrix_mul_impl<float>::mul(r1, c1, out +  5);
            detail::matrix_mul_impl<float>::mul(r1, c2, out +  6);
            detail::matrix_mul_impl<float>::mul(r1, c3, out +  7);

            detail::matrix_mul_impl<float>::mul(r2, c0, out +  8);
            detail::matrix_mul_impl<float>::mul(r2, c1, out +  9);
            detail::matrix_mul_impl<float>::mul(r2, c2, out + 10);
            detail::matrix_mul_impl<float>::mul(r2, c3, out + 11);

            detail::matrix_mul_impl<float>::mul(r3, c0, out + 12);
            detail::matrix_mul_impl<float>::mul(r3, c1, out + 13);
            detail::matrix_mul_impl<float>::mul(r3, c2, out + 14);
            detail::matrix_mul_impl<float>::mul(r3, c3, out + 15);
        }
    };

    template <>
    struct matrix_mul<float, 3, 3, 3> {

        static inline void mul(const float* dat1, const float* dat2, float* out) {

            const float datc0[4] __attribute__((aligned(16))) = { dat2[0], dat2[3], dat2[6], 0 };
            const float datc1[4] __attribute__((aligned(16))) = { dat2[1], dat2[4], dat2[7], 0 };
            const float datc2[4] __attribute__((aligned(16))) = { dat2[2], dat2[5], dat2[8], 0 };

            const __m128 c0 = detail::load(datc0);
            const __m128 c1 = detail::load(datc1);
            const __m128 c2 = detail::load(datc2);

            __m128 r0 = _mm_loadu_ps(dat1);
            __m128 r1 = _mm_loadu_ps(dat1 + 3);
            __m128 r2 = _mm_loadu_ps(dat1 + 6);

            detail::matrix_mul_impl<float>::mul(r0, c0, out);
            detail::matrix_mul_impl<float>::mul(r0, c1, out + 1);
            detail::matrix_mul_impl<float>::mul(r0, c2, out + 2);

            detail::matrix_mul_impl<float>::mul(r1, c0, out + 3);
            detail::matrix_mul_impl<float>::mul(r1, c1, out + 4);
            detail::matrix_mul_impl<float>::mul(r1, c2, out + 5);

            detail::matrix_mul_impl<float>::mul(r2, c0, out + 6);
            detail::matrix_mul_impl<float>::mul(r2, c1, out + 7);
            detail::matrix_mul_impl<float>::mul(r2, c2, out + 8);
        }
    };
}

#endif
