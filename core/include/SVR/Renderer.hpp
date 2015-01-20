#ifndef _SVR_RENDERER_HPP_
#define _SVR_RENDERER_HPP_

#include <glm/glm.hpp>
#include "SVR/Interface.hpp"
#include "SVR/Texture.hpp"
#include "SVR/Framebuffer.hpp"

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
    virtual void setTexture(const Texture2DPtr &texture) = 0;
    virtual void setLinearGradient(const Texture1DPtr &texture, const glm::vec2 &start, const glm::vec2 &end) = 0;

    virtual void drawLine(const glm::vec2 &start, const glm::vec2 &end) = 0;
    virtual void drawTriangle(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c) = 0;
    virtual void drawRectangle(const glm::vec2 &min, const glm::vec2 &max) = 0;
    virtual void drawText(const glm::vec2 &position, const std::string &text) = 0;
    virtual void drawGlyph(const glm::vec2 &sheetPosition, const glm::vec2 &sheetExtent, const glm::vec2 &position, const glm::vec2 &extent) = 0;

    virtual void setGammaCorrection(float gammaCorrection) = 0;

    virtual void flushCommands() = 0;

    virtual Texture1DPtr createTexture1D(int width, PixelFormat pixelFormat) = 0;
    virtual Texture2DPtr createTexture2D(int width, int height, PixelFormat pixelFormat) = 0;

    virtual void beginCompute() = 0;
    virtual void endCompute() = 0;

    virtual const glm::mat3 &getModelViewMatrix() const = 0;
    virtual void setModelViewMatrix(const glm::mat3 &matrix) = 0;

    virtual void useMainFramebuffer() = 0;
    virtual FramebufferPtr createFramebuffer(int width, int height) = 0;

    template<typename F>
    void restoreModelViewMatrixAfter(F f)
    {
        auto oldMatrix = getModelViewMatrix();
        f();
        setModelViewMatrix(oldMatrix);
    }

    inline void translateModelView(const glm::vec2 &translation)
    {
        glm::mat3 mat;
        mat[2][0] = translation.x;
        mat[2][1] = translation.y;
        multModelView(mat);
    }

    inline void multModelView(const glm::mat3 &matrix)
    {
        setModelViewMatrix(getModelViewMatrix()*matrix);
    }
};

RendererPtr createRenderer();

} // namespace SVR

#endif //SVR_RENDERER_HPP_
