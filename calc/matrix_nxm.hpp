#pragma once

#ifndef _CALC_MATRIX_NXM_HPP
#define _CALC_MATRIX_NXM_HPP

#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>
#include <type_traits>

#ifndef __NO_USE_SIMD__
#include "simd/matrix_add.hpp"
#include "simd/matrix_mul.hpp"
#include "simd/matrix_sub.hpp"
#include "simd/scalar_div.hpp"
#include "simd/scalar_mul.hpp"
#endif

// Padds to alignment size
#define __padd__(a) (((a) == 0) || ((a) % 16) ?    ((a) + 16 - ((a) % 16))    :    (a))

namespace calc {

    template <typename T__,
              unsigned N__,
              unsigned M__>
    class matrix {

        // Row-major ordered matrix data
        T__ buffer_[__padd__(N__ * M__)] __attribute__((aligned(16)));

    public:

        static matrix<T__, N__, M__> identity(const T__ eigenout = 1) {

            matrix<T__, N__, M__> out(T__(0));
            for (unsigned i = 0; i != N__; ++i)
                out(i, i) = eigenout;
            return out;
        }

        operator T__*() {
            return buffer_;
        }

        operator const T__*() const {
            return buffer_;
        }

        /// ctor.
        matrix() {
            std::memset(buffer_, 0, sizeof(buffer_));
        }

        /// ctor.
        matrix(const typename std::enable_if<std::is_same<T__, float>::value, T__>::type fill) {

            for (unsigned i = 0; i != N__ * M__; ++i) {
                buffer_[i] = fill;
            }
        }

        /// ctor.
        explicit matrix(const T__* fill) {

            std::memset(buffer_, 0, sizeof(buffer_));
            std::memcpy(buffer_, fill, N__ * M__ * sizeof(float));
        }

        /// ctor.
        template <unsigned N1,
                  unsigned M1,
                  unsigned N = N__,
                  unsigned M = M__>
        matrix(const matrix<T__, N1, M1>& v, typename std::enable_if<N * M == 4 && (N == 1 || M == 1) && (N1 * M1 == 2), T__>::type x2, T__ x3) {

            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = v[0];
            buffer_[1] = v[1];
            buffer_[2] = x2;
            buffer_[3] = x3;
        }

        /// ctor.
        template <unsigned N1,
                  unsigned M1,
                  unsigned N = N__,
                  unsigned M = M__>
        matrix(const matrix<T__, N1, M1>& v, typename std::enable_if<N * M == 4 && (N == 1 || M == 1) && (N1 * M1 == 3), T__>::type x3) {

            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = v[0];
            buffer_[1] = v[1];
            buffer_[2] = v[2];
            buffer_[3] = x3;
        }

        /// ctor.
        template <unsigned N = N__,
                  unsigned M = M__>
        matrix(typename std::enable_if<N * M == 1, T__>::type x0) {

            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
        }

        /// ctor.
        template <unsigned N = N__,
                  unsigned M = M__>
        matrix(typename std::enable_if<N * M == 2, T__>::type x0, T__ x1) {

            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
            buffer_[1] = x1;
        }

        /// ctor.
        template <unsigned N = N__,
                  unsigned M = M__>
        matrix(typename std::enable_if<N * M == 3, T__>::type x0, T__ x1, T__ x2) {

            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
            buffer_[1] = x1;
            buffer_[2] = x2;
        }

        /// ctor.
        template <unsigned N = N__,
                  unsigned M = M__>
        matrix(typename std::enable_if<N * M == 4, T__>::type x0, T__ x1, T__ x2, T__ x3) {

            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
            buffer_[1] = x1;
            buffer_[2] = x2;
            buffer_[3] = x3;
        }

        /// ctor.
        template <unsigned N = N__,
                  unsigned M = M__>
        matrix(typename std::enable_if<N * M == 9, T__>::type x0, T__ x1, T__ x2,
               T__ x3, T__ x4, T__ x5,
               T__ x6, T__ x7, T__ x8) {

            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[0] = x0;
            buffer_[1] = x1;
            buffer_[2] = x2;

            buffer_[3] = x3;
            buffer_[4] = x4;
            buffer_[5] = x5;

            buffer_[6] = x6;
            buffer_[7] = x7;
            buffer_[8] = x8;
        }

