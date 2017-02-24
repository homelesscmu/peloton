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


namespace peloton {
namespace index {
namespace skiplist{

constexpr int MAX_LEVEL = 32;


// TODO: add valid flag to support logical deletion
template <typename KeyType, typename ValueType>
class SkipListEntry
{
public:
    SkipListEntry() = default;
    SkipListEntry(KeyType k, ValueType v): key(k), val(v) {}
    KeyType key;
    ValueType val;
    std::vector<SkipListEntry<KeyType, ValueType>*> forwards{MAX_LEVEL, nullptr};
};


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

    void Display()
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

    Entry* Find(KeyType target_key)
    {
        auto curr = _head;
        for (int level = _curr_height; level >= 0; --level)
        {
            while (curr->forwards[level] && _key_cmp_obj(curr->forwards[level]->key, target_key))
            {
                curr = curr->forwards[level];
            }
        }
        curr = curr->forwards[0];
        return curr && _key_eq_obj(curr->key, target_key) ? curr : nullptr;
    }

    // TODO: latch-free
    bool Insert(KeyType inserted_key, ValueType inserted_val)
    {
        std::vector<Entry*> updates(MAX_LEVEL, nullptr);
        auto curr = _head;
        for (int h = _curr_height; h >= 0; --h)
        {
            for (;curr->forwards[h] && _key_cmp_obj(curr->forwards[h]->key, inserted_key); curr = curr->forwards[h]);
            updates[h] = curr;
        }

        curr = curr->forwards[0];
        // already exist
        if (curr && _key_eq_obj(curr->key, inserted_key)) {
            return false;
        }
        else {
            int new_h = RandLevel();
            if (new_h > _curr_height) {
                for (int h = _curr_height + 1; h <= new_h; ++h)
                {
                    updates[h] = _head;
                }
                _curr_height = new_h;
            }
            curr = new Entry(inserted_key, inserted_val);
            for (int h = 0; h <= new_h; ++h)
            {
                curr->forwards[h] = updates[h]->forwards[h];
                updates[h]->forwards[h] = curr;
            }
            return true;
        }
    }


    // TODO: latch-free
    bool Remove(KeyType target_key, ValueType target_val)
    {
        std::vector<Entry*> updates(MAX_LEVEL, nullptr);
        auto curr = _head;
        for (int h = _curr_height; h >= 0; --h)
        {
            for (;curr->forwards[h] && _key_cmp_obj(curr->forwards[h]->key, target_key); curr = curr->forwards[h]);
            updates[h] = curr;
        }

        curr = curr->forwards[0];
        if (curr && _key_eq_obj(curr->key, target_key) && _val_eq_obj(curr->val, target_val)) {
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
    int RandLevel()
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

}
}
}
