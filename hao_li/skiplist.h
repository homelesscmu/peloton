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
