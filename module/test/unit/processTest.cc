#include "test/common/dep.hpp"

using namespace by;

TEST(processTest, createAndWait) {
    process p;
    assertEquals(p.create(), 0);
    assertEquals(p.wait(), 0);
}
