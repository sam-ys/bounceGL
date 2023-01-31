#include "camera.hpp"

Camera::vector_pair::vector_pair(const calc::vec3f& value) : value(value)
                                                           , defaultValue(value) {}

/*! ctor.
 */
Camera::Camera(const calc::vec3f& eye,
               float fov,
               float zfar,
               float znear,
               unsigned screenWidth,
               unsigned screenHeight) : screenWidth_(screenWidth)
                                      , screenHeight_(screenHeight)
                                      , fov_(fov, znear, zfar)
                                      , E_(eye)
                                      , F_(calc::vec3f(0, 0, 1))
                                      , U_(calc::vec3f(0, 1, 0)) {
    update();
}

void Camera::move(const calc::vec3f& direction)
{
    const calc::mat3f rot = calc::rotate_3x(calc::radians(viewerOrientation_.pitch))
        * calc::rotate_3y(calc::radians(viewerOrientation_.yaw))
        * calc::rotate_3z(calc::radians(viewerOrientation_.roll));

    E_.value -= rot * direction;

#if 0
    const calc::mat4f rot = calc::rotate_4x(calc::radians(viewerOrientation_.pitch))
        * calc::rotate_4y(calc::radians(viewerOrientation_.yaw))
        * calc::rotate_4z(calc::radians(viewerOrientation_.roll));

    const calc::vec4f eye = rot * calc::vec4f(direction, 0);

    (E_.value)[0] -= eye[0];
    (E_.value)[1] -= eye[1];
    (E_.value)[2] -= eye[2];
#endif
}

void Camera::reset()
{
    viewOrientation_ = orientation();
    viewerOrientation_ = orientation();

    (E_.value) = (E_.defaultValue);
    (F_.value) = (F_.defaultValue);
    (U_.value) = (U_.defaultValue);
}

void Camera::resize(int screenWidthidth, int screenHeighteight)
{
    screenWidth_ = screenWidthidth;
    screenHeight_ = screenHeighteight;
}

void Camera::rotate_scene(const float pitch, const float yaw, const float roll)
{
    viewOrientation_.pitch += pitch;
    viewOrientation_.yaw += yaw;
    viewOrientation_.roll += roll;
}

void Camera::rotate_viewer(const float pitch, const float yaw, const float roll)
{
    viewerOrientation_.pitch += pitch;
    viewerOrientation_.yaw += yaw;
    viewerOrientation_.roll += roll;

    const calc::mat3f rot = calc::rotate_3z(calc::radians(viewerOrientation_.roll))
        * calc::rotate_3y(calc::radians(viewerOrientation_.yaw))
        * calc::rotate_3x(calc::radians(viewerOrientation_.pitch));

    U_.value = rot * U_.defaultValue;
    F_.value = rot * F_.defaultValue;

#if 0
    const calc::mat4f rot = calc::rotate_4z(calc::radians(viewerOrientation_.roll))
        * calc::rotate_4y(calc::radians(viewerOrientation_.yaw))
        * calc::rotate_4x(calc::radians(viewerOrientation_.pitch));

    const calc::vec4f u = rot * calc::vec4f(U_.defaultValue, 0);
    const calc::vec4f f = rot * calc::vec4f(F_.defaultValue, 0);

    U_.value = calc::vec3f(u[0], u[1], u[2]);
    F_.value = calc::vec3f(f[0], f[1], f[2]);
#endif
}

void Camera::calc_look_at()
{
    // Calculate the transposed rotation matrix
    calc::mat4f lookAt = calc::mat4f::identity();
    // Forward
    const calc::vec3f f = calc::normal(F_.value);
    // Left
    const calc::vec3f s = calc::normal(calc::cross(f, U_.value));
    // Up (recalculated and normalized)
    const calc::vec3f u = calc::cross(s, f);

    lookAt(0, 0) = s[0];
    lookAt(0, 1) = s[1];
    lookAt(0, 2) = s[2];

    lookAt(1, 0) = u[0];
    lookAt(1, 1) = u[1];
    lookAt(1, 2) = u[2];

    lookAt(2, 0) = -f[0];
    lookAt(2, 1) = -f[1];
    lookAt(2, 2) = -f[2];

    lookAt(0, 3) = -calc::dot(s, E_.value);
    lookAt(1, 3) = -calc::dot(u, E_.value);
    lookAt(2, 3) =  calc::dot(f, E_.value);

    lookAt_.value = lookAt
        * calc::rotate_4x(calc::radians(viewOrientation_.pitch))
        * calc::rotate_4y(calc::radians(viewOrientation_.yaw))
        * calc::rotate_4z(calc::radians(viewOrientation_.roll));
    lookAt_.deviceValue = calc::transpose(lookAt_.value);
}

void Camera::calc_projection()
{
    const float aspect = screenWidth_ / screenHeight_;
    const float tanHalfFOV = std::tan(calc::radians(fov_.half));

    float zfar = fov_.zfar;
    float znear = fov_.znear;

    projection_.value(0, 0) =  1.0 / (tanHalfFOV * aspect);
    projection_.value(1, 1) =  1.0 / tanHalfFOV;
    projection_.value(2, 2) = -(zfar + znear) / (zfar - znear);
    projection_.value(2, 3) = -2.0 * zfar * znear / (zfar - znear);
    projection_.value(3, 2) = -1.0;
    projection_.value(3, 3) =  0.0;

    projection_.deviceValue = calc::transpose(projection_.value);
}

namespace {

