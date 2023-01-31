#pragma once

#ifndef _CALC_SIMD_MATRIX_ADD_HPP
#define _CALC_SIMD_MATRIX_ADD_HPP

#include <algorithm>

#include <immintrin.h>

namespace calc {

    namespace detail {

        template <typename>
        struct matrix_add_impl;

        template <>
        struct matrix_add_impl<float> {

            static void add(const float* dat1, const float* dat2, float* out) {

                __m128 v1 = _mm_load_ps(dat1);
                __m128 v2 = _mm_load_ps(dat2);

                __m128 v3 = _mm_add_ps(v1, v2);
                _mm_store_ps(out, v3);
            }
        };

        template <>
        struct matrix_add_impl<double> {

            static void add(const double* dat1, const double* dat2, double* out) {

                __m128d v1 = _mm_load_pd(dat1);
                __m128d v2 = _mm_load_pd(dat2);

                __m128d v3 = _mm_add_pd(v1, v2);
                _mm_store_pd(out, v3);
            }
        };
    }

    /// functor matrix_add
    /*! SIMD matrix addition
     */
    template <typename T,
              unsigned = 0>
    struct matrix_add {

        static inline void add(const T* dat1, const T* dat2, T* out, std::size_t size) {

            size = std::max(size / sizeof(T), sizeof(T));

            std::size_t i = 0;
            for ( ; i < size; ++i)
            {
                std::size_t ii = i * 16 / sizeof(T);
                detail::matrix_add_impl<T>::add(dat1 + ii, dat2 + ii, out + ii);
            }
        }
    };

    /*! SIMD matrix addition / matrix size 2
     */
    template <>
    struct matrix_add<float, 2> {

        static inline void add(const float* dat1, const float* dat2, float* out, std::size_t) {
            detail::matrix_add_impl<float>::add(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix addition / matrix size 3
     */
        template <>
        struct matrix_add<float, 3> {

            static inline void add(const float* dat1, const float* dat2, float* out, std::size_t) {
                detail::matrix_add_impl<float>::add(dat1, dat2, out); //> 1 pass
            }
        };

    /*! SIMD matrix addition / matrix size 4
     */
    template <>
    struct matrix_add<float, 4> {

        static inline void add(const float* dat1, const float* dat2, float* out, std::size_t) {
            detail::matrix_add_impl<float>::add(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix addition / matrix size 8
     */
    template <>
    struct matrix_add<float, 9> {

        static const unsigned i1 = 2;

        static inline void add(const float* dat1, const float* dat2, float* out, std::size_t) {
            // 2 passes
            detail::matrix_add_impl<float>::add(dat1, dat2, out);
            detail::matrix_add_impl<float>::add(dat1 + i1, dat2 + i1, out + i1);
        }
    };

    /*! SIMD matrix addition / matrix size 16
     */
    template <>
    struct matrix_add<float, 16> {

        static const unsigned i1 =  4;
        static const unsigned i2 =  8;
        static const unsigned i3 = 12;

        static inline void add(const float* dat1, const float* dat2, float* out, std::size_t) {
            // 4 passes
            detail::matrix_add_impl<float>::add(dat1, dat2, out);
            detail::matrix_add_impl<float>::add(dat1 + i1, dat2 + i1, out + i1);
            detail::matrix_add_impl<float>::add(dat1 + i2, dat2 + i2, out + i2);
            detail::matrix_add_impl<float>::add(dat1 + i3, dat2 + i3, out + i3);
        }
    };
};

#endif
