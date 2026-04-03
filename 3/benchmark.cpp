#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "patricia.hpp"

namespace {

using clock_t = std::chrono::steady_clock;
using duration_t = std::chrono::microseconds;

struct Entry {
    std::string key;
    uint64_t value;
};

struct Result {
    std::vector<uint64_t> insert;
    std::vector<uint64_t> search;
    std::vector<uint64_t> remove;
};

std::string normalize(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return s;
}

std::string randomWord(std::mt19937_64& rng, int minLen = 6, int maxLen = 24) {
    std::uniform_int_distribution<int> lenDist(minLen, maxLen);
    std::uniform_int_distribution<int> charDist(0, 51);
    int len = lenDist(rng);
    std::string s;
    s.reserve(static_cast<size_t>(len));
    for (int i = 0; i < len; ++i) {
        int x = charDist(rng);
        s.push_back(static_cast<char>(x < 26 ? 'a' + x : 'A' + (x - 26)));
    }
    return s;
}

std::vector<Entry> makeDataset(size_t n, uint64_t seed) {
    std::mt19937_64 rng(seed);
    std::vector<Entry> data;
    data.reserve(n);
    std::map<std::string, bool> used;

    while (data.size() < n) {
        std::string word = randomWord(rng);
        std::string normalized = normalize(word);
        if (used.count(normalized) != 0) continue;
        used[normalized] = true;
        data.push_back({word, rng()});
    }

    return data;
}

uint64_t median(std::vector<uint64_t> values) {
    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

template <class Fn>
uint64_t measure(Fn&& fn) {
    auto start = clock_t::now();
    fn();
    auto finish = clock_t::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<duration_t>(finish - start).count());
}

Result benchmarkPatricia(const std::vector<Entry>& data, int runs) {
    Result result;
    result.insert.reserve(runs);
    result.search.reserve(runs);
    result.remove.reserve(runs);

    for (int i = 0; i < runs; ++i) {
        PATRICIA dict;

        result.insert.push_back(measure([&]() {
            for (const auto& entry : data) {
                dict.insert(entry.key, entry.value);
            }
        }));

        result.search.push_back(measure([&]() {
            for (const auto& entry : data) {
                volatile auto found = dict.search(entry.key);
                (void)found;
            }
        }));

        result.remove.push_back(measure([&]() {
            for (const auto& entry : data) {
                dict.remove(entry.key);
            }
        }));
    }

    return result;
}

Result benchmarkMap(const std::vector<Entry>& data, int runs) {
    Result result;
    result.insert.reserve(runs);
    result.search.reserve(runs);
    result.remove.reserve(runs);

    for (int i = 0; i < runs; ++i) {
        std::map<std::string, uint64_t> dict;

        result.insert.push_back(measure([&]() {
            for (const auto& entry : data) {
                dict.emplace(normalize(entry.key), entry.value);
            }
        }));

        result.search.push_back(measure([&]() {
            for (const auto& entry : data) {
                volatile auto it = dict.find(normalize(entry.key));
                (void)it;
            }
        }));

        result.remove.push_back(measure([&]() {
            for (const auto& entry : data) {
                dict.erase(normalize(entry.key));
            }
        }));
    }

    return result;
}

void printStats(const std::string& label, const Result& result) {
    auto avg = [](const std::vector<uint64_t>& values) {
        return std::accumulate(values.begin(), values.end(), uint64_t{0}) / values.size();
    };

    std::cout << label << " insert median(us): " << median(result.insert)
              << ", avg(us): " << avg(result.insert) << '\n';
    std::cout << label << " search median(us): " << median(result.search)
              << ", avg(us): " << avg(result.search) << '\n';
    std::cout << label << " remove median(us): " << median(result.remove)
              << ", avg(us): " << avg(result.remove) << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    size_t size = 100000;
    int runs = 5;
    uint64_t seed = 42;

    if (argc >= 2) size = static_cast<size_t>(std::stoull(argv[1]));
    if (argc >= 3) runs = std::max(1, std::stoi(argv[2]));
    if (argc >= 4) seed = std::stoull(argv[3]);

    const auto data = makeDataset(size, seed);

    std::cout << "dataset_size=" << size << '\n';
    std::cout << "runs=" << runs << '\n';

    const Result patricia = benchmarkPatricia(data, runs);
    const Result orderedMap = benchmarkMap(data, runs);

    printStats("patricia", patricia);
    printStats("std::map", orderedMap);
}
