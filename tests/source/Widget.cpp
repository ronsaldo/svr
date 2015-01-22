#include <UnitTest++.h>
#include "SVR/Comparison.hpp"
#include "SVR/Widget.hpp"

using namespace SVR;

SUITE(Widget)
{
    TEST(New)
    {
        auto widget = std::make_shared<Widget> ();
        CHECK(closeTo(widget->getPosition(), glm::vec2(0.0, 0.0)));
        CHECK(closeTo(widget->getSize(), glm::vec2(0.0, 0.0)));
        CHECK(closeTo(widget->getBackgroundColor(), glm::vec4(0.2, 0.2, 0.2, 1.0)));
    }

    TEST(ChangePosition)
    {
        auto widget = std::make_shared<Widget> ();
        widget->setPosition(glm::vec2(30, 15));
        widget->setSize(glm::vec2(50, 55));

        CHECK(closeTo(widget->getPosition(), glm::vec2(30.0, 15.0)));
        CHECK(closeTo(widget->getSize(), glm::vec2(50.0, 55.0)));
        CHECK(closeTo(widget->getWidth(), 50));
        CHECK(closeTo(widget->getHeight(), 55));
    }

}

