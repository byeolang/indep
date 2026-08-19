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
            ASSERT_EQ(root->filename(), "CMakeLists.txt");
            ASSERT_EQ(root.getDir(), "../build");
            ASSERT_EQ(root.getDir().filename(), "build");
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
            ASSERT_EQ(root->filename(), "CMakeLists.txt");
            ASSERT_EQ(root.getDir(), "../build");
            break;
        }
    }
    root.rel();
    ASSERT_TRUE(buildFound);
}

TEST(fsystemTest, findAcceptsFilePathWithoutWildcard) {
    // a bare file path used to open as a directory, which always failed and
    // left the iterator empty. it now names that one file.
    // find() still recurses, so subdirectories holding the same name keep
    // showing up. the point here is that the named file is the first hit.
    auto found = fsystem::find("../build/CMakeLists.txt");
    ASSERT_TRUE(found.next());
    ASSERT_EQ(found->filename(), "CMakeLists.txt");
    ASSERT_EQ(*found, "../build/CMakeLists.txt");
    found.rel();
}

TEST(fsystemTest, findOnMissingFileStaysEmpty) {
    auto found = fsystem::find("../build/thereIsNoSuchFile.txt");
    ASSERT_FALSE(found.next());
    found.rel();
}

TEST(fsystemTest, findTreatsFileNameAsLiteralNotRegex) {
    // '.' must not match any character, or a missing file would be reported
    // as found whenever a similarly named one sits next to it.
    auto found = fsystem::find("../build/CMakeListsGtxt");
    ASSERT_FALSE(found.next());
    found.rel();
}

TEST(fsystemTest, findOnMissingWildcardStaysEmpty) {
    // a pattern with no matching file name should not accidentally hit
    // anything by recursion.
    auto found = fsystem::find("../thereIsNoSuchFi?e");
    ASSERT_FALSE(found.next());
    found.rel();
}
