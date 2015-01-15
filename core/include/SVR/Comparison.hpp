#ifndef _SVR_COMPARISON_HPP_
#define _SVR_COMPARISON_HPP_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace SVR
{

/**
 * Float epsilon
 */
const float FloatEpsilon = 0.000001;

/**
 * Double epsilon
 */
const double DoubleEpsilon = 0.0000000001;

/**
 * closeTo definition
 */
inline bool closeTo(float a, float b)
{
    auto dif = a - b;
    return -FloatEpsilon <= dif && dif <= FloatEpsilon;
}

inline bool closeTo(double a, double b)
{
    auto dif = a - b;
    return -DoubleEpsilon <= dif && dif <= DoubleEpsilon;
}

inline bool closeTo(const glm::vec2 &a, const glm::vec2 &b)
{
    return closeTo(a.x, b.x) && closeTo(a.y, b.y);
}

inline bool closeTo(const glm::vec3 &a, const glm::vec3 &b)
{
    return closeTo(a.x, b.x) && closeTo(a.y, b.y) && closeTo(a.z, b.z);
}

inline bool closeTo(const glm::vec4 &a, const glm::vec4 &b)
{
    return closeTo(a.x, b.x) && closeTo(a.y, b.y) && closeTo(a.z, b.z) && closeTo(a.w, b.w);
}

inline bool closeTo(const glm::quat &a, const glm::quat &b)
{
    return closeTo(a.x, b.x) && closeTo(a.y, b.y) && closeTo(a.z, b.z) && closeTo(a.w, b.w);
}

inline bool closeTo(const glm::mat4 &a, const glm::mat4 &b)
{
    return closeTo(a[0], b[0]) && closeTo(a[1], b[1]) && closeTo(a[2], b[2]) && closeTo(a[3], b[3]);
}

} //namespace SVR

#endif //_SVR_COMPARISON_HPP_

