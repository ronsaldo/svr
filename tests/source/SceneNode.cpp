#include <UnitTest++.h>
#include "SVR/Comparison.hpp"
#include "SVR/SceneNode.hpp"

using namespace SVR;

SUITE(SceneNode)
{
    TEST(New)
    {
        auto node = std::make_shared<SceneNode> ();
        CHECK(closeTo(node->getPosition(), glm::vec3(0.0, 0.0, 0.0)));
        CHECK(closeTo(node->getOrientation(), glm::quat(1.0, 0.0, 0.0, 0.0)));
        CHECK(closeTo(node->getModelMatrix(), glm::mat4(1.0)));
    }

}

