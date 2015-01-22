#include <UnitTest++.h>
#include "SVR/Comparison.hpp"
#include "SVR/ContainerWidget.hpp"
#include "SVR/DockingLayout.hpp"

using namespace SVR;

SUITE(DockingLayout)
{
    TEST(New)
    {
        auto widget = std::make_shared<Widget> ();
        auto layout = std::make_shared<DockingLayout> ();
    }

    TEST(CenterElement)
    {
        auto widget = std::make_shared<Widget> ();
        auto layout = std::make_shared<DockingLayout> ();
        auto element = layout->centerElement(widget);
        CHECK(closeTo(element->getProportion(), 1.0f));
        CHECK(element->getAlignment() == DockAlignment::Center);
        CHECK(element->getWidget() == widget);
    }

    TEST(RightElement)
    {
        auto widget = std::make_shared<Widget> ();
        auto layout = std::make_shared<DockingLayout> ();
        auto element = layout->rightElement(widget, 0.1f);
        CHECK(closeTo(element->getProportion(), 0.1f));
        CHECK(element->getAlignment() == DockAlignment::Right);
        CHECK(element->getWidget() == widget);
    }

    TEST(LeftElement)
    {
        auto widget = std::make_shared<Widget> ();
        auto layout = std::make_shared<DockingLayout> ();
        auto element = layout->leftElement(widget, 0.1f);
        CHECK(closeTo(element->getProportion(), 0.1f));
        CHECK(element->getAlignment() == DockAlignment::Left);
        CHECK(element->getWidget() == widget);
    }

    TEST(TopElement)
    {
        auto widget = std::make_shared<Widget> ();
        auto layout = std::make_shared<DockingLayout> ();
        auto element = layout->topElement(widget, 0.1f);
        CHECK(closeTo(element->getProportion(), 0.1f));
        CHECK(element->getAlignment() == DockAlignment::Top);
        CHECK(element->getWidget() == widget);
    }

    TEST(BottomElement)
    {
        auto widget = std::make_shared<Widget> ();
        auto layout = std::make_shared<DockingLayout> ();
        auto element = layout->bottomElement(widget, 0.1f);
        CHECK(closeTo(element->getProportion(), 0.1f));
        CHECK(element->getAlignment() == DockAlignment::Bottom);
        CHECK(element->getWidget() == widget);
    }



}
