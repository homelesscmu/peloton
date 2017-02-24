#include <iostream>
#include <map>
#include <unordered_map>
#include "skiplist.h"

using namespace std;

int main()
{
    vector<pair<int, string>> data;
    data.emplace_back(std::make_pair(1, "one"));
    data.emplace_back(std::make_pair(10, "ten"));
    data.emplace_back(std::make_pair(5, "five"));
    data.emplace_back(std::make_pair(2, "two"));
    data.emplace_back(std::make_pair(8, "eight"));
    data.emplace_back(std::make_pair(8, "dup-eight"));
    data.emplace_back(std::make_pair(10, "dup-ten"));

    SkipList<int, string, 32> *sl = new SkipList<int, string, 32>();
    sl->set_duplicate(true);

    for (const auto & pair : data)
    {
        std::cout << "inserting (" << pair.first << ", " << pair.second << ") ";
        auto result = sl->insert(pair.first, pair.second) ? "Success" : "Failed";
        cout << result << endl;
        sl->display();
        cout << endl;
    }


    std::vector<int> targets{1, 2, 3, 4, 8};
    for (int target : targets)
    {
        cout << endl;
        auto result = sl->find(target);
        cout << "finding " << target << ", found " << (result ? result->val : "NULL") << endl;
        cout << "removing " << target << " result: " << (sl->remove(target) ? "Success" : "Fail") << endl;
        sl->display();
    }

    delete sl;

    return 0;
}
