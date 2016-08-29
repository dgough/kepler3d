#include "gtest/gtest.h"
#include <StringUtils.hpp>

using namespace kepler;

using std::string;

TEST(string_utils, startsWith) {
    string str = "The quick brown fox.";
    EXPECT_TRUE(startsWith("", ""));
    EXPECT_TRUE(startsWith(str, ""));
    EXPECT_TRUE(startsWith(str, nullptr));
    EXPECT_TRUE(startsWith(str, 0));
    EXPECT_TRUE(startsWith(str, str.c_str()));

    EXPECT_TRUE(startsWith(str, "The"));
    EXPECT_FALSE(startsWith(str, "the"));

    // prefix longer
    EXPECT_FALSE(startsWith("word", "words"));
    EXPECT_FALSE(startsWith("", "a"));

    // ignore case
    EXPECT_TRUE(startsWith(str, "the", true));

    EXPECT_TRUE(startsWith("data:image/png;base64", "data:image/png"));
}

TEST(string_utils, endswith) {
    string str = "c:\\models\\duck.gltf";
    EXPECT_TRUE(endsWith("", ""));
    EXPECT_TRUE(endsWith(str, ""));
    EXPECT_TRUE(endsWith(str, nullptr));
    EXPECT_TRUE(endsWith(str, 0));
    EXPECT_TRUE(endsWith(str, str.c_str()));

    EXPECT_TRUE(endsWith(str, ".gltf"));
    EXPECT_TRUE(endsWith(str, ".gltf", true));
    EXPECT_TRUE(endsWith(str, ".GLTF", true));
    EXPECT_TRUE(endsWith(str, "f"));
    EXPECT_TRUE(endsWith("data:image/png;base64", "base64"));

    // suffix longer
    EXPECT_FALSE(endsWith("word", "words"));

    EXPECT_FALSE(endsWith("", "a"));
    EXPECT_FALSE(endsWith(str, ".GLTF", false));
}
