#pragma once

#ifndef _CALC_SIMD_SCALAR_DIV_HPP
#define _CALC_SIMD_SCALAR_DIV_HPP

#include <algorithm>

#include "common.hpp"

namespace calc {

    namespace detail {

        template <typename>
        struct scalar_div_impl;

        template <>
        struct scalar_div_impl<float> {

            static void div(const float* dat1, const __m128 v2, float* out) {

                __m128 v1 = _mm_load_ps(dat1);
                __m128 v3 = _mm_div_ps(v1, v2);
                _mm_store_ps(out, v3);
            }
        };

        template <>
        struct scalar_div_impl<double> {

            static void div(const double* dat1, const __m128d v2, double* out) {

                __m128d v1 = _mm_load_pd(dat1);
                __m128d v3 = _mm_div_pd(v1, v2);
                _mm_store_pd(out, v3);
            }
        };
    }

    /// functor scalar_div
    /*! SIMD scalar division
     */
    template <typename T,
              unsigned = 0>
    struct scalar_div {

        static inline void div(const T* dat1, const T dat2, T* out, std::size_t size) {

            const typename detail::__m<T>::T v2 = _mm_set1_ps(dat2);

            size = std::max(size / sizeof(T), sizeof(T));

            std::size_t i = 0;
            for ( ; i < size; ++i)
            {
                std::size_t ii = i * 16 / sizeof(T);
                detail::scalar_div_impl<T>::div(dat1 + ii, v2, out + ii);
            }
        }
    };

    /*! SIMD scalar division / matrix size 2
     */
    template <>
    struct scalar_div<float, 2> {

        static inline void div(const float* dat1, const float dat2, float* out, std::size_t) {

            const __m128 v2 = _mm_set1_ps(dat2);
            detail::scalar_div_impl<float>::div(dat1, v2, out); //> 1 pass
        }
    };

    /*! SIMD scalar division / matrix size 3
     */
    template <>
    struct scalar_div<float, 3> {

        static inline void div(const float* dat1, const float dat2, float* out, std::size_t) {

            const __m128 v2 = _mm_set1_ps(dat2);
            detail::scalar_div_impl<float>::div(dat1, v2, out); //> 1 pass
        }
    };

    /*! SIMD scalar division / matrix size 4
     */
    template <>
    struct scalar_div<float, 4> {

        static inline void div(const float* dat1, const float dat2, float* out, std::size_t) {

            const __m128 v2 = _mm_set1_ps(dat2);
            detail::scalar_div_impl<float>::div(dat1, v2, out); //> 1 pass
        }
    };

    /*! SIMD scalar division / matrix size 9
     */
    template <>
    struct scalar_div<float, 9> {

        // Index offset
        static const unsigned i1 = 4;

        static inline void div(const float* dat1, const float dat2, float* out, std::size_t) {

            const __m128 v2 = _mm_set1_ps(dat2);
            // 2 passes
            detail::scalar_div_impl<float>::div(dat1, v2, out);
            detail::scalar_div_impl<float>::div(dat1 + i1, v2, out + i1);
        }
    };

    /*! SIMD scalar division / matrix size 16
     */
    template <>
    struct scalar_div<float, 16> {

        // Index offsets...
        static const unsigned i1 =  4;
        static const unsigned i2 =  8;
        static const unsigned i3 = 12;

        static inline void div(const float* dat1, const float dat2, float* out, std::size_t) {

            const __m128 v2 = _mm_set1_ps(dat2);
            // 4 passes
            detail::scalar_div_impl<float>::div(dat1, v2, out);
            detail::scalar_div_impl<float>::div(dat1 + i1, v2, out + i1);
            detail::scalar_div_impl<float>::div(dat1 + i2, v2, out + i2);
            detail::scalar_div_impl<float>::div(dat1 + i3, v2, out + i3);
        }
    };
}

#endif
