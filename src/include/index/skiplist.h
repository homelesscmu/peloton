//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// skiplist.h
//
// Identification: src/include/index/skiplist.h
//
// Copyright (c) 2015-17, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <atomic>

#define SKIPLIST_TEMPLATE_ARGUMENTS                                       \
  template <typename KeyType, typename ValueType, typename KeyComparator, \
            typename KeyEqualityChecker, typename ValueEqualityChecker>

#define SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS template \
  <typename KeyType, typename ValueType>

namespace peloton {
namespace index {
namespace skiplist{

constexpr int MAX_LEVEL = 32;

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
class SkipListEntry
{
public:
    SkipListEntry() = default;
    SkipListEntry(KeyType k, ValueType v): key(k), val(v) {}
    KeyType key;
    ValueType val;
    std::vector<SkipListEntry<KeyType, ValueType>*> forwards{MAX_LEVEL, nullptr};
};

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
class EpochManager;

template <typename KeyType,
          typename ValueType,
          typename KeyComparator = std::less<KeyType>,
          typename KeyEqualityChecker = std::equal_to<KeyType>,
          typename ValueEqualityChecker = std::equal_to<ValueType>>
class SkipList
{
    typedef SkipListEntry<KeyType, ValueType> Entry;

public:

    SkipList(KeyComparator key_cmp_obj = KeyComparator{},
             KeyEqualityChecker key_eq_obj = KeyEqualityChecker{},
             ValueEqualityChecker val_eq_obj = ValueEqualityChecker{})
             : _curr_height(-1)
             , _head(new Entry())
             , _key_cmp_obj(key_cmp_obj)
             , _key_eq_obj(key_eq_obj)
             , _val_eq_obj(val_eq_obj)
             {}

    ~SkipList()
    {
        auto curr = _head->forwards[0];
        while (curr)
        {
            auto temp = curr;
            curr = curr->forwards[0];
            delete temp;
        }
        delete _head;
    }

    void display()
    {
        for (auto h = _curr_height; h >= 0; --h)
        {
            for (auto curr = _head->forwards[h]; curr; curr = curr->forwards[h])
            {
                std::cout << "(" << curr->key << ", " << curr->val << ") -> ";
            }
            std::cout << std::endl;
        }
    }

    Entry* find(KeyType target)
    {
        auto curr = _head;
        for (int level = _curr_height; level >= 0; --level)
        {
            while (curr->forwards[level] && _key_cmp_obj(curr->forwards[level]->key, target))
            {
                curr = curr->forwards[level];
            }
        }
        curr = curr->forwards[0];
        return curr && _key_eq_obj(curr->key, target) ? curr : nullptr;
    }

    bool insert(KeyType key, ValueType val)
    {
        std::vector<Entry*> updates(MAX_LEVEL, nullptr);
        auto curr = _head;
        for (int h = _curr_height; h >= 0; --h)
        {
            for (;curr->forwards[h] && _key_cmp_obj(curr->forwards[h]->key, key); curr = curr->forwards[h]);
            updates[h] = curr;
        }

        curr = curr->forwards[0];
        // already exist
        if (curr && _key_eq_obj(curr->key, key)) {
            return false;
        }
        else {
            int new_h = rand_level();
            if (new_h > _curr_height) {
                for (int h = _curr_height + 1; h <= new_h; ++h)
                {
                    updates[h] = _head;
                }
                _curr_height = new_h;
            }
            curr = new Entry(key, val);
            for (int h = 0; h <= new_h; ++h)
            {
                curr->forwards[h] = updates[h]->forwards[h];
                updates[h]->forwards[h] = curr;
            }
            return true;
        }
    }

