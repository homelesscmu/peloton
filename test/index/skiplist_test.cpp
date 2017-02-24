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
  index::skiplist::SkipList<int, int> *sl = new index::skiplist::SkipList<int, int>();
  int key = 1;
  int val = 1000;
  sl->insert(key, val);
  auto found = sl->find(key);
  EXPECT_EQ(val, found->val);
  delete sl;
}

}  // End test namespace
}  // End peloton namespace
