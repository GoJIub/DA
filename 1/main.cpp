#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using Pair = std::pair<int, std::string>;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<Pair> data;
    int n = 0;
    
    int max_key = 0;
    int min_key = 1000000;

    int key;
    std::string value;

    while (std::cin >> key >> value) {
        max_key = std::max(max_key, key);
        min_key = std::min(min_key, key);
        
        data.push_back({key, value});
        ++n;
    }

    if (data.empty()) return 0;

    int k = max_key - min_key;
    std::vector<int> key_list(k + 1, 0);

    for (const auto& entry : data)
        ++key_list[entry.first - min_key];

    for (int i = 1; i < k + 1; ++i)
        key_list[i] += key_list[i - 1];

    std::vector<Pair> sort_data(n);

    for (int i = n - 1; i >= 0; --i) {
        int key = data[i].first;
        int pos = --key_list[key - min_key];
        sort_data[pos] = std::move(data[i]);
    }

    for (const auto& entry : sort_data)
        std::cout << std::setw(6) << std::setfill('0') << entry.first << '\t' << entry.second << '\n';
}
