#include "test/byeolTest.hpp"

typedef byeolTest me;

void me::SetUp() { _assertVault(); }

void me::TearDown() { _assertVault(); }

by::instance::vault& me::_getVault() { return by::instance::getVault(); }

void me::_assertVault() {
    auto& v = _getVault();
    BY_DI("vault[%s].len()=%d", &v, v.len());
    ASSERT_EQ(v.len(), 0);
}