        /// ctor.
        template <unsigned N = N__,
                  unsigned M = M__>
        matrix(typename std::enable_if<N * M == 16, T__>::type x0, T__ x1, T__ x2, T__ x3,
               T__ x4,  T__ x5,  T__ x6,  T__ x7,
               T__ x8,  T__ x9,  T__ x10, T__ x11,
               T__ x12, T__ x13, T__ x14, T__ x15) {

            std::memset(buffer_, 0, sizeof(buffer_));
            buffer_[ 0] = x0;
            buffer_[ 1] = x1;
            buffer_[ 2] = x2;
            buffer_[ 3] = x3;

            buffer_[ 4] = x4;
            buffer_[ 5] = x5;
            buffer_[ 6] = x6;
            buffer_[ 7] = x7;

            buffer_[ 8] = x8;
            buffer_[ 9] = x9;
            buffer_[10] = x10;
            buffer_[11] = x11;

            buffer_[12] = x12;
            buffer_[13] = x13;
            buffer_[14] = x14;
            buffer_[15] = x15;
        }

        unsigned rows() const {
            return N__;
        }

        unsigned cols() const {
            return M__;
        }

        unsigned size() const {
            return N__ * M__;
        }

        /// @overload
        template <unsigned N = N__,
                  unsigned M = M__>
        typename std::enable_if<N == 1 || M == 1, T__&>::type operator[](unsigned i) {
            return buffer_[i];
        }

        /// @overload
        template <unsigned N = N__,
                  unsigned M = M__>
        typename std::enable_if<N == 1 || M == 1, const T__&>::type operator[](unsigned i) const {
            return buffer_[i];
        }

        /// @overload
        template <unsigned N = N__,
                  unsigned M = M__>
        typename std::enable_if<(N > 1 && M > 1), T__*>::type operator[](unsigned r) {
            return &buffer_[r * M__];
        }

        /// @overload
        template <unsigned N = N__,
                  unsigned M = M__>
        typename std::enable_if<(N > 1 && M > 1), const T__*>::type operator[](unsigned r) const {
            return &buffer_[r * M__];
        }

        /// @overload
        T__& operator()(const unsigned r, const unsigned c) {
            return buffer_[r * M__ + c];
        }

        /// @overload
        const T__& operator()(const unsigned r, const unsigned c) const {
            return buffer_[r * M__ + c];
        }

        /// @overload
        template <unsigned M1>
        matrix<T__, N__, M1> operator*(const matrix<T__, M__, M1>& rhs) const {
#ifdef __NO_USE_SIMD__
            matrix<T__, N__, M1> out;

            for (unsigned r = 0; r != N__; ++r)
            {
                for (unsigned c = 0; c != M1; ++c)
                {
                    T__ sum = 0;
                    for (unsigned i = 0; i != M__; ++i)
                    {
                        T__ a = (*this)(r, i);
                        T__ b = rhs(i, c);
                        sum += (a * b);
                    }

                    out(r, c) = sum;
                }
            }
#else
            matrix<T__, N__, M1> out;
            matrix_mul<T__, N__, M__, M1>::mul(buffer_, data(rhs), data(out));
#endif
            return out;
        }

        /// @overload
        matrix<T__, N__, M__> operator*(const T__ scalar) const {
#ifdef __NO_USE_SIMD__
            matrix<T__, N__, M__> out;
            for (unsigned i = 0; i != size(); ++i)
                out.buffer_[i] = buffer_[i] * scalar;
#else
            matrix<T__, N__, M__> out;
            scalar_mul<T__, N__ * M__>::mul(buffer_, scalar, out.buffer_, size());
#endif
            return out;
        }

        /// @overload
        matrix<T__, N__, M__> operator/(const T__ scalar) const {
#ifdef __NO_USE_SIMD__
            matrix<T__, N__, M__> out;
            for (unsigned i = 0; i != size(); ++i) {
                out.buffer_[i] = buffer_[i] / scalar;
            }
#else
            matrix<T__, N__, M__> out;
            scalar_div<T__, N__ * M__>::div(buffer_, scalar, out.buffer_, size());
#endif
            return out;
        }

