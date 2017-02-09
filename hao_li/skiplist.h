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
    ~SkipList() { delete _head;}

    Entry find(KeyType target);
    bool insert(KeyType key, ValueType val);
    bool remove(KeyType key);

private:
    int _cur_height;
    Entry *_head;
};
