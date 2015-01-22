#ifndef _SVR_COLOR_BAR_WIDGET_HPP_
#define _SVR_COLOR_BAR_WIDGET_HPP_

#include <algorithm>
#include <functional>
#include "SVR/Widget.hpp"

namespace SVR
{
DECLARE_CLASS(ColorBarWidget)

/**
 * Color bar widget
 */
class ColorBarWidget: public Widget
{
public:
    typedef std::function<double (double)> ValueMap;

    ColorBarWidget()
    {
        minValue = 0.0;
        maxValue = 1.0;
        valueMap = identityValueMap();
    }

    ~ColorBarWidget()
    {
    }

    const Texture1DPtr &getGradient() const
    {
        return gradient;
    }

    void setGradient(const Texture1DPtr &newGradient)
    {
        gradient = newGradient;
    }

    float getMinValue() const
    {
        return minValue;
    }

    float getMaxValue() const
    {
        return maxValue;
    }

    void setMinValue(float newMinValue)
    {
        minValue = newMinValue;
    }

    void setMaxValue(float newMaxValue)
    {
        maxValue = newMaxValue;
    }

    const ValueMap &getValueMap() const
    {
        return valueMap;
    }

    void setValueMap(const ValueMap &newValueMap)
    {
        valueMap = newValueMap;
    }

    static ValueMap identityValueMap()
    {
        return [](double x) {return x; };
    }

public:
    virtual void onMouseButtonDown(MouseButtonDownEvent *event)
    {
        auto value = event->position.y/getHeight();

        switch(event->button)
        {
        case MouseButton::Left:
            minValue = std::min(value, maxValue);
            break;
        case MouseButton::Right:
            maxValue = std::max(value, minValue);
            break;
        default:
            // Do nothing.
            break;
        }
    }

    virtual void onMouseButtonUp(MouseButtonUpEvent *event)
    {
    }

protected:
    virtual void drawContent(const RendererPtr &renderer)
    {
        const float BarWidth = 20.0f;

        // Clear the background.
        clearBackground(renderer);

        // Draw the bar gradient
        if(gradient)
            renderer->setLinearGradient(gradient, glm::vec2(0,0), glm::vec2(0, getHeight()));
        else
            renderer->setColor(glm::vec4(1.0, 0.0, 1.0, 1.0));
        renderer->drawRectangle(glm::vec2(), glm::vec2(BarWidth, getHeight()));

        renderer->restoreModelViewMatrixAfter([&]{
            renderer->translateModelView(glm::vec2(BarWidth, 0.0));

            // Draw the min triangle
            renderer->setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
            drawTriangle(renderer, minValue);
            drawValue(renderer, minValue);

            // Draw the max triangle
            renderer->setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
            drawTriangle(renderer, maxValue);
            drawValue(renderer, maxValue);
        });
    }

private:
    static const int TriangleWidth = 20;
    static const int TriangleHeight = 20;
    static const int TriangleHalfHeight = TriangleHeight/2;

    void drawTriangle(const RendererPtr &renderer, float value)
    {

        float y = getHeight() * value;
        renderer->drawTriangle(glm::vec2(0.0, y), glm::vec2(TriangleWidth, y + TriangleHalfHeight), glm::vec2(TriangleWidth, y - TriangleHalfHeight));
    }

    void drawValue(const RendererPtr &renderer, float value)
    {
        char buffer[256];
        sprintf(buffer, "%f", valueMap(value));

        auto position = glm::vec2(TriangleWidth, getHeight() * value - TriangleHalfHeight + TriangleHalfHeight/2);
        renderer->drawText(position, buffer);
    }

    Texture1DPtr gradient;
    float minValue, maxValue;
    ValueMap valueMap;
};

} // namespace SVR

#endif //_SVR_COLOR_BAR_WIDGET_HPP_
