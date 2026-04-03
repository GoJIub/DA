#include <iostream>
#include <string>

#include "profile_common.hpp"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <dataset> <tmpfile>\n";
        return 1;
    }

    const std::vector<ProfileEntry> data = loadProfileDataset(argv[1]);
    if (const int status = ensureDataLoaded(data); status != 0) return status;

    PATRICIA dict;
    for (const auto& entry : data) {
        dict.insert(entry.key, entry.value);
    }

    const uint64_t elapsed = profileMeasure([&]() {
        volatile std::string err = dict.save(argv[2]);
        (void)err;
    });

    std::cout << "mode=save\n";
    std::cout << "count=" << data.size() << '\n';
    std::cout << "elapsed_us=" << elapsed << '\n';
}
