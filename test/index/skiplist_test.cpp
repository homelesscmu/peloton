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
  index::SkipList<KeyInt,ValueInt,int,int,int> list;
  KeyInt key;
  EXPECT_EQ(nullptr, list.find(key));
}

}  // End test namespace
}  // End peloton namespace
