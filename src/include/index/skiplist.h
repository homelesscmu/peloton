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

#define MAX_LEVEL 32

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
    SkipList(): _curr_height(-1), _head(new Entry()), _duplicate(false) {
      key_comparator = nullptr;
      key_equality_checker = nullptr;
    }
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

    void set_duplicate(bool dup)
    {
        _duplicate = dup;
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
            while (curr->forwards[level] && curr->forwards[level]->key < target)
            {
                curr = curr->forwards[level];
            }
        }
        curr = curr->forwards[0];
        return curr && curr->key == target ? curr : nullptr;
    }

    bool insert(KeyType key, ValueType val)
    {
        std::vector<Entry*> updates(MAX_LEVEL, nullptr);
        auto curr = _head;
        for (int h = _curr_height; h >= 0; --h)
        {
            for (;curr->forwards[h] && curr->forwards[h]->key < key; curr = curr->forwards[h]);
            updates[h] = curr;
        }

        curr = curr->forwards[0];
        // already exist
        if (curr && curr->key == key && !_duplicate) {
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
            for (int h = 0; h <= _curr_height; ++h)
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
            for (;curr->forwards[h] && curr->forwards[h]->key < target; curr = curr->forwards[h]);
            updates[h] = curr;
        }

        curr = curr->forwards[0];
        if (curr && curr->key == target) {
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


private:
    int rand_level()
    {
        int level = 1;
        while (double(std::rand()) / RAND_MAX > 0.5)
        {
            ++level;
        }
        return level;
    }

private:
    int _curr_height;
    Entry *_head;
    bool _duplicate;

    KeyComparator *key_comparator;
    KeyEqualityChecker *key_equality_checker;
};

}
}
