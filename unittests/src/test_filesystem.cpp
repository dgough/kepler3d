#include "gtest/gtest.h"

#include <FileSystem.hpp>

using namespace kepler;

using std::string;

static constexpr char* TEST_PATH = "test.bin";

TEST(filesystem, get_directory_name) {
    EXPECT_EQ(directoryName(nullptr), "");
    EXPECT_EQ(directoryName(""), "");
    EXPECT_EQ(directoryName("asdf.txt"), "");
    EXPECT_EQ(directoryName("c:\\foo\\bar.txt"), "c:/foo/");
    EXPECT_EQ(directoryName("d:/asdf/image.png"), "d:/asdf/");
    EXPECT_EQ(directoryName("folder/subfolder/data.bin"), "folder/subfolder/");
    //EXPECT_EQ(getDirectoryName("folder/subfolder/"), "folder/"); // TODO
}

TEST(filesystem, joinPath) {
    EXPECT_EQ(joinPath("", ""), "");
    EXPECT_EQ(joinPath("asdf", "image.png"), "asdf/image.png");
    EXPECT_EQ(joinPath("a/", "image.png"), "a/image.png");

    EXPECT_EQ(joinPath(directoryName("res/box.gltf"), "image.png"), "res/image.png");
}

TEST(filesystem, read_write_binary_file) {
    // 0x1A: bad character data. Be sure to include it.
    std::vector<long> out = { 5123L, 0x1A, 458034L, 0x19L, 0x1B, 3584L, -253L, 03L, 5L, -1L, 14586L };
    std::vector<long> in;
    writeBinaryFile(TEST_PATH, out);
    readBinaryFile(TEST_PATH, in);
    EXPECT_EQ(out, in);
}

TEST(filesystem, read_write_binary_file_unsigned_char) {
    std::vector<unsigned char> out;
    out.reserve(0xFF);
    for (unsigned char i = 0; i < 0xFF; ++i) {
        out.push_back(i);
    }
    std::vector<unsigned char> in;
    writeBinaryFile(TEST_PATH, out);
    readBinaryFile(TEST_PATH, in);
    EXPECT_EQ(out, in);
}

TEST(filesystem, read_write_text_file) {
    constexpr char* path = "test.txt";
    string out = "The quick brown fox";
    string in;
    writeTextFile(path, out);
    readTextFile(path, in);
    EXPECT_EQ(out, in);
}
