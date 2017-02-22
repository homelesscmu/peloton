/**
 * This SKipList is an implementation of <ftp://ftp.cs.umd.edu/pub/skipLists/skiplists.pdf>
 */

#include <iostream>
#include <vector>
#include <cstdlib>


template <typename KeyType, typename ValueType, int MAX_LEVEL>
class SkipListEntry
{
public:
    SkipListEntry() = default;

    SkipListEntry(KeyType k, ValueType v): key(k), val(v) {}

    KeyType key;
    ValueType val;
    std::vector<SkipListEntry<KeyType, ValueType, MAX_LEVEL>*> forwards{MAX_LEVEL, nullptr};
};


template <typename KeyType, typename ValueType, int MAX_LEVEL>
class SkipList
{
    typedef SkipListEntry<KeyType, ValueType, MAX_LEVEL> Entry;

public:
    SkipList(): _curr_height(-1), _head(new Entry()), _duplicate(false) {}
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
        int r = std::rand() & ((1 << MAX_LEVEL) - 1);
        int level = 0;
        while (r & 1)
        {
            ++level;
            r >>= 1;
        }
        std::cout << "#####################" << std::endl
                  << level << std::endl
                  << "#####################" << std::endl;
        return level;
    }

private:
    int _curr_height;
    Entry *_head;
    bool _duplicate;
};
