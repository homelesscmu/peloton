#include <iostream>
#include <map>
#include <unordered_map>
#include "skiplist.h"

using namespace std;

int main()
{
    std::unordered_map<int, string> data {
        {1, "one"},
        {5, "five"},
        {2, "two"},
        {4, "four"},
        {10, "ten"},
        {22, "twenty-two"},
        {8, "eight"}
    };

    cout << "in main" << endl;
    SkipList<int, string, 32> *sl = new SkipList<int, string, 32>();

    for (const auto & pair : data)
    {
        std::cout << "inserting (" << pair.first << ", " << pair.second << ")" << std::endl;
        sl->insert(pair.first, pair.second);
        sl->display();
        cout << endl;
    }


    std::vector<int> targets{1, 2, 3, 4};
    for (int target : targets)
    {
        cout << endl;
        auto result = sl->find(target);
        cout << "finding " << target << ", found " << (result ? result->val : "NULL") << endl;
        cout << "removing" << target << " result: " << (sl->remove(target) ? "Success" : "Fail") << endl;
        sl->display();
    }

    delete sl;

    return 0;
}