    bool remove(KeyType target)
    {
        std::vector<Entry*> updates(MAX_LEVEL, nullptr);
        auto curr = _head;
        for (int h = _curr_height; h >= 0; --h)
        {
            for (;curr->forwards[h] && _key_cmp_obj(curr->forwards[h]->key, target); curr = curr->forwards[h]);
            updates[h] = curr;
        }

        curr = curr->forwards[0];
        if (curr && _key_eq_obj(curr->key, target)) {
            for (int h = 0; h <= _curr_height; ++h)
            {
                if (updates[h]->forwards[h] != curr) {
                    break;
                }
                updates[h]->forwards[h] = curr->forwards[h];
            }
            delete curr;
            while (_curr_height > 0 && _head->forwards[_curr_height] == nullptr)
            {
                --_curr_height;
            }
            return true;
        }
        else {
            return false;
        }
    }

    EpochManager<KeyType, ValueType> epoch_manager;

private:
    int rand_level()
    {
        int r = std::rand() & ((1L << MAX_LEVEL) - 1);
        int level = 0;
        while (r & 1)
        {
            ++level;
            r >>= 1;
        }
        return level;
    }

private:
    int _curr_height;
    Entry *_head;
    const KeyComparator _key_cmp_obj;
    const KeyEqualityChecker _key_eq_obj;
    const ValueEqualityChecker _val_eq_obj;
};

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
class EpochManager
{
public:

  constexpr static int GC_INTERVAL = 50;

  struct GarbageNode {
    const SkipListEntry<KeyType, ValueType> *entry_p;
    GarbageNode *next_p;
  };

  class EpochNode {
   public:
    std::atomic<int> active_thread_count;
    std::atomic<GarbageNode *> garbage_list_p;
    EpochNode *next_p;

    EpochNode();
  };

  EpochNode *head_epoch_p;
  EpochNode *current_epoch_p;
  std::atomic<bool> exited_flag;

  EpochManager();
  ~EpochManager();

  void AddGarbageNode(const SkipListEntry<KeyType, ValueType> *entry_p);
  inline EpochNode *JoinEpoch();
  inline void LeaveEpoch(EpochNode *epoch_p);

  void PerformGarbageCollection();
  void CreateNewEpoch();
  void ClearEpoch();

  void StartThread();

private:
  void FreeEpochDeltaChain(const SkipListEntry<KeyType, ValueType> *entry_p);

  void ThreadFunc();
};

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
EpochManager<KeyType, ValueType>::EpochNode::EpochNode() {
  active_thread_count = 0;
  garbage_list_p = nullptr;
  next_p = nullptr;
}

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
EpochManager<KeyType, ValueType>::EpochManager() {
  head_epoch_p = new EpochNode();

  current_epoch_p = head_epoch_p;

  exited_flag.store(false);

  return;
}

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
EpochManager<KeyType, ValueType>::~EpochManager() {
  exited_flag.store(true);
  current_epoch_p = nullptr;

  auto *epoch_node_p = head_epoch_p;
  while (epoch_node_p != nullptr) {
    auto prev_node_p = epoch_node_p;
    epoch_node_p = epoch_node_p->next_p;
    delete prev_node_p;
  }
  head_epoch_p = nullptr;

  EpochManager<KeyType, ValueType>::ClearEpoch();
  assert(head_epoch_p == nullptr);

  return;
}

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
void EpochManager<KeyType, ValueType>::CreateNewEpoch() {
  EpochNode *epoch_node_p = new EpochNode();
  current_epoch_p->next_p = epoch_node_p;
  current_epoch_p = epoch_node_p;
  return;
}

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
void EpochManager<KeyType, ValueType>::ClearEpoch() {
  // TODO: to be implemented.
  return;
}

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
inline typename EpochManager<KeyType, ValueType>::EpochNode *EpochManager<KeyType, ValueType>::JoinEpoch() {
  EpochNode *epoch_p = current_epoch_p;
  epoch_p->active_thread_count.fetch_add(1);
  return epoch_p;
}

SKIPLSIT_ENTRY_TEMPLATE_ARGUMENTS
inline void EpochManager<KeyType, ValueType>::LeaveEpoch(EpochNode *epoch_p) {
  epoch_p->active_thread_count.fetch_sub(1);
  return;
}

}
}
}
