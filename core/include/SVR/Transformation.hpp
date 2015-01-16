#ifndef _SVR_TRANSFORMATION_HPP_
#define _SVR_TRANSFORMATION_HPP_

#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SVR
{

template<typename R>
inline R degreesToRadians(R deg)
{
    return deg*float(M_PI/180.0);
}

template<typename R>
inline R radiansToDegrees(R rad)
{
    return rad*180.0/M_PI;
}

inline glm::quat axisAngleToQuaternion(float rad, const glm::vec3 &axis)
{
    auto c = cos(rad);
    auto s = sin(rad);
    return glm::quat(c, axis.x*s, axis.y*s, axis.z*s);    
}

inline glm::quat xyzEulerToQuaternion(const glm::vec3 &euler)
{
    auto x = axisAngleToQuaternion(euler.x, glm::vec3(1.0, 0.0, 0.0));
    auto y = axisAngleToQuaternion(euler.y, glm::vec3(0.0, 1.0, 0.0));
    auto z = axisAngleToQuaternion(euler.z, glm::vec3(0.0, 0.0, 1.0));
    return x*y*z;
}

} // namespace SVR

#endif //_SVR_TRANSFORMATION_HPP_
