#include "gtest/gtest.h"
#include "macros.hpp"

#include <RenderState.hpp>

using namespace kepler;
using namespace kepler::gl;

TEST(renderState, set_bits) {
    RenderState r;
    r.setCulling(true);
    EXPECT_TRUE(r.isCullingEnabled());
    r.setDepthTest(true);
    EXPECT_TRUE(r.isDepthTestEnabled());
    r.setCulling(false);
    EXPECT_TRUE(r.isDepthTestEnabled());
    EXPECT_FALSE(r.isCullingEnabled());
}
