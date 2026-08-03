#include "test/common/dep.hpp"

using namespace by;

TEST(processTest, createAndWait) {
#ifdef BY_BUILD_PLATFORM_IS_WINDOWS
    GTEST_SKIP() << "process test not implemented for Windows";
#else
    process p;
    ASSERT_EQ(p.create("/bin/true"), 0);
    ASSERT_EQ(p.wait(), 0);
#endif
}
