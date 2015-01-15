#ifndef _SVR_AABOX_HPP_
#define _SVR_AABOX_HPP_

#include <glm/glm.hpp>

namespace SVR
{

/**
 * Axis aligned bounding box
 */
class AABox
{
public:
    AABox() {}
    AABox(const glm::vec3 &min, const glm::vec3 &max)
        : min(min), max(max) {}
    ~AABox() {}

    glm::vec3 min;
    glm::vec3 max;
};

} // namespace SVR

#endif // _SVR_AABOX_HPP_
