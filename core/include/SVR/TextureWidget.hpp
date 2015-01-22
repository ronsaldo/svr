#ifndef _SVR_TEXTURE_WIDGET_HPP_
#define _SVR_TEXTURE_WIDGET_HPP_

#include "SVR/Widget.hpp"

namespace SVR
{
DECLARE_CLASS(TextureWidget);

/**
 * Texture widget
 */
class TextureWidget: public Widget
{
public:
    const Texture2DPtr &getTexture() const
    {
        return texture;
    }

    void setTexture(const Texture2DPtr &newTexture)
    {
        texture = newTexture;
    }

protected:
    virtual void drawContent(const RendererPtr &renderer)
    {
        renderer->setTexture(texture);
        renderer->drawRectangle(glm::vec2(), getSize());
    }

private:
    Texture2DPtr texture;
};

} // namespace SVR

#endif //_SVR_TEXTURE_WIDGET_HPP_
