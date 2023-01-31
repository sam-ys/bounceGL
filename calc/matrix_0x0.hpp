#pragma once

#ifndef _CALC_MATRIX_0x0_HPP
#define _CALC_MATRIX_0x0_HPP

#include <new>

#include "matrix_nxm.hpp"
#include "mem.hpp"

#define __max__(a, b) (((a) > (b)) ? (a) : (b))
#define __min__(a, b) (((a) < (b)) ? (a) : (b))

namespace calc {

    template <typename T>
    class matrix<T, 0, 0> {

        std::size_t nrows_; //> # rows
        std::size_t ncols_; //> # cols

        T*          buffer_; //> Data buffer
        std::size_t capacity_; //> Buffer capacity

    public:

        static matrix<T> identity(std::size_t size, const T eigenout = 1) {

            matrix<T> out(T(0));
            for (std::size_t i = 0; i != size; ++i)
                out(i, i) = eigenout;
            return out;
        }

        operator T*() {
            return buffer_;
        }

        operator const T*() const {
            return buffer_;
        }

        /// dtor.
        ~matrix() {
            // Clean up
            if (buffer_ != nullptr) {
                delmap<T>(buffer_, size());
            }
        }

        /// ctor.
        matrix() : nrows_(0), ncols_(0), capacity_(0) {  }

        /// ctor.
        /// @param nrows - # rows
        /// @param ncols - # cols
        matrix(std::size_t nrows, std::size_t ncols) : nrows_(nrows)
                                                     , ncols_(ncols)
                                                     , capacity_(nrows * ncols) {
            buffer_ = static_cast<T*>(genmap<T>(&capacity_));

            // Just in case
            if (!buffer_) {
                throw;
            }

            std::memset(buffer_, 0, capacity_ * sizeof(T));
        }

        /// ctor.
        /// @param nrows - # rows
        /// @param ncols - # cols
        /// @param fill - fill value
        matrix(std::size_t nrows, std::size_t ncols,
               const typename std::enable_if<std::is_same<T, float>::value, T>::type fill) : nrows_(nrows)
                                                                                           , ncols_(ncols)
                                                                                           , capacity_(nrows * ncols) {
            buffer_ = static_cast<T*>(genmap<T>(&capacity_));
            // Just in case
            if (!buffer_) {
                throw std::bad_alloc();
            }

            for (unsigned i = 0; i != size(); ++i) {
                buffer_[i] = fill;
            }
        }

        /// ctor.
        /// @param nrows - # rows
        /// @param ncols - # cols
        /// @param fill - array of values
        matrix(std::size_t nrows, std::size_t ncols, const T* fill) : nrows_(nrows)
                                                                    , ncols_(ncols)
                                                                    , capacity_(nrows * ncols) {
            buffer_ = static_cast<T*>(genmap<T>(&capacity_));
            // Just in case
            if (!buffer_) {
                throw;
            }

            std::memset(buffer_, 0, capacity_ * sizeof(T));
            std::memcpy(buffer_, fill, nrows * ncols * sizeof(T));
        }

        /// Move ctor.
        /// @param value - source matrix
        matrix(matrix<T, 0, 0>&& m) : nrows_(m.rows())
                                    , ncols_(m.cols())
                                    , buffer_(m.buffer_)
                                    , capacity_(m.capacity_) {
            m.nrows_ = 0;
            m.ncols_ = 0;
            m.buffer_ = nullptr;
            m.capacity_ = 0;
        }

        /// Swaps values with another matrix
        /// @param value - source matrix
        void swap(matrix<T, 0, 0>& m) {

            nrows_   ^= m.nrows_;
            m.nrows_ ^= nrows_;
            nrows_   ^= m.nrows_;

            ncols_   ^= m.ncols_;
            m.ncols_ ^= ncols_;
            ncols_   ^= m.ncols_;

            buffer_   ^= m.buffer_;
            m.buffer_ ^= buffer_;
            buffer_   ^= m.buffer_;
        }

        /// @return the number of rows
        std::size_t rows() const {
            return nrows_;
        }

        /// @return the number of columns
        std::size_t cols() const {
            return ncols_;
        }

        /// @return the total size
        std::size_t size() const {
            return nrows_ * ncols_;
        }

        T& operator()(const std::size_t r, const std::size_t c) {
            return buffer_[(r * size()) + c];
        }

