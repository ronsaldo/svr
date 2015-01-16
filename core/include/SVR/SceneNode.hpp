#ifndef _SVR_SCENE_NODE_HPP_
#define _SVR_SCENE_NODE_HPP_

#include "SVR/Transformation.hpp"
#include "SVR/Interface.hpp"

namespace SVR
{
DECLARE_CLASS(SceneNode);

/**
 * SceneNode
 */
class SceneNode: public Interface
{
public:
    SceneNode()
    {
    }
    ~SceneNode()
    {
    }

    const glm::vec3 &getPosition() const
    {
        return position;
    }

    void setPosition(const glm::vec3 &newPosition)
    {
        position = newPosition;
    }

    const glm::quat &getOrientation() const
    {
        return orientation;
    }

    void setOrientation(const glm::quat &newOrientation)
    {
        orientation = newOrientation;
    }

    glm::mat4 getModelMatrix() const
    {
        return glm::translate(glm::mat4_cast(orientation), position);
    }

private:
    glm::vec3 position;
    glm::quat orientation;
};

} // namespace SVR

#endif //_SVR_SCENE_NODE_HPP_

