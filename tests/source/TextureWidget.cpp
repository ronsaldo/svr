#include <UnitTest++.h>
#include "SVR/Comparison.hpp"
#include "SVR/TextureWidget.hpp"

using namespace SVR;

SUITE(TextureWidget)
{
    TEST(New)
    {
        auto widget = std::make_shared<TextureWidget> ();
        CHECK(!widget->getTexture());
    }
}

