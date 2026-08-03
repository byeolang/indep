#include "test/common/dep.hpp"

using namespace by;

TEST(processTest, createAndWait) {
    process p;
    ASSERT_TRUE(p.create("/usr/bin/true"));
    ASSERT_EQ(p.wait(), 0);
}

TEST(processTest, nonZeroExitCode) {
    process p;
    ASSERT_TRUE(p.create("/usr/bin/false"));
    ASSERT_EQ(p.wait(), 1);
}

TEST(processTest, passesArguments) {
    process p;
    ASSERT_TRUE(p.create("/bin/sh", {"-c", "exit 42"}));
    ASSERT_EQ(p.wait(), 42);
}

TEST(processTest, execFailureReports127) {
    process p;
    ASSERT_TRUE(p.create("/nonexistent/path/to/binary"));
    ASSERT_EQ(p.wait(), 127);
}
