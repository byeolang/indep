#include "test/common/dep.hpp"

using namespace by;
using namespace std;

TEST(fsystemTest, recursiveDirSearchAtBin) {
    // expect that workdirectory of this executable is at 'bin'.
    auto root = fsystem::find("..");
    nbool buildFound = false;

    while(root.next()) {
        if(*root == "../build/CMakeLists.txt") {
            buildFound = true;
            ASSERT_EQ(root.getName(), "CMakeLists.txt");
            ASSERT_EQ(root.getDir(), "../build");
            break;
        }
    }
    root.rel();
    ASSERT_TRUE(buildFound);
}

TEST(fsystemTest, dirSearchWithPathEndsWithSlash) {
    auto root = fsystem::find("../");
    nbool buildFound = false;

    while(root.next()) {
        if(*root == "../build/CMakeLists.txt") {
            buildFound = true;
            ASSERT_EQ(root.getName(), "CMakeLists.txt");
            ASSERT_EQ(root.getDir(), "../build");
            break;
        }
    }
    root.rel();
    ASSERT_TRUE(buildFound);
}

TEST(fsystemTest, supportGlobPatternAtTestdata) {
    auto root = fsystem::find("*meta*");
    ASSERT_TRUE(root.next());
    root.rel();
}
