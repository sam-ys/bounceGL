#pragma once

#ifndef _CALC_SIMD_COMMON_HPP
#define _CALC_SIMD_COMMON_HPP

#include <immintrin.h>

#define __stride__(a) (16 / (a))

namespace calc {

    namespace detail {

        template <typename>
        struct __m;

        template <>
        struct __m<float> {
            typedef __m128 T;
        };

        template <>
        struct __m<double> {
            typedef __m128d T;
        };

        template <typename U> inline typename detail::__m<U>::T load(const U*);
        template <typename U> inline typename detail::__m<U>::T loadu(const U*);

        template <> inline __m128 load(const float* dat) { return _mm_load_ps(dat); }
        template <> inline __m128d load(const double* dat) { return _mm_load_pd(dat); }

        template <> inline __m128 loadu(const float* dat) { return _mm_loadu_ps(dat); }
        template <> inline __m128d loadu(const double* dat) { return _mm_loadu_pd(dat); }

        template <typename U> void store(U*, typename detail::__m<U>::T);
        template <typename U> void storeu(U*, typename detail::__m<U>::T);

        template <> inline void store(float* dat, __m128 fill) { _mm_store_ps(dat, fill); }
        template <> inline void store(double* dat, __m128d fill) { _mm_store_pd(dat, fill); }
    }
}

#endif
