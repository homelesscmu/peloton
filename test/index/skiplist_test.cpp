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

TEST_F(SkipListTests, EpochJoinAndExit) {
  index::skiplist::SkipList<int, int, int ,int ,int> list;
  auto &epoch_manager = list.epoch_manager;

  // Epoch #1
  epoch_manager.CreateNewEpoch();
  auto epoch1 = epoch_manager.JoinEpoch();
  auto epoch2 = epoch_manager.JoinEpoch();
  EXPECT_EQ(epoch1, epoch2);
  EXPECT_EQ(epoch1->active_thread_count, 2);

  // Epoch #2
  epoch_manager.CreateNewEpoch();
  auto epoch3 = epoch_manager.JoinEpoch();
  EXPECT_EQ(epoch1->active_thread_count, 2);
  EXPECT_EQ(epoch3->active_thread_count, 1);

  epoch_manager.LeaveEpoch(epoch1);
  EXPECT_EQ(epoch2->active_thread_count, 1);
}

}  // End test namespace
}  // End peloton namespace
