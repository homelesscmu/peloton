//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// skiplist_test.cpp
//
// Identification: test/index/skiplist_test.cpp
//
// Copyright (c) 2015-17, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "common/harness.h"
#include "gtest/gtest.h"

#include "index/skiplist.h"

namespace peloton {
namespace test {

class SkipListTests : public PelotonTest {};

class KeyInt {};
class ValueInt {};

TEST_F(SkipListTests, BasicTest) {
  index::SkipList<int, int, int ,int ,int> list;
  EXPECT_EQ(nullptr, nullptr);
}

}  // End test namespace
}  // End peloton namespace
