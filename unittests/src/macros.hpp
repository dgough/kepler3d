#pragma once

#include "gtest/gtest.h"

#define EXPECT_VE3_EQ(A, B) \
{ \
    EXPECT_FLOAT_EQ((A).x, (B).x); \
    EXPECT_FLOAT_EQ((A).y, (B).y); \
    EXPECT_FLOAT_EQ((A).z, (B).z); \
}

#define EXPECT_FLOAT_CLOSE(A, B, float_err) \
{ \
    EXPECT_TRUE(glm::abs((B) - (A)) < (float_err)); \
}
