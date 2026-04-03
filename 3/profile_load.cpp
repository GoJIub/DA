#include <cstdlib>
#include <iostream>
#include <string>

#include "profile_common.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <tmpfile>\n";
        return 1;
    }

    const uint32_t count = readSerializedCount(argv[1]);

    PATRICIA dict;
    const uint64_t elapsed = profileMeasure([&]() {
        volatile std::string err = dict.load(argv[1]);
        (void)err;
    });

    std::cout << "mode=load\n";
    std::cout << "count=" << count << '\n';
    std::cout << "elapsed_us=" << elapsed << '\n';
}
