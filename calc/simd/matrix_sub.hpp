#pragma once

#ifndef _CALC_MATRIX_SUB_IMPL_HPP
#define _CALC_MATRIX_SUB_IMPL_HPP

#include <algorithm>

#include <immintrin.h>

namespace calc {

    namespace detail {

        template <typename>
        struct matrix_sub_impl;

        template <>
        struct matrix_sub_impl<float> {

            static void sub(const float* dat1, const float* dat2, float* out) {

                __m128 v1 = _mm_load_ps(dat1);
                __m128 v2 = _mm_load_ps(dat2);

                __m128 v3 = _mm_sub_ps(v1, v2);
                _mm_storeu_ps(out, v3);
            }
        };

        template <>
        struct matrix_sub_impl<double> {

            static void sub(const double* dat1, const double* dat2, double* out) {

                __m128d v1 = _mm_load_pd(dat1);
                __m128d v2 = _mm_load_pd(dat2);

                __m128d v3 = _mm_sub_pd(v1, v2);
                _mm_store_pd(out, v3);
            }
        };
    }

    /// functor detail::matrix_sub
    /*! SIMD matrix subtraction 
     */
    template <typename T,
              unsigned = 0>
    struct matrix_sub {

        static inline void sub(const T* dat1, const T* dat2, T* out, std::size_t size) {

            size = std::max(size / sizeof(T), sizeof(T));

            std::size_t i = 0;
            for ( ; i < size; ++i)
            {
                std::size_t ii = i * 16 / sizeof(T);
                detail::matrix_sub_impl<T>::sub(dat1 + ii, dat2 + ii, out + ii);
            }
        }
    };

    /*! SIMD matrix subtraction / matrix size 2
     */
    template <typename T>
    struct matrix_sub<T, 2> {

        static inline void sub(const T* dat1, const T* dat2, T* out, std::size_t) {
            detail::matrix_sub_impl<T>::sub(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix subtraction / matrix size 3
     */
    template <typename T>
    struct matrix_sub<T, 3> {

        static inline void sub(const T* dat1, const T* dat2, T* out, std::size_t) {
            detail::matrix_sub_impl<T>::sub(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix subtraction / matrix size 4
     */
    template <typename T>
    struct matrix_sub<T, 4> {

        static inline void sub(const T* dat1, const T* dat2, T* out, std::size_t) {
            detail::matrix_sub_impl<T>::sub(dat1, dat2, out); //> 1 pass
        }
    };

    /*! SIMD matrix subtraction / matrix size 9
     */
    template <typename T>
    struct matrix_sub<T, 9> {

        static const unsigned i1 = 2;

        static inline void sub(const T* dat1, const T* dat2, T* out, std::size_t) {
            // 2 passes
            detail::matrix_sub_impl<T>::sub(dat1, dat2, out);
            detail::matrix_sub_impl<T>::sub(dat1 + i1, dat2 + i1, out + i1);
        }
    };

    /*! SIMD matrix subtraction / matrix size 16
     */
    template <typename T>
    struct matrix_sub<T, 16> {

        static const unsigned i1 = 16 / sizeof(T);
        static const unsigned i2 = 32 / sizeof(T);
        static const unsigned i3 = 48 / sizeof(T);

        static inline void sub(const T* dat1, const T* dat2, T* out, std::size_t) {
            // 4 passes
            detail::matrix_sub_impl<T>::sub(dat1, dat2, out);
            detail::matrix_sub_impl<T>::sub(dat1 + i1, dat2 + i1, out + i1);
            detail::matrix_sub_impl<T>::sub(dat1 + i2, dat2 + i2, out + i2);
            detail::matrix_sub_impl<T>::sub(dat1 + i3, dat2 + i3, out + i3);
        }
    };
}

#endif
