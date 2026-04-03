#ifndef PROFILE_COMMON_HPP
#define PROFILE_COMMON_HPP

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "patricia.hpp"

struct ProfileEntry {
    std::string key;
    uint64_t value;
};

using profile_clock_t = std::chrono::steady_clock;
using profile_duration_t = std::chrono::microseconds;

inline std::vector<ProfileEntry> loadProfileDataset(const std::string& path) {
    std::ifstream input(path);
    std::vector<ProfileEntry> data;

    std::string key;
    uint64_t value = 0;
    while (input >> key >> value) {
        data.push_back({key, value});
    }

    return data;
}

template <class Fn>
uint64_t profileMeasure(Fn&& fn) {
    const auto start = profile_clock_t::now();
    fn();
    const auto finish = profile_clock_t::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<profile_duration_t>(finish - start).count());
}

inline int ensureDataLoaded(const std::vector<ProfileEntry>& data) {
    if (!data.empty()) return 0;
    std::cerr << "dataset is empty or cannot be read\n";
    return 1;
}

inline uint32_t readSerializedCount(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) return 0;

    uint32_t count = 0;
    if (!input.read(reinterpret_cast<char*>(&count), sizeof(count))) return 0;
    return count;
}

#endif
