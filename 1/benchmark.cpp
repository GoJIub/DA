#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "sort.hpp"

using duration_t = std::chrono::milliseconds;
const std::string DURATION_PREFIX = "ms";

static uint64_t median(std::vector<uint64_t> v) {
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int runs = 1;
    if (argc >= 2) {
        runs = std::max(1, std::stoi(argv[1]));
    }

    std::vector<Pair> input;
    int key;
    std::string value;

    while (std::cin >> key >> value) {
        input.push_back({key, value});
    }

    std::vector<Pair> input_stl = input;

    std::cout << "Count of lines is " << input.size() << '\n';
    std::cout << "Runs: " << runs << '\n';

    std::vector<uint64_t> counting_times;
    std::vector<uint64_t> stl_times;
    counting_times.reserve(runs);
    stl_times.reserve(runs);

    for (int i = 0; i < runs; ++i) {
        auto input_copy = input;
        auto start_ts = std::chrono::system_clock::now();
        counting_sort(input_copy);
        auto end_ts = std::chrono::system_clock::now();
        counting_times.push_back(
            std::chrono::duration_cast<duration_t>(end_ts - start_ts).count());
    }

    for (int i = 0; i < runs; ++i) {
        auto input_stl_copy = input_stl;
        auto start_ts = std::chrono::system_clock::now();
        std::stable_sort(input_stl_copy.begin(), input_stl_copy.end(),
                         [](const Pair& a, const Pair& b) { return a.first < b.first; });
        auto end_ts = std::chrono::system_clock::now();
        stl_times.push_back(
            std::chrono::duration_cast<duration_t>(end_ts - start_ts).count());
    }

    const uint64_t counting_avg =
        std::accumulate(counting_times.begin(), counting_times.end(), uint64_t{0}) /
        counting_times.size();
    const uint64_t stl_avg =
        std::accumulate(stl_times.begin(), stl_times.end(), uint64_t{0}) / stl_times.size();

    std::cout << "Counting sort median time: " << median(counting_times) << DURATION_PREFIX
              << '\n';
    std::cout << "Counting sort avg time: " << counting_avg << DURATION_PREFIX << '\n';
    std::cout << "STL stable sort median time: " << median(stl_times) << DURATION_PREFIX
              << '\n';
    std::cout << "STL stable sort avg time: " << stl_avg << DURATION_PREFIX << '\n';
}
