#include <UnitTest++.h>
#include "SVR/Comparison.hpp"
#include "SVR/ContainerWidget.hpp"

using namespace SVR;

SUITE(ContainerWidget)
{
    TEST(New)
    {
        auto container = std::make_shared<ContainerWidget> ();
        CHECK(container->getLayout().get());
        CHECK(!container->getAutoLayout());
    }

    TEST(AddChild)
    {
        auto container = std::make_shared<ContainerWidget> ();
        auto child = std::make_shared<Widget> ();
        container->add(child);
        CHECK(child->getParent() == container);
        CHECK(container->getNumberOfChildren() == 1);
        CHECK(container->getChild(0) == child);
    }

    TEST(AddTwoChild)
    {
        auto container = std::make_shared<ContainerWidget> ();
        auto child = std::make_shared<Widget> ();
        auto child2 = std::make_shared<Widget> ();
        container->add(child);
        container->add(child2);
        CHECK(child->getParent() == container);
        CHECK(child2->getParent() == container);
        CHECK(container->getNumberOfChildren() == 2);
        CHECK(container->getChild(0) == child);
        CHECK(container->getChild(1) == child2);
    }

    TEST(ChangeParent)
    {
        auto container = std::make_shared<ContainerWidget> ();
        auto container2 = std::make_shared<ContainerWidget> ();
        auto child = std::make_shared<Widget> ();
        container->add(child);
        container2->add(child);
        CHECK(child->getParent() == container2);
        CHECK(container->getNumberOfChildren() == 0);
        CHECK(container2->getNumberOfChildren() == 1);
        CHECK(container2->getChild(0) == child);
    }
    
    TEST(RemoveChild)
    {
        auto container = std::make_shared<ContainerWidget> ();
        auto child = std::make_shared<Widget> ();
        container->add(child);
        container->remove(child);

        CHECK(!child->getParent());
        CHECK(container->getNumberOfChildren() == 0);
    }
}

