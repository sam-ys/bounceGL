#pragma once

#ifndef BALL_DATA_HPP
#define BALL_DATA_HPP

#include "calc/matrix.hpp"

//! struct BallData
/*! Defines a moving ball
 */
struct BallData {
    // Ball sprite index
    unsigned selectedSkin;
    // Ball direction
    calc::vec3f direction;
    // Ball speed
    calc::vec3f speed;
    // Ball turn rate
    calc::vec3f turnRate;
    // Current ball postition
    calc::mat4f translation;
    /*! ctor.
     */
    BallData() : selectedSkin(0) , direction(1.0, 1.0, 0) , speed(0, 0, 0) , translation(calc::mat4f::identity()) {}
};

#endif
