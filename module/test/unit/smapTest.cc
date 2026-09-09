#include "test/common/dep.hpp"

#include <string>
#include <vector>

using namespace by;

namespace {
    typedef smap<std::string, int, std::map> uniqMap;
    typedef smap<std::string, int, std::unordered_multimap> multiMap;

    template <typename MAP> std::vector<std::string> keysOf(const MAP& it) {
        std::vector<std::string> ret;
        for(auto e = it.begin(); e != it.end(); ++e)
            ret.push_back(*e.getKey());
        return ret;
    }
}

TEST(smapTest, iteratesInInsertionOrderNotAlphabetical) {
    // the reason this class exists: std::map would hand these back apple, mango, zebra.
    uniqMap it;
    it.insert("zebra", 1);
    it.insert("apple", 2);
    it.insert("mango", 3);

    ASSERT_EQ(it.size(), 3);
    ASSERT_EQ(keysOf(it), (std::vector<std::string>{"zebra", "apple", "mango"}));
}

TEST(smapTest, insertionOrderSurvivesNumericLookingKeys) {
    // stela names bundles v0_1_10_0 and v0_1_4_1. sorted by string, the 10 comes first
    // and "the last one is the newest" stops being true.
    uniqMap it;
    it.insert("v0_1_4_0", 1);
    it.insert("v0_1_4_1", 2);
    it.insert("v0_1_10_0", 3);

    ASSERT_EQ(keysOf(it), (std::vector<std::string>{"v0_1_4_0", "v0_1_4_1", "v0_1_10_0"}));
}

TEST(smapTest, uniqueKeyContainerReplacesValueAndKeepsPosition) {
    uniqMap it;
    it.insert("a", 1);
    it.insert("b", 2);
    it.insert("a", 99);

    ASSERT_EQ(it.size(), 2);
    ASSERT_EQ(keysOf(it), (std::vector<std::string>{"a", "b"}));
    ASSERT_EQ(*it.find("a"), 99);
}

TEST(smapTest, duplicateReplacesTheStoredValue) {
    // the refused insertion has to be noticed: without it the new value is dropped and
    // the element gets linked a second time, which splices it after itself.
    smap<std::string, std::string, std::map> it;
    it.insert("k", std::string("first"));
    it.insert("k", std::string("second"));

    ASSERT_EQ(it.size(), 1);
    ASSERT_STREQ(it.find("k")->c_str(), "second");
}

TEST(smapTest, multiKeyContainerKeepsEveryInsertion) {
    multiMap it;
    it.insert("dup", 1);
    it.insert("dup", 2);
    it.insert("dup", 3);

    ASSERT_EQ(it.size(), 3);
    ASSERT_EQ(keysOf(it), (std::vector<std::string>{"dup", "dup", "dup"}));
}

TEST(smapTest, iteratesEveryValueOfOneKeyInOrder) {
    multiMap it;
    it.insert("a", 1);
    it.insert("b", 2);
    it.insert("a", 3);

    std::vector<int> got;
    for(auto e = it.begin("a"); !e.isEnd(); ++e)
        got.push_back(*e);
    ASSERT_EQ(got, (std::vector<int>{1, 3}));
}

TEST(smapTest, reverseWalksBackwards) {
    uniqMap it;
    it.insert("zebra", 1);
    it.insert("apple", 2);
    it.insert("mango", 3);

    std::vector<std::string> got;
    for(auto e = it.rbegin(); e != it.rend(); ++e)
        got.push_back(*e.getKey());
    ASSERT_EQ(got, (std::vector<std::string>{"mango", "apple", "zebra"}));
}

TEST(smapTest, eraseByKeyClosesTheGap) {
    uniqMap it;
    it.insert("a", 1);
    it.insert("b", 2);
    it.insert("c", 3);
    it.erase(std::string("b"));

    ASSERT_EQ(it.size(), 2);
    ASSERT_EQ(keysOf(it), (std::vector<std::string>{"a", "c"}));
}

