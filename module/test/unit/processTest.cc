#include "test/common/dep.hpp"

using namespace by;

TEST(processTest, createAndWait) {
    process p;
    ASSERT_TRUE(p.create("/bin/true"));
    ASSERT_EQ(p.wait(), 0);
}
