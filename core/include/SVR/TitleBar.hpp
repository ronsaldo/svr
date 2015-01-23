#ifndef _SVR_TITLE_BAR_HPP_
#define _SVR_TITLE_BAR_HPP_

#include "SVR/Widget.hpp"

namespace SVR
{
DECLARE_CLASS(TitleBar);

/**
 * Title bar widget
 */
class TitleBar: public Widget
{
public:

    const std::string &getText() const
    {
        return text;
    }

    void setText(const std::string &newText)
    {
        text = newText;
    }

protected:
    virtual void drawContent(const RendererPtr &renderer)
    {
        // Clear the background
        clearBackground(renderer);

        // Draw the text
        auto pos = glm::vec2(20, getHeight()/3);        
        renderer->drawText(pos, text);
    }

private:
    std::string text;
};
} // namespace SVR

#endif //_SVR_TITLE_BAR_HPP_
