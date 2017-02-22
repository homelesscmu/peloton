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
    SkipList(): _cur_height(0), _head(new Entry()) {}
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
        for (auto h = _cur_height; h >= 0; --h)
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
        auto cur = _head;
        for (int level = _cur_height; level >= 0; --level)
        {
            while (cur->forwards[level] && cur->forwards[level]->key < target)
            {
                cur = cur->forwards[level];
            }
        }
        cur = cur->forwards[0];
        return cur->key == target ? cur : nullptr;
    }

    bool insert(KeyType key, ValueType val)
    {
        std::vector<Entry*> updates(MAX_LEVEL, nullptr);
        auto curr = _head;
        for (int h = _cur_height; h >= 0; --h)
        {
            for (;curr->forwards[h] && curr->forwards[h]->key < key; curr = curr->forwards[h]);
            updates[h] = curr;
        }

        curr = curr->forwards[0];
        if (curr && curr->key == key) {
            curr->val = val;
        }
        else {
            int new_h = rand_level();
            if (new_h > _cur_height) {
                for (int h = _cur_height + 1; h <= new_h; ++h)
                {
                    updates[h] = _head;
                }
                _cur_height = new_h;
            }
            curr = new Entry(key, val);
            for (int h = 0; h <= _cur_height; ++h)
            {
                curr->forwards[h] = updates[h]->forwards[h];
                updates[h]->forwards[h] = curr;
            }
        }
    }

    bool remove(KeyType target)
    {
        std::vector<Entry*> updates(MAX_LEVEL, nullptr);
        auto curr = _head;
        for (int h = _cur_height; h >= 0; --h)
        {
            for (;curr->forwards[h] && curr->forwards[h]->key < target; curr = curr->forwards[h]);
            updates[h] = curr;
        }

        curr = curr->forwards[0];
        if (curr->key == target) {
            for (int h = 0; h <= _cur_height; ++h)
            {
                if (updates[h]->forwards[h] != curr) {
                    break;
                }
                updates[h]->forwards[h] = curr->forwards[h];
            }
            delete curr;
            while (_cur_height > 0 && _head->forwards[_cur_height] == nullptr)
            {
                --_cur_height;
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
    int _cur_height;
    Entry *_head;
};