    // Helper
    // Hadamard product
    /* TODO: move this */
    inline calc::vec4f operator*(const calc::vec4f& v1, const calc::vec4f& v2)
    {
        float a = v1[0] * v2[0];
        float b = v1[1] * v2[1];
        float c = v1[2] * v2[2];
        float d = v1[3] * v2[3];

        return calc::vec4f(a, b, c, d);
    }

    // Helper
    // @param m matrix
    // @return inverse of m
    calc::mat4f inverse(const calc::mat4f& m)
    {
        float coef00 = m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2);
        float coef02 = m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1);
        float coef03 = m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1);

        float coef04 = m(1, 2) * m(3, 3) - m(1, 3) * m(3, 2);
        float coef06 = m(1, 1) * m(3, 3) - m(1, 3) * m(3, 1);
        float coef07 = m(1, 1) * m(3, 2) - m(1, 2) * m(3, 1);

        float coef08 = m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2);
        float coef10 = m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1);
        float coef11 = m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1);

        float coef12 = m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2);
        float coef14 = m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1);
        float coef15 = m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1);

        float coef16 = m(0, 2) * m(2, 3) - m(0, 3) * m(2, 2);
        float coef18 = m(0, 1) * m(2, 3) - m(0, 3) * m(2, 1);
        float coef19 = m(0, 1) * m(2, 2) - m(0, 2) * m(2, 1);

        float coef20 = m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2);
        float coef22 = m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1);
        float coef23 = m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1);

        calc::vec4f fac0(coef00, coef00, coef02, coef03);
        calc::vec4f fac1(coef04, coef04, coef06, coef07);
        calc::vec4f fac2(coef08, coef08, coef10, coef11);
        calc::vec4f fac3(coef12, coef12, coef14, coef15);
        calc::vec4f fac4(coef16, coef16, coef18, coef19);
        calc::vec4f fac5(coef20, coef20, coef22, coef23);

        calc::vec4f vec0(m(0, 1), m(0, 0), m(0, 0), m(0, 0));
        calc::vec4f vec1(m(1, 1), m(1, 0), m(1, 0), m(1, 0));
        calc::vec4f vec2(m(2, 1), m(2, 0), m(2, 0), m(2, 0));
        calc::vec4f vec3(m(3, 1), m(3, 0), m(3, 0), m(3, 0));

        calc::vec4f inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
        calc::vec4f inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
        calc::vec4f inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
        calc::vec4f inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

        calc::vec4f signA(+1, -1, +1, -1);
        calc::vec4f signB(-1, +1, -1, +1);

        const calc::vec4f is0 = inv0 * signA;
        const calc::vec4f is1 = inv1 * signB;
        const calc::vec4f is2 = inv2 * signA;
        const calc::vec4f is3 = inv3 * signB;

        const calc::mat4f inverseMat(is0[0], is1[0], is2[0], is3[0],
                                     is0[1], is1[1], is2[1], is3[1],
                                     is0[2], is1[2], is2[2], is3[2],
                                     is0[3], is1[3], is2[3], is3[3]);

        const calc::vec4f row0(inverseMat(0, 0), inverseMat(0, 1), inverseMat(0, 2), inverseMat(0, 3));

        const calc::vec4f dot0(calc::vec4f(m(0,0), m(0,1), m(0, 2), m(0, 3)) * row0);

        float dot1 = (dot0[0] + dot0[1]) + (dot0[2] + dot0[3]);
        float oneOverDeterminant = 1.0 / dot1;

        return inverseMat * oneOverDeterminant;
    }

    // Helper
    ray unproject_impl(float x, float y, float screenWidth, float screenHeight, const calc::mat4f& scene)
    {
        y = screenHeight - y - 1;

        x = 2 * x / screenWidth - 1;
        y = 2 * y / screenHeight - 1;

        const calc::mat4f inv = inverse(scene);

        ray r;
        r.x = x;
        r.y = y;

        r.fRay = inv * calc::vec4f(x, y, 1, 1);
        r.nRay = inv * calc::vec4f(x, y, 0, 1);

        r.fRay = r.fRay / r.fRay[3];
        r.nRay = r.nRay / r.nRay[3];

        const calc::vec4f dir = calc::normal(r.fRay - r.nRay);

        r.origin = calc::vec3f(r.nRay[0], r.nRay[1], r.nRay[2]);
        r.direction = calc::vec3f(dir[0], dir[1], dir[2]);
        return r;
    }
}

ray Camera::unproject(float x, float y) const
{
    const calc::mat4f& scene = scene_.value;
    return unproject_impl(x, y, screenWidth_, screenHeight_, scene);
}

ray Camera::unproject(float x, float y, const calc::mat4f& lookAt, const calc::mat4f& projection) const {

    const calc::mat4f scene = projection * lookAt;
    return unproject_impl(x, y, screenWidth_, screenHeight_, scene);
}

void Camera::update()
{
    calc_look_at();
    calc_projection();

    scene_.value = projection_.value * lookAt_.value;
    scene_.deviceValue = calc::transpose(scene_.value);
}

float Camera::get_screen_width() const {
    return screenWidth_;
}

float Camera::get_screen_height() const {
    return screenHeight_;
}

const calc::vec3f& Camera::get_position() const {
    return E_.value;
}

const calc::mat4f& Camera::get_scene() const {
    return scene_.value;
}

const calc::mat4f& Camera::get_device_scene() const {
    return scene_.value;
}

const calc::mat4f& Camera::get_look_at() const {
    return lookAt_.value;
}

const calc::mat4f& Camera::get_device_look_at() const {
    return lookAt_.deviceValue;
}

const calc::mat4f& Camera::get_projection() const {
    return projection_.value;
}

const calc::mat4f& Camera::get_device_projection() const {
    return projection_.deviceValue;
}
