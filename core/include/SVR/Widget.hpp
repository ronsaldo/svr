#ifndef _SVR_WIDGET_HPP_
#define _SVR_WIDGET_HPP_

#include "SVR/Interface.hpp"
#include "SVR/Renderer.hpp"
#include "SVR/Rectangle.hpp"
#include "SVR/Event.hpp"

namespace SVR
{
DECLARE_CLASS(Widget);
DECLARE_CLASS(ContainerWidget);

/**
 * 2D UI Widget
 */
class Widget: public EventHandler,
    public std::enable_shared_from_this<Widget>
{
public:
    Widget()
    {
        backgroundColor = glm::vec4(0.2, 0.2, 0.2, 1.0);
    }

    ~Widget()
    {
    }

    ContainerWidgetPtr getParent() const
    {
        return parent.lock();
    }

    void setParent(const ContainerWidgetPtr &newParent)
    {
        parent = newParent;
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
    virtual void onMouseMove(MouseMoveEvent *event)
    {
    }

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

    ContainerWidgetWeakPtr parent;
};

} // namespace SVR

#endif //_SVR_WIDGET_HPP_

