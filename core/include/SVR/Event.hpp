#ifndef _SVR_EVENT_HPP_
#define _SVR_EVENT_HPP_

#include "SVR/Interface.hpp"

namespace SVR
{
DECLARE_INTERFACE(EventHandler);
DECLARE_CLASS(Event);
DECLARE_CLASS(MouseMoveEvent);
DECLARE_CLASS(MouseButtonDownEvent);
DECLARE_CLASS(MouseButtonUpEvent);

/**
 * Mouse button
 */
enum class MouseButton
{
    Left = 1,
    Middle,
    Right,
    Aux0,
    Aux1
};

/**
 * Event handler
 */
struct EventHandler: Interface
{
    virtual void onMouseMove(MouseMoveEvent *event) = 0;
    virtual void onMouseButtonDown(MouseButtonDownEvent *event) = 0;
    virtual void onMouseButtonUp(MouseButtonUpEvent *event) = 0;
};

/**
 * Event
 */
class Event: public Interface
{
public:
    virtual void accept(EventHandler *handler) = 0;

    bool wasHandled() const
    {
        return handled;
    }

    void markAsHandled()
    {
        handled = true;
    }

private:
    bool handled;
};

/**
 * Mouse move event
 */
class MouseMoveEvent: public Event
{
public:
    virtual void accept(EventHandler *handler)
    {
        handler->onMouseMove(this);
    }

    glm::vec2 position;
    glm::vec2 delta;
};

/**
 * Mouse button event
 */
class MouseButtonEvent: public Event
{
public:

    glm::vec2 position;
    MouseButton button;
};

/**
 * Mouse button event
 */
class MouseButtonDownEvent: public MouseButtonEvent
{
public:
    virtual void accept(EventHandler *handler)
    {
        handler->onMouseButtonDown(this);
    }
};

/**
 * Mouse button event
 */
class MouseButtonUpEvent: public MouseButtonEvent
{
public:
    virtual void accept(EventHandler *handler)
    {
        handler->onMouseButtonUp(this);
    }
};

} // namespace SVR

#endif //_SVR_EVENT_HPP_
