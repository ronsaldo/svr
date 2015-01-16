#ifndef _SVR_COLOR_BAR_WIDGET_HPP_
#define _SVR_COLOR_BAR_WIDGET_HPP_

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
    ColorBarWidget()
    {
        minValue = 0.0;
        maxValue = 1.0;
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

            // Draw the max triangle
            renderer->setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
            drawTriangle(renderer, maxValue);
        });
    }

private:
    void drawTriangle(const RendererPtr &renderer, float value)
    {
        const float TriangleWidth = 20.0f;
        const float TriangleHeight = 20.0f;
        const float TriangleHalfHeight = TriangleHeight*0.5f;

        float y = getHeight() * value;
        renderer->drawTriangle(glm::vec2(0.0, y), glm::vec2(TriangleWidth, y + TriangleHalfHeight), glm::vec2(TriangleWidth, y - TriangleHalfHeight));
    }

    Texture1DPtr gradient;
    float minValue, maxValue;
};

} // namespace SVR

#endif //_SVR_COLOR_BAR_WIDGET_HPP_
