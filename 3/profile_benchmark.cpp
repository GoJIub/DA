#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
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

std::vector<Entry> loadDataset(const std::string& path) {
    std::ifstream input(path);
    std::vector<Entry> data;

    std::string key;
    uint64_t value = 0;
    while (input >> key >> value) {
        data.push_back({key, value});
    }

    return data;
}

template <class Fn>
uint64_t measure(Fn&& fn) {
    auto start = clock_t::now();
    fn();
    auto finish = clock_t::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<duration_t>(finish - start).count());
}

void runInsert(const std::vector<Entry>& data) {
    PATRICIA dict;
    const uint64_t elapsed = measure([&]() {
        for (const auto& entry : data) {
            dict.insert(entry.key, entry.value);
        }
    });
    std::cout << "mode=insert\n";
    std::cout << "count=" << data.size() << '\n';
    std::cout << "elapsed_us=" << elapsed << '\n';
}

void runSearch(const std::vector<Entry>& data) {
    PATRICIA dict;
    for (const auto& entry : data) {
        dict.insert(entry.key, entry.value);
    }

    const uint64_t elapsed = measure([&]() {
        for (const auto& entry : data) {
            volatile auto found = dict.search(entry.key);
            (void)found;
        }
    });
    std::cout << "mode=search\n";
    std::cout << "count=" << data.size() << '\n';
    std::cout << "elapsed_us=" << elapsed << '\n';
}

void runRemove(const std::vector<Entry>& data) {
    PATRICIA dict;
    for (const auto& entry : data) {
        dict.insert(entry.key, entry.value);
    }

    const uint64_t elapsed = measure([&]() {
        for (const auto& entry : data) {
            dict.remove(entry.key);
        }
    });
    std::cout << "mode=remove\n";
    std::cout << "count=" << data.size() << '\n';
    std::cout << "elapsed_us=" << elapsed << '\n';
}

void runSave(const std::vector<Entry>& data, const std::string& path) {
    PATRICIA dict;
    for (const auto& entry : data) {
        dict.insert(entry.key, entry.value);
    }

    const uint64_t elapsed = measure([&]() {
        volatile std::string err = dict.save(path);
        (void)err;
    });
    std::cout << "mode=save\n";
    std::cout << "count=" << data.size() << '\n';
    std::cout << "elapsed_us=" << elapsed << '\n';
}

void runLoad(const std::vector<Entry>& data, const std::string& path) {
    {
        PATRICIA dict;
        for (const auto& entry : data) {
            dict.insert(entry.key, entry.value);
        }
        const std::string err = dict.save(path);
        if (!err.empty()) {
            std::cerr << "failed to prepare load fixture: " << err << '\n';
            std::exit(1);
        }
    }

    PATRICIA dict;
    const uint64_t elapsed = measure([&]() {
        volatile std::string err = dict.load(path);
        (void)err;
    });
    std::cout << "mode=load\n";
    std::cout << "count=" << data.size() << '\n';
    std::cout << "elapsed_us=" << elapsed << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <insert|search|remove|save|load> <dataset> [tmpfile]\n";
        return 1;
    }

    const std::string mode = argv[1];
    const std::string datasetPath = argv[2];
    const std::string tmpPath = argc >= 4 ? argv[3] : "/tmp/patricia_profile.bin";

    const std::vector<Entry> data = loadDataset(datasetPath);
    if (data.empty()) {
        std::cerr << "dataset is empty or cannot be read\n";
        return 1;
    }

    if (mode == "insert") runInsert(data);
    else if (mode == "search") runSearch(data);
    else if (mode == "remove") runRemove(data);
    else if (mode == "save") runSave(data, tmpPath);
    else if (mode == "load") runLoad(data, tmpPath);
    else {
        std::cerr << "unknown mode\n";
        return 1;
    }
}
