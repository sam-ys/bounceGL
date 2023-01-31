#pragma once

#ifndef _CALC_TRANSFORM_HPP
#define _CALC_TRANSFORM_HPP

#include "calc/matrix.hpp"

namespace calc {

    /// 3.1415926...
    static const float PI = ::atan(1.0) * 4;

    /// @return angle in radians
    static inline float radians(const float deg) {
        return PI * deg / 180.0;
    }

    /// @return rotation matrix
    static inline mat4f rotate_4x(const float rad)
    {
        const float r[] = {
            1, 0, 0, 0,
            0, static_cast<float>(std::cos(rad)), static_cast<float>(-std::sin(rad)), 0,
            0, static_cast<float>(std::sin(rad)), static_cast<float>(+std::cos(rad)), 0,
            0, 0, 0, 1,
        };

        return mat4f(r);
    }

    /// @return rotation matrix
    static inline mat3f rotate_3x(const float rad)
    {
        const float r[] = {
            1, 0, 0,
            0, static_cast<float>(std::cos(rad)), static_cast<float>(-std::sin(rad)),
            0, static_cast<float>(std::sin(rad)), static_cast<float>(+std::cos(rad)),
        };

        return mat3f(r);
    }

    /// @return rotation matrix
    static inline mat4f rotate_4y(const float rad)
    {
        const float r[] = {
            static_cast<float>(+std::cos(rad)), 0, static_cast<float>(std::sin(rad)), 0,
            0, 1, 0, 0,
            static_cast<float>(-std::sin(rad)), 0, static_cast<float>(std::cos(rad)), 0,
            0, 0, 0, 1,
        };

        return mat4f(r);
    }

    /// @return rotation matrix
    static inline mat3f rotate_3y(float rad)
    {
        const float r[] = {
            static_cast<float>(+std::cos(rad)), 0, static_cast<float>(std::sin(rad)),
            0, 1, 0,
            static_cast<float>(-std::sin(rad)), 0, static_cast<float>(std::cos(rad)),
            0, 0, 1,
        };

        return mat3f(r);
    }

    /// @return rotation matrix
    static inline mat4f rotate_4z(const float rad)
    {
        const float r[] = {
            static_cast<float>(std::cos(rad)), static_cast<float>(-std::sin(rad)), 0, 0,
            static_cast<float>(std::sin(rad)), static_cast<float>(+std::cos(rad)), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };

        return mat4f(r);
    }

    /// @return rotation matrix
    static inline mat3f rotate_3z(const float rad)
    {
        const float r[] = {
            static_cast<float>(std::cos(rad)), static_cast<float>(-std::sin(rad)), 0,
            static_cast<float>(std::sin(rad)), static_cast<float>(+std::cos(rad)), 0,
            0, 0, 1,
        };

        return mat3f(r);
    }
}

#endif
