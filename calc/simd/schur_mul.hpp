#pragma once

#ifndef _CALC_SIMD_SCHUR_MUL_HPP
#define _CALC_SIMD_SCHUR_MUL_HPP

#include "common.hpp"

namespace calc {

    namespace detail {

        template <typename>
        struct schur_mul_impl;

        template <>
        struct schur_mul_impl<float> {

            static void mul(const float* dat1, const float* dat2, float* out) {

                __m128 v1 = _mm_load_ps(dat1);
                __m128 v2 = _mm_load_ps(dat2);

                __m128 v3 = _mm_mul_ps(v1, v2);
                _mm_store_ps(out, v3);
            }
        };

        template <>
        struct schur_mul_impl<double> {

            static void mul(const double* dat1, const double* dat2, double* out) {

                __m128d v1 = _mm_load_pd(dat1);
                __m128d v2 = _mm_load_pd(dat2);

                __m128d v3 = _mm_mul_pd(v1, v2);
                _mm_store_pd(out, v3);
            }
        };

        /// functor schur_mul
        /*! SIMD schur multiplication 
         */
        template <typename T>
        struct schur_mul {

            static inline void mul(const T* dat1, const T* dat2, T* out, std::size_t size) {

                std::size_t i = 0;
                for ( ; i < size; i += sizeof(T))
                {
                    const std::size_t ii = i * 4;
                    schur_mul_impl<T>::mul(dat1 + ii, dat2 + ii, out + ii);
                }
            }
        };
#if 0
        /*! SIMD schur multiplication / 16 byte operation
         */
        template <typename T>
        struct schur_mul<T, 16> {

            static inline void mul(const T* dat1, const T* dat2, T* out, std::size_t) {
                // 1 pass
                schur_mul_impl<T>::mul(dat1, dat2, out);
            }
        };

        /*! SIMD schur multiplication / 32 byte operation
         */
        template <typename T>
        struct schur_mul<T, 32> {

            static const unsigned i1 = 16 / sizeof(T);

            static inline void mul(const T* dat1, const T* dat2, T* out, std::size_t) {
                // 2 passes
                schur_mul_impl<T>::mul(dat1, dat2, out);
                schur_mul_impl<T>::mul(dat1 + i1, dat2 + i1, out + i1);
            }
        };

        /*! SIMD schur multiplication / 48 byte operation
         */
        template <typename T>
        struct schur_mul<T, 48> {

            static const unsigned i1 = 16 / sizeof(T);
            static const unsigned i2 = 32 / sizeof(T);

            static inline void mul(const T* dat1, const T* dat2, T* out, std::size_t) {
                // 3 passes
                schur_mul_impl<T>::mul(dat1, dat2, out);
                schur_mul_impl<T>::mul(dat1 + i1, dat2 + i1, out + i1);
                schur_mul_impl<T>::mul(dat1 + i2, dat2 + i2, out + i2);
            }
        };

        /*! SIMD schur multiplication / 64 byte operation
         */
        template <typename T>
        struct schur_mul<T, 64> {

            static const unsigned i1 = 16 / sizeof(T);
            static const unsigned i2 = 32 / sizeof(T);
            static const unsigned i3 = 48 / sizeof(T);

            static inline void mul(const T* dat1, const T* dat2, T* out, std::size_t) {
                // 4 passes
                schur_mul_impl<T>::mul(dat1, dat2, out);
                schur_mul_impl<T>::mul(dat1 + i1, dat2 + i1, out + i1);
                schur_mul_impl<T>::mul(dat1 + i2, dat2 + i2, out + i2);
                schur_mul_impl<T>::mul(dat1 + i3, dat2 + i3, out + i3);
            }
        };
#endif
    }
}

#endif