        const T& operator()(const std::size_t r, const std::size_t c) const {
            return buffer_[(r * size()) + c];
        }

#if 0
        /// Matrix multiplication
        matrix<T, 0, 0> operator*(const matrix<T, 0, 0>& rhs) const {

            /**/ assert(cols() == rhs.rows());

            matrix<T> out(rows(), rhs.cols());
            matrix_mul<T, 0, 0, 0>::mul(buffer_, rhs.buffer_, out.buffer_, rows(), cols(), rhs.cols());
            return out;
        }

        /// Scalar multiplication
        matrix<T, 0, 0> operator*(const T scalar) const {

            matrix<T> out(rows(), cols());
            scalar_mul<T, sizeof(T)>::mul(buffer_, scalar, out.buffer_, rows(), cols());
            return out;
        }

        /// Scalar division
        matrix<T, 0, 0> operator/(const T scalar) const {

            matrix<T> out(rows(), cols());
            scalar_div<T, sizeof(T)>::div(buffer_, scalar, out.buffer_, rows(), cols());
            return out;
        }

        /// Scalar multiplication
        matrix<T, 0, 0>& operator*=(const T scalar) {

            scalar_mul<T, sizeof(T)>::mul(buffer_, scalar, buffer_);
            return *this;
        }

        /// Scalar division
        matrix<T, 0, 0>& operator/=(const T scalar) {

            scalar_div<T, sizeof(T)>::div(buffer_, scalar, buffer_);
            return *this;
        }

        matrix<T, 0, 0> operator+(const matrix<T, 0, 0>& rhs) const {

            /**/ assert(rows() == rhs.rows());
            /**/ assert(cols() == rhs.cols());

            matrix<T, 0, 0> out;
            matrix_add<T, sizeof(T)>::add(buffer_, rhs.buffer_, out.buffer_, rows(), cols());
            return out;
        }

        matrix<T, 0, 0>& operator+=(const matrix<T, 0, 0>& rhs) {

            /**/ assert(rows() == rhs.rows());
            /**/ assert(cols() == rhs.cols());

            matrix_add<T, sizeof(T)>::add(buffer_, rhs.buffer_, buffer_, rows(), cols());
            return *this;
        }

        matrix<T, 0, 0> operator-(const matrix<T, 0, 0>& rhs) const {

            /**/ assert(rows() == rhs.rows());
            /**/ assert(cols() == rhs.cols());

            matrix<T, 0, 0> out;
            matrix_sub<T, sizeof(T)>::sub(buffer_, rhs.buffer_, out.buffer_, rows(), cols());
            return out;
        }

        matrix<T, 0, 0>& operator-=(const matrix<T, 0, 0>& rhs) {

            /**/ assert(rows() == rhs.rows());
            /**/ assert(cols() == rhs.cols());

            matrix<T, 0, 0> out;
            matrix_sub<T, sizeof(T)>::sub(buffer_, rhs.buffer_, buffer_, rows(), cols());
            return *this;
        }
#endif
    };

#if 0
    template <typename T>
    matrix<T, 0, 0> rot_090(const matrix<T, 0, 0>& m)
    {
        matrix<T, 0, 0> out(m.cols(), m.rows());

        std::size_t size  = out.size();
        std::size_t nrows = out.rows();
        std::size_t ncols = out.cols();

        for (std::size_t i = 0; i != size; ++i)
        {
            std::size_t r = i / nrows;
            std::size_t c = i % nrows;
            out(c, ncols - r - 1) = m(i);
        }

        return out;
    }

    template <typename T>
    matrix<T, 0, 0> rot_180(const matrix<T, 0, 0>& m)
    {
        matrix<T, 0, 0> out(m.rows(), m.cols());

        std::size_t size  = out.size();
        std::size_t nrows = out.rows();
        std::size_t ncols = out.cols();

        for (std::size_t i = 0; i != size; ++i)
        {
            std::size_t r = i / ncols;
            std::size_t c = i % ncols;
            out(nrows - r - 1, ncols - c - 1) = m(i);
        }

        return out;
    }

    template <typename T>
    matrix<T, 0, 0> rot_270(const matrix<T, 0, 0>& m)
    {
        matrix<T, 0, 0> out(m.cols(), m.rows());

        std::size_t size  = out.size();
        std::size_t nrows = out.rows();

        for (std::size_t i = 0; i != size; ++i)
        {
            std::size_t r = i / nrows;
            std::size_t c = i % nrows;
            out(nrows - c - 1, r) = m(i);
        }

        return out;
    }
#endif
}

#endif
