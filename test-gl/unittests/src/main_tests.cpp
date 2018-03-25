#include "gtest/gtest.h"
#include "KeplerEnvironment.hpp"

using kepler::KeplerEnvironment;

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    ::testing::AddGlobalTestEnvironment(new KeplerEnvironment());
    RUN_ALL_TESTS();
    std::getchar(); // keep console window open until Return keystroke
}