#ifndef _SVR_CONTAINER_WIDGET_HPP_
#define _SVR_CONTAINER_WIDGET_HPP_

#include <vector>
#include "SVR/Widget.hpp"
#include "SVR/Layout.hpp"

namespace SVR
{
DECLARE_CLASS(ContainerWidget);

/**
 * ContainerWidget
 */
class ContainerWidget: public Widget
{
public:
    ContainerWidget()
    {
        layout = std::make_shared<NullLayout> ();
        autoLayout = false;
    }

    void add(const WidgetPtr &child)
    {
        // Remove the new child from the old parent.
        auto oldParent = child->getParent();
        if(oldParent)
            oldParent->remove(child);

        // Add the child
        children.push_back(child);

        // Set the parent
        child->setParent(shared_from_this());
    }

    void remove(const WidgetPtr &child)
    {
        // Clear the child parent.
        assert(child->getParent() == shared_from_this());
        child->setParent(ContainerWidgetPtr());

        // Find the child.
        auto it = children.begin();
        for(; it != children.end(); ++it)
        {
            if(*it == child)
                break;
        }

        // Erase the child.
        if(it != children.end())
            children.erase(it);

    }

    size_t getNumberOfChildren() const
    {
        return children.size();
    }

    WidgetPtr getChild(size_t index)
    {
        return children[index];
    }

    const LayoutPtr &getLayout() const
    {
        return layout;
    }

    void setLayout(const LayoutPtr &newLayout)
    {
        layout = newLayout;
    }

    void applyLayout()
    {
        getLayout()->applyOn(this);
    }

    bool getAutoLayout() const
    {
        return autoLayout;
    }

    void setAutoLayout(bool newAutoLayout)
    {
        autoLayout = newAutoLayout;
    }

    ContainerWidgetPtr shared_from_this()
    {
        return std::static_pointer_cast<ContainerWidget> (Widget::shared_from_this());
    }

    virtual void onMouseMove(MouseMoveEvent *event)
    {
        // Iterate through the children in reverse order
        auto it = children.rbegin();
        for(; it != children.rend(); ++it)
        {
            auto &child = *it;

            // Send to the child whose rectangle contains the mouse position.
            if(child->getRectangle().containsPoint(event->position))
            {
                auto newEvent = *event;
                newEvent.position -= child->getPosition();
                child->processEvent(&newEvent);

                // Stop propagating if already handled.
                if(newEvent.wasHandled())
                {
                    event->markAsHandled();
                    return;
                }
            }
        }
    }

    virtual void onMouseButtonDown(MouseButtonDownEvent *event)
    {
        // Iterate through the children in reverse order
        auto it = children.rbegin();
        for(; it != children.rend(); ++it)
        {
            auto &child = *it;

            // Send to the child whose rectangle contains the mouse position.
            if(child->getRectangle().containsPoint(event->position))
            {
                auto newEvent = *event;
                newEvent.position -= child->getPosition();
                child->processEvent(&newEvent);

                // Stop propagating if already handled.
                if(newEvent.wasHandled())
                {
                    event->markAsHandled();
                    return;
                }
            }
        }
    }

    virtual void onMouseButtonUp(MouseButtonUpEvent *event)
    {
        // Iterate through the children in reverse order
        auto it = children.rbegin();
        for(; it != children.rend(); ++it)
        {
            auto &child = *it;

            // Send to the child whose rectangle contains the mouse position.
            if(child->getRectangle().containsPoint(event->position))
            {
                auto newEvent = *event;
                newEvent.position -= child->getPosition();
                child->processEvent(&newEvent);

                // Stop propagating if already handled.
                if(newEvent.wasHandled())
                {
                    event->markAsHandled();
                    return;
                }
            }
        }
    }

protected:
    virtual void drawClipingContent(const RendererPtr &renderer) override
    {
        // TODO: Setup some cliping planes
        renderer->restoreModelViewMatrixAfter([&]{
            renderer->translateModelView(getPosition());
            drawContent(renderer);
            drawChildren(renderer);
        });
    }

    virtual void drawChildren(const RendererPtr &renderer)
    {
        for(auto & child: children)
            child->draw(renderer);
    }

    virtual void onSizeChanged()
    {
        if(autoLayout)
            applyLayout();
    }

private:
    std::vector<WidgetPtr> children;
    LayoutPtr layout;
    bool autoLayout;
};

} // namespace SVR

#endif //_SVR_CONTAINER_WIDGET_HPP_
