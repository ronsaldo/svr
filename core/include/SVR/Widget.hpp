#ifndef _SVR_WIDGET_HPP_
#define _SVR_WIDGET_HPP_

#include "SVR/Interface.hpp"
#include "SVR/Renderer.hpp"
#include "SVR/Rectangle.hpp"
#include "SVR/Event.hpp"

namespace SVR
{
/**
 * 2D UI Widget
 */
class Widget: public EventHandler
{
public:
    Widget()
    {
        backgroundColor = glm::vec4(0.2, 0.2, 0.2, 1.0);
    }

    ~Widget()
    {
    }

    float getWidth() const
    {
        return size.x;
    }

    float getHeight() const
    {
        return size.y;
    }

    const glm::vec2 &getPosition() const
    {
        return position;
    }

    const glm::vec2 &getSize() const
    {
        return size;
    }

    const glm::vec4 &getBackgroundColor()
    {
        return backgroundColor;
    }

    void setPosition(const glm::vec2 &newPosition)
    {
        position = newPosition;
        onPositionChanged();
    }

    void setSize(const glm::vec2 &newSize)
    {
        size = newSize;
        onSizeChanged();
    }

    void setBackgroundColor(const glm::vec4 &newBackgroundColor)
    {
        backgroundColor = newBackgroundColor;
    }

    Rectangle getRectangle() const
    {
        return Rectangle::positionExtent(position, size);
    }

    virtual void draw(const RendererPtr &renderer)
    {
        drawClipingContent(renderer);
    }

    virtual void processEvent(Event *event)
    {
        event->accept(this);
    }

public:
    virtual void onMouseButtonDown(MouseButtonDownEvent *event)
    {
    }

    virtual void onMouseButtonUp(MouseButtonUpEvent *event)
    {
    }

protected:
    virtual void clearBackground(const RendererPtr &renderer)
    {
        renderer->setColor(getBackgroundColor());
        renderer->drawRectangle(glm::vec2(), getSize());
    }

    virtual void drawClipingContent(const RendererPtr &renderer)
    {
        // TODO: Setup some cliping planes
        renderer->restoreModelViewMatrixAfter([&]{
            renderer->translateModelView(position);
            drawContent(renderer);
        });
    }

    virtual void drawContent(const RendererPtr &renderer)
    {
    }

    virtual void onPositionChanged()
    {
    }

    virtual void onSizeChanged()
    {
    }

private:
    glm::vec2 position;
    glm::vec2 size;

    glm::vec4 backgroundColor;
};

} // namespace SVR

#endif //_SVR_WIDGET_HPP_

