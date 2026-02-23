#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using Pair = std::pair<int, std::string>;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<Pair> data;
    int key;
    std::string value;

    while (std::cin >> key >> value)
        data.push_back({key, value});

    std::sort(data.begin(), data.end(),
              [](const Pair& a, const Pair& b) { return a.first < b.first; });

    for (const auto& entry : data)
        std::cout << std::setw(6) << std::setfill('0') << entry.first
                  << '\t' << entry.second << '\n';
}