TEST(smapTest, eraseByKeyRemovesEveryDuplicate) {
    multiMap it;
    it.insert("a", 1);
    it.insert("dup", 2);
    it.insert("dup", 3);
    it.insert("b", 4);
    it.erase(std::string("dup"));

    ASSERT_EQ(it.size(), 2);
    ASSERT_EQ(keysOf(it), (std::vector<std::string>{"a", "b"}));
}

TEST(smapTest, eraseByIteratorRemovesOnlyThatElement) {
    multiMap it;
    it.insert("dup", 1);
    it.insert("dup", 2);
    it.erase(it.begin());

    ASSERT_EQ(it.size(), 1);
    ASSERT_EQ(*it.begin(), 2);
}

TEST(smapTest, clearEmptiesBothTheMapAndTheChain) {
    uniqMap it;
    it.insert("a", 1);
    it.insert("b", 2);
    it.clear();

    ASSERT_EQ(it.size(), 0);
    ASSERT_TRUE(it.begin() == it.end());

    // the linked list has to be usable again, not left pointing at freed wraps.
    it.insert("c", 3);
    ASSERT_EQ(keysOf(it), (std::vector<std::string>{"c"}));
}

TEST(smapTest, emptyMapBeginIsEnd) {
    uniqMap it;
    ASSERT_EQ(it.size(), 0);
    ASSERT_TRUE(it.begin() == it.end());
    ASSERT_TRUE(it.rbegin() == it.rend());
}

TEST(smapTest, findMissingKeyLandsOnEnd) {
    uniqMap it;
    it.insert("a", 1);
    ASSERT_TRUE(it.find("nope").isEnd());
}

TEST(smapTest, smultimapAliasIsTheUnorderedMultimapShape) {
    smultimap<std::string, int> it;
    it.insert("dup", 1);
    it.insert("dup", 2);
    ASSERT_EQ(it.size(), 2);
}

TEST(smapTest, copyRebuildsItsOwnChain) {
    // wrap's copy constructor drops prev/next, so copying the container alone leaves
    // size() reporting 2 over a chain that walks nothing.
    uniqMap origin;
    origin.insert("zebra", 1);
    origin.insert("apple", 2);

    uniqMap copied(origin);
    ASSERT_EQ(copied.size(), 2);
    ASSERT_EQ(keysOf(copied), (std::vector<std::string>{"zebra", "apple"}));

    // the two are independent afterwards.
    copied.insert("mango", 3);
    ASSERT_EQ(origin.size(), 2);
    ASSERT_EQ(keysOf(origin), (std::vector<std::string>{"zebra", "apple"}));
}

TEST(smapTest, copyOutlivesItsSource) {
    // wrap::key points at the source map's key. the rebuilt copy must own its own.
    uniqMap copied;
    {
        uniqMap origin;
        origin.insert("a", 1);
        origin.insert("b", 2);
        copied = origin;
    }
    ASSERT_EQ(keysOf(copied), (std::vector<std::string>{"a", "b"}));
    ASSERT_EQ(*copied.find("b"), 2);
}

TEST(smapTest, assignmentReplacesWhatWasThere) {
    uniqMap lhs;
    lhs.insert("old", 1);

    uniqMap rhs;
    rhs.insert("new", 2);

    lhs = rhs;
    ASSERT_EQ(lhs.size(), 1);
    ASSERT_EQ(keysOf(lhs), (std::vector<std::string>{"new"}));
}

TEST(smapTest, selfAssignmentKeepsContents) {
    uniqMap it;
    it.insert("a", 1);
    it.insert("b", 2);

    uniqMap& alias = it;
    it = alias;
    ASSERT_EQ(keysOf(it), (std::vector<std::string>{"a", "b"}));
}

TEST(smapTest, copyKeepsDuplicates) {
    multiMap origin;
    origin.insert("dup", 1);
    origin.insert("dup", 2);

    multiMap copied(origin);
    ASSERT_EQ(copied.size(), 2);

    std::vector<int> got;
    for(auto e = copied.begin(); e != copied.end(); ++e)
        got.push_back(*e);
    ASSERT_EQ(got, (std::vector<int>{1, 2}));
}
