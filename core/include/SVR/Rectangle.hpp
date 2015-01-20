#ifndef _SVR_RECTANGLE_HPP_
#define _SVR_RECTANGLE_HPP_

#include "SVR/Comparison.hpp"

namespace SVR
{

/**
 * 2D rectangle
 */
class Rectangle
{
public:
    Rectangle() {}
    Rectangle(const glm::vec2 &min, const glm::vec2 &max)
        : min(min), max(max) {}

    bool containsPoint(const glm::vec2 &point) const
    {
        return min.x <= point.x && point.x <= max.x &&
               min.y <= point.y && point.y <= max.y;
    }

    static Rectangle positionExtent(const glm::vec2 &position, const glm::vec2 &extent)
    {
        return Rectangle(position, position + extent);
    }

    glm::vec2 min;
    glm::vec2 max;
};

} // namespace SVR

#endif //_SVR_RECTANGLE_HPP_

