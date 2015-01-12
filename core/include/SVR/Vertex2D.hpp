#ifndef _SVR_VERTEX2D_HPP_
#define _SVR_VERTEX2D_HPP_

#include <glm/glm.hpp>

namespace SVR
{
/**
 * 2D vertex definition.
 */
struct Vertex2D
{
    glm::vec2 position;
    glm::vec2 texcoord;
    glm::vec4 color;
};
} // namespace SVR

#endif //_SVR_VERTEX2D_HPP_