        /// @overload
        template <unsigned M1>
        matrix<T__, N__, M1>& operator*=(const matrix<T__, M__, M1>& rhs) {
#ifdef __NO_USE_SIMD__
            return (*this = (*this * rhs));
#else
            matrix_mul<T__, N__, M__, M1>::mul(buffer_, data(rhs), buffer_);
            return *this;
#endif
        }

        /// @overload
        matrix<T__, N__, M__>& operator*=(const T__ scalar) {
#ifdef __NO_USE_SIMD__
            return (*this = (*this * scalar));
#else
            scalar_mul<T__, matrix<T__, N__, M__>::size()>::mul(buffer_, scalar, buffer_, size());
            return *this;
#endif
        }

        /// @overload
        matrix<T__, N__, M__>& operator/=(const T__ scalar) {
#ifdef __NO_USE_SIMD__
            return (*this = (*this / scalar));
#else
            scalar_div<T__, N__ * M__>::div(buffer_, scalar, buffer_, size());
            return *this;
#endif
        }

        /// @overload
        matrix<T__, N__, M__> operator+(const matrix<T__, N__, M__>& rhs) const {
#ifdef __NO_USE_SIMD__
            matrix<T__, N__, M__> out;
            for (unsigned r = 0; r != N__; ++r)
            {
                for (unsigned c = 0; c != M__; ++c) {
                    out(r, c) = (*this)(r, c) + rhs(r, c);
                }
            }
#else
            matrix<T__, N__, M__> out;
            matrix_add<T__, sizeof(T__)>::add(buffer_, rhs.buffer_, out.buffer_, size());
#endif
            return out;
        }

        /// @overload
        matrix<T__, N__, M__>& operator+=(const matrix<T__, N__, M__>& rhs) {
#ifdef __NO_USE_SIMD__
            return (*this = (*this + rhs));
#else
            matrix_add<T__, sizeof(T__)>::add(buffer_, rhs.buffer_, buffer_, size());
            return *this;
#endif
        }

        /// @overload
        matrix<T__, N__, M__> operator-(const matrix<T__, N__, M__>& rhs) const {
#ifdef __NO_USE_SIMD__
            matrix<T__, N__, M__> out;
            for (unsigned r = 0; r != N__; ++r)
            {
                for (unsigned c = 0; c != M__; ++c) {
                    out(r, c) = (*this)(r, c) - rhs(r, c);
                }
            }
#else
            matrix<T__, N__, M__> out;
            matrix_sub<T__, sizeof(T__)>::sub(buffer_, rhs.buffer_, out.buffer_, size());
#endif
            return out;
        }

        /// @overload
        matrix<T__, N__, M__>& operator-=(const matrix<T__, N__, M__>& rhs) {
#ifdef __NO_USE_SIMD__
            return (*this = (*this + rhs));
#else
            matrix_sub<T__, sizeof(T__)>::sub(buffer_, rhs.buffer_, buffer_, size());
            return *this;
#endif
        }
    };

    /// @overload
    template <typename T__,
              unsigned N__ = 0,
              unsigned M__ = 0>
    matrix<T__, N__, M__> operator-(const matrix<T__, N__, M__>& m) {
        return m * -1;
    }

    /// @overload
    template <typename T__,
              unsigned N__ = 0,
              unsigned M__ = 0>
    matrix<T__, N__, M__> operator*(const T__ scalar, const matrix<T__, N__, M__>& m) {
        return m * scalar;
    }

    // 2x2
    typedef matrix<float, 2, 2> mat2f;
    // 3x3
    typedef matrix<float, 3, 3> mat3f;
    // 4x4
    typedef matrix<float, 4, 4> mat4f;

    // 2x1
    typedef matrix<float, 2, 1> vec2f;
    // 3x1
    typedef matrix<float, 3, 1> vec3f;
    // 4x1
    typedef matrix<float, 4, 1> vec4f;
}

#endif
