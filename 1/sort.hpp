#pragma once

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

using Pair = std::pair<int, std::string>;

inline void counting_sort(std::vector<Pair>& data) {
    if (data.empty()) {
        return;
    }

    int max_key = data.front().first;
    int min_key = data.front().first;

    for (const auto& entry : data) {
        max_key = std::max(max_key, entry.first);
        min_key = std::min(min_key, entry.first);
    }

    const int key_range = max_key - min_key;
    std::vector<int> key_list(key_range + 1, 0);

    for (const auto& entry : data) {
        ++key_list[entry.first - min_key];
    }

    for (int i = 1; i <= key_range; ++i) {
        key_list[i] += key_list[i - 1];
    }

    std::vector<Pair> sorted_data(data.size());
    for (int i = static_cast<int>(data.size()) - 1; i >= 0; --i) {
        const int key = data[i].first;
        const int pos = --key_list[key - min_key];
        sorted_data[pos] = std::move(data[i]);
    }

    data = std::move(sorted_data);
}
