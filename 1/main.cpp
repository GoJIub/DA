#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "sort.hpp"

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<Pair> data;
    int key;
    std::string value;

    while (std::cin >> key >> value) {
        data.push_back({key, value});
    }

    if (data.empty()) return 0;

    counting_sort(data);

    for (const auto& entry : data)
        std::cout << std::setw(6) << std::setfill('0') << entry.first << '\t' << entry.second << '\n';
}
