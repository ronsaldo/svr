#ifndef _SVR_DOCKING_LAYOUT_HPP_
#define _SVR_DOCKING_LAYOUT_HPP_

#include <algorithm>
#include "SVR/Layout.hpp"

namespace SVR
{
DECLARE_CLASS(DockedElement);
DECLARE_CLASS(DockingLayout)

/**
 * Dock alignment
 */
enum class DockAlignment
{
    Bottom = 0,
    Top,
    Left,
    Right,
    Center
};

/**
 * Docked element
 */
class DockedElement
{
public:
    DockedElement(const WidgetPtr &widget, DockAlignment alignment, float proportion)
        : widget(widget), alignment(alignment), proportion(proportion)
    {
    }

    ~DockedElement()
    {
    }

    DockAlignment getAlignment() const
    {
        return alignment;
    }

    const WidgetPtr &getWidget() const
    {
        return widget;
    }

    float getProportion() const
    {
        return proportion;
    }

private:
    WidgetPtr widget;
    DockAlignment alignment;
    float proportion;
};

/**
 * Docking layout
 */
class DockingLayout: public Layout
{
public:
    DockedElementPtr addElement(const WidgetPtr &widget, DockAlignment alignment, float proportion)
    {
        auto element = std::make_shared<DockedElement> (widget, alignment, proportion);
        addElement(element);
        return element;
    }

    void addElement(const DockedElementPtr &element)
    {
        elements.push_back(element);
    }

    DockedElementPtr centerElement(const WidgetPtr &widget)
    {
        return addElement(widget, DockAlignment::Center, 1.0);
    }

    DockedElementPtr rightElement(const WidgetPtr &widget, float proportion)
    {
        return addElement(widget, DockAlignment::Right, proportion);
    }

    DockedElementPtr leftElement(const WidgetPtr &widget, float proportion)
    {
        return addElement(widget, DockAlignment::Left, proportion);
    }

    DockedElementPtr topElement(const WidgetPtr &widget, float proportion)
    {
        return addElement(widget, DockAlignment::Top, proportion);
    }

    DockedElementPtr bottomElement(const WidgetPtr &widget, float proportion)
    {
        return addElement(widget, DockAlignment::Bottom, proportion);
    }

    virtual void applyOn(ContainerWidget *container)
    {
        // Sort the elements according to the alignment.
        std::sort(elements.begin(), elements.end(), [](const DockedElementPtr &a, const DockedElementPtr &b) {
            return a->getAlignment() < b->getAlignment();
        });

        // Reset the sizes.
        extent = container->getSize();
        left = 0; right = extent.x;
        bottom = 0; top = extent.y; 

        // Apply to the elements
        for(auto &element: elements)
            applyOnElement(element);
    }

private:
    std::vector<DockedElementPtr> elements;

    void applyOnElement(const DockedElementPtr &element)
    {
        // Compute the element extent.
        glm::vec2 elementExtent;
        switch(element->getAlignment())
        {
        case DockAlignment::Bottom:
        case DockAlignment::Top:
            elementExtent = glm::vec2(extent.x, extent.y*element->getProportion());
            extent.y -= elementExtent.y;
            break;
        case DockAlignment::Left:
        case DockAlignment::Right:
            elementExtent = glm::vec2(extent.x*element->getProportion(), extent.y);
            extent.x -= elementExtent.x;
            break;
        case DockAlignment::Center:
            elementExtent = extent*element->getProportion();
            extent -= elementExtent;
            break;
        }

        // Compute the element position.
        glm::vec2 elementPosition;
        switch(element->getAlignment())
        {
        case DockAlignment::Bottom:
            elementPosition = glm::vec2(left,bottom);
            bottom += elementExtent.y;
            break;
        case DockAlignment::Top:
            top -= elementExtent.y;
            elementPosition = glm::vec2(left,top);
            break;
        case DockAlignment::Left:
            elementPosition = glm::vec2(left,bottom);
            left += elementExtent.x;
            break;
        case DockAlignment::Right:
            right -= elementExtent.x;
            elementPosition = glm::vec2(right,bottom);
            break;
        case DockAlignment::Center:
            elementPosition = glm::vec2(left, bottom);
            left += elementExtent.x;
            bottom += elementExtent.y;
            break;
        }

        // Set the widget position and size
        const auto &widget = element->getWidget();
        widget->setSize(elementExtent);
        widget->setPosition(elementPosition);
    }

    glm::vec2 extent;
    float left, right;
    float top, bottom;
};

} // namespace SVR

#endif //_SVR_DOCKING_LAYOUT_HPP_
