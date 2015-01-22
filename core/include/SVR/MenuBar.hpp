#ifndef _SVR_MENU_BAR_HPP_
#define _SVR_MENU_BAR_HPP_

#include "SVR/Widget.hpp"

namespace SVR
{
DECLARE_CLASS(MenuBar);

/**
 * Menu Bar
 */
class MenuBar: public Widget
{
public:

protected:
    virtual void drawContent(const RendererPtr &renderer)
    {
        clearBackground(renderer);
    }
};

} // namespace SVR

#endif //_SVR_MENU_BAR_HPP_

