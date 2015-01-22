#ifndef _SVR_LAYOUT_HPP_
#define _SVR_LAYOUT_HPP_

#include "SVR/Interface.hpp"

namespace SVR
{
DECLARE_CLASS(Widget)
DECLARE_CLASS(ContainerWidget)
DECLARE_INTERFACE(Layout)

/**
 * Layout
 */
struct Layout: Interface
{
    virtual void applyOn(ContainerWidget *container) = 0;
};

/**
 *  Null layout
 */
class NullLayout: public Layout
{
public:
    virtual void applyOn(ContainerWidget *container)
    {
        // I am the null layout, so I do nothing here.
    }
};

} // namespace SVR

#endif //_SVR_LAYOUT_HPP_

