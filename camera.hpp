#pragma once

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "calc/matrix.hpp"

//! @struct ray
/*! unnormalized ray
 */
struct ray {

    float x,y;
    calc::vec3f origin;
    calc::vec3f direction;

    calc::vec4f fRay; // Far ray
    calc::vec4f nRay; // Near ray
};

//! @class Camera
/*! user-modifiable Camera: calculates perspective projection and look-at transformations
 */
class Camera {
public:

    /// ctor.
    Camera(const calc::vec3f& eye,
           float fov,
           float zfar,
           float znear = 0.1,
           unsigned screenWidth = 0,
           unsigned screenHeight = 0);
    // Transforms
    void move(const calc::vec3f& direction);
    // Transforms
    void reset();
    // Transforms
    void resize(int screenWidth, int screenHeight);
    // Transform
    void set_scene_rotation(const float pitch, const float yaw, const float roll);
    // Transform
    void rotate_viewer(const float pitch, const float yaw, const float roll);

    /// @param x, y mouse coordinates
    /// @return ray
    ray unproject(float x, float y) const;
    /// @param x, y mouse coordinates
    /// @return ray
    ray unproject(float x, float y, const calc::mat4f& lookAt, const calc::mat4f& projection) const;

    /// Recalculates look-at and projection matrices;
    /// should be alwas called after moving or rotating camera
    void update();

    float get_pitch() const {
        return viewOrientation_.pitch;
    }

    float get_yaw() const {
        return viewOrientation_.yaw;
    }

    float get_roll() const {
        return viewOrientation_.roll;
    }

    float get_screen_width() const; ///> @return the stored screen width
    float get_screen_height() const; ///> @return the stored screen height

    const calc::vec3f& get_position() const; ///> @return the eye coordinates

    const calc::mat4f& get_scene() const; ///> @return the projection x view matrix
    const calc::mat4f& get_device_scene() const; ///> @return the projection x view matrix in column-major ordering

    const calc::mat4f& get_look_at() const; ///> @return the view matrix
    const calc::mat4f& get_device_look_at() const; ///> @return the view matrix

    const calc::mat4f& get_projection() const; ///> @return the projection matrix
    const calc::mat4f& get_device_projection() const; ///> @return the projection matrix

private:

    void calc_look_at(); //> Helper
    void calc_projection(); //> Helper

    float screenWidth_; //> Screen width
    float screenHeight_; //> Screen height

    //! struct orientation
    /*! 3d space orientatoin
     */
    struct orientation {
        float pitch, yaw, roll;
        orientation() : pitch(0), yaw(0), roll(0) {}
    };

    orientation viewOrientation_; //> Current 3d orientation of scene
    orientation viewerOrientation_; //> Current 3d orientation of first-person viewer

    //! struct fov
    /*! Field of view
     */
    struct fov {
        float half, znear, zfar;
        fov(float half, float znear, float zfar) : half(half), znear(znear), zfar(zfar) {}
    };

    fov fov_; //. Current field of view

    //! struct vector_pair
    /*! vector and its default (initial) value
     */
    struct vector_pair {
        calc::vec3f value;
        const calc::vec3f defaultValue;
        vector_pair(const calc::vec3f& value);
    };

    vector_pair E_; //> Eye position vector
    vector_pair F_; //> Forward direction vector
    vector_pair U_; //> Up direction vector

    //! struct matrix_pair
    /*! matrix and its (transposed) gpu device-friendly pair
     */
    struct matrix_pair {
        calc::mat4f value, deviceValue;
    };

    matrix_pair lookAt_; //> View matrix
    matrix_pair projection_; //> Perspective projection matrix
    matrix_pair scene_; //> Perspective x view
};

#endif
