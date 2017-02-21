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

#define SKIPLIST_TEMPLATE_ARGUMENTS                                       \
  template <typename KeyType, typename ValueType, typename KeyComparator, \
            typename KeyEqualityChecker, typename ValueEqualityChecker>

#define SKIPLIST_TYPENAME KeyType,ValueType,KeyComparator,KeyEqualityChecker,ValueEqualityChecker

#define MAX_LEVEL 16

namespace peloton {
namespace index {

SKIPLIST_TEMPLATE_ARGUMENTS
class SkipListEntry
{
public:
    SkipListEntry() = default;

    SkipListEntry(KeyType k, ValueType v): key(k), val(v) {}

    KeyType key;
    ValueType val;
    std::vector<SkipListEntry<SKIPLIST_TYPENAME>*> forwards{MAX_LEVEL, nullptr};
};

SKIPLIST_TEMPLATE_ARGUMENTS
class SkipList
{
    typedef SkipListEntry<SKIPLIST_TYPENAME> Entry;

public:
    SkipList(): _cur_height(0), _head(new Entry()) {}
    ~SkipList() { delete _head;}

    Entry *find(__attribute__ ((unused)) KeyType target)
    {
        return nullptr;
    }

    bool insert(KeyType key, ValueType val);
    bool remove(KeyType key);

private:
    int _cur_height;
    Entry *_head;
};

}
}
