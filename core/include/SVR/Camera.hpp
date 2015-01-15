#ifndef _SVR_CAMERA_HPP_
#define _SVR_CAMERA_HPP_

#include <math.h>
#include "SVR/SceneNode.hpp"

namespace SVR
{
DECLARE_CLASS(Camera);

/**
 * Frustum corners
 */
enum class FrustumCorner
{
    LeftTopNear = 0,
    RightTopNear,
    LeftBottomNear,
    RightBottomNear,
    LeftTopFar,
    RightTopFar,
    LeftBottomFar,
    RightBottomFar,
};

typedef glm::vec4 FrustumCorners[8];

/**
 * Camera
 */
class Camera: public SceneNode
{
public:
    Camera()
    {
    }

    ~Camera()
    {
    }

    void orthographic(float left, float right, float bottom, float top, float near, float far)
    {
        projectionMatrix = glm::ortho(left, right, bottom, top, near, far);

        // Ner plane
        frustumCorners[(int)FrustumCorner::LeftTopNear] = glm::vec4(left, top, near, 1.0);
        frustumCorners[(int)FrustumCorner::RightTopNear] = glm::vec4(right, top, near, 1.0);
        frustumCorners[(int)FrustumCorner::LeftBottomNear] = glm::vec4(left, bottom, near, 1.0);
        frustumCorners[(int)FrustumCorner::RightBottomNear] = glm::vec4(right, bottom, near, 1.0);

        // Far plane
        frustumCorners[(int)FrustumCorner::LeftTopFar] = glm::vec4(left, top, far, 1.0);
        frustumCorners[(int)FrustumCorner::RightTopFar] = glm::vec4(right, top, far, 1.0);
        frustumCorners[(int)FrustumCorner::LeftBottomFar] = glm::vec4(left, bottom, far, 1.0);
        frustumCorners[(int)FrustumCorner::RightBottomFar] = glm::vec4(right, bottom, far, 1.0);
    }

    void perspective(float fovy, float aspect, float near, float far)
    {
        projectionMatrix = glm::perspective(fovy, aspect, near, far);
        auto angle = fovy*M_PI*0.5/180.0;
        auto tg = tan(angle);

        // Near plane
        auto nearTop = tg*near;
        auto nearRight = nearTop*aspect;
        auto nearBottom = -nearTop;
        auto nearLeft = -nearRight;

        frustumCorners[(int)FrustumCorner::LeftTopNear] = glm::vec4(nearLeft, nearTop, -near, 1.0);
        frustumCorners[(int)FrustumCorner::RightTopNear] = glm::vec4(nearRight, nearTop, -near, 1.0);
        frustumCorners[(int)FrustumCorner::LeftBottomNear] = glm::vec4(nearLeft, nearBottom, -near, 1.0);
        frustumCorners[(int)FrustumCorner::RightBottomNear] = glm::vec4(nearRight, nearBottom, -near, 1.0);

        // Far plane
        auto farTop = tg*far;
        auto farRight = nearTop*aspect;
        auto farBottom = -nearTop;
        auto farLeft = -nearRight;

        frustumCorners[(int)FrustumCorner::LeftTopFar] = glm::vec4(farLeft, farTop, -far, 1.0);
        frustumCorners[(int)FrustumCorner::RightTopFar] = glm::vec4(farRight, farTop, -far, 1.0);
        frustumCorners[(int)FrustumCorner::LeftBottomFar] = glm::vec4(farLeft, farBottom, -far, 1.0);
        frustumCorners[(int)FrustumCorner::RightBottomFar] = glm::vec4(farRight, farBottom, -far, 1.0);
    }

    const glm::mat4 &getProjectionMatrix() const
    {
        return projectionMatrix;
    }

    void getFrustumCorners(FrustumCorners dest) const
    {
        for(size_t i = 0; i < 8; ++i)
            dest[i] = frustumCorners[i];
    }

    void getWorldFrustumCorners(FrustumCorners dest) const
    {
        auto modelMatrix = getModelMatrix();

        for(size_t i = 0; i < 8; ++i)
            dest[i] = modelMatrix*frustumCorners[i];
    }

private:
    glm::mat4 projectionMatrix;
    FrustumCorners frustumCorners;
    
};

} // namespace SVR

#endif //_SVR_CAMERA_HPP_

