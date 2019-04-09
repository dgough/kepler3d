#include "gtest/gtest.h"
#include "KeplerEnvironment.hpp"

using kepler::KeplerEnvironment;

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    ::testing::AddGlobalTestEnvironment(new KeplerEnvironment());

    //::testing::GTEST_FLAG(filter) = "shader*";

    RUN_ALL_TESTS();
}
