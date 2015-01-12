#ifndef _SVR_RENDERER_HPP_
#define _SVR_RENDERER_HPP_

#include <glm/glm.hpp>
#include "SVR/Interface.hpp"

namespace SVR
{
DECLARE_INTERFACE(Renderer);

/**
 * The main window renderer interface.
 */
class Renderer: public Interface
{
public:
    virtual bool initialize(int argc, const char **argv) = 0;
    virtual void shutdown() = 0;

    virtual void setScreenSize(const glm::vec2 &newScreenSize) = 0;

    virtual void clearColor(const glm::vec4 &color) = 0;
    virtual void clear() = 0;

    virtual void setColor(const glm::vec4 &color) = 0;
    virtual void drawLine(const glm::vec2 &start, const glm::vec2 &end) = 0;
    virtual void drawTriangle(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c) = 0;

    virtual void flushCommands() = 0;
};

RendererPtr createRenderer();

} // namespace SVR

#endif //SVR_RENDERER_HPP_
