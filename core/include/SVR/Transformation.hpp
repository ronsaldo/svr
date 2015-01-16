#ifndef _SVR_TRANSFORMATION_HPP_
#define _SVR_TRANSFORMATION_HPP_

#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SVR
{

template<typename R>
inline R degreesToRadians(R deg)
{
    return deg*M_PI/180.0;
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

} // namespace SVR

#endif //_SVR_TRANSFORMATION_HPP_
