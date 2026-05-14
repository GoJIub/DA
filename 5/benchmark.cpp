#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define main suffix_tree_solution_main
#include "main.cpp"
#undef main

struct TestCase {
    std::string name;
    std::string scenario;
    std::string text;
    std::vector<std::string> patterns;
};

struct TimedRun {
    std::uint64_t build_us = 0;
    std::uint64_t suffix_tree_search_us = 0;
    std::uint64_t std_find_us = 0;
    std::size_t matches = 0;
};

const std::size_t REPEATS = 5;

std::uint64_t NowUs() {
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}

std::string MakeRandomText(std::size_t length, int alphabet_size, std::uint32_t seed) {
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(0, alphabet_size - 1);
    std::string text;
    text.reserve(length);

    for (std::size_t i = 0; i < length; ++i)
        text.push_back(static_cast<char>('a' + distribution(generator)));

    return text;
}

std::string MakePeriodicText(std::size_t length, const std::string& period) {
    std::string text;
    text.reserve(length);

    for (std::size_t i = 0; i < length; ++i)
        text.push_back(period[i % period.size()]);

    return text;
}

std::vector<std::string> MakeMixedPatterns(const std::string& text, std::size_t count,
                                           int min_length, int max_length, std::uint32_t seed) {
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> length_distribution(min_length, max_length);
    std::uniform_int_distribution<int> char_distribution(0, 3);
    std::vector<std::string> patterns;
    patterns.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const int length = length_distribution(generator);

        if (i % 2 == 0 && static_cast<std::size_t>(length) <= text.size()) {
            std::uniform_int_distribution<std::size_t> start_distribution(0, text.size() - length);
            const std::size_t start = start_distribution(generator);
            patterns.push_back(text.substr(start, length));
        } else {
            std::string pattern;
            pattern.reserve(length);
            for (int j = 0; j < length; ++j)
                pattern.push_back(static_cast<char>('a' + char_distribution(generator)));
            patterns.push_back(pattern);
        }
    }

    return patterns;
}

std::vector<std::string> MakeRepeatedPatterns(std::size_t max_length) {
    std::vector<std::string> patterns;
    patterns.reserve(max_length);

    for (std::size_t length = 1; length <= max_length; ++length)
        patterns.push_back(std::string(length, 'a'));

    return patterns;
}

std::vector<int> FindWithStd(const std::string& text, const std::string& pattern) {
    std::vector<int> positions;
    std::size_t position = text.find(pattern);

    while (position != std::string::npos) {
        positions.push_back(static_cast<int>(position));
        position = text.find(pattern, position + 1);
    }

    return positions;
}

TimedRun RunOnce(const TestCase& test_case) {
    TimedRun run;

    const std::uint64_t build_start = NowUs();
    SufTree tree(test_case.text);
    const std::uint64_t build_finish = NowUs();
    run.build_us = build_finish - build_start;

    std::vector<std::vector<int>> suffix_tree_results;
    suffix_tree_results.reserve(test_case.patterns.size());

    const std::uint64_t suffix_tree_start = NowUs();
    for (const std::string& pattern : test_case.patterns) {
        std::vector<int> positions = tree.search(pattern);
        std::sort(positions.begin(), positions.end());
        run.matches += positions.size();
        suffix_tree_results.push_back(std::move(positions));
    }
    const std::uint64_t suffix_tree_finish = NowUs();
    run.suffix_tree_search_us = suffix_tree_finish - suffix_tree_start;

    std::vector<std::vector<int>> std_find_results;
    std_find_results.reserve(test_case.patterns.size());

    std::size_t std_matches = 0;
    const std::uint64_t std_start = NowUs();
    for (const std::string& pattern : test_case.patterns) {
        std::vector<int> positions = FindWithStd(test_case.text, pattern);
        std_matches += positions.size();
        std_find_results.push_back(std::move(positions));
    }
    const std::uint64_t std_finish = NowUs();
    run.std_find_us = std_finish - std_start;

    if (std_matches != run.matches)
        throw std::runtime_error("match count differs from std::string::find");
    if (suffix_tree_results != std_find_results)
        throw std::runtime_error("suffix tree result differs from std::string::find");

    return run;
}

std::uint64_t Median(std::vector<std::uint64_t> values) {
    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

std::string JoinRaw(const std::vector<std::uint64_t>& values) {
    std::ostringstream output;

    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0)
            output << ';';
        output << values[i];
    }

    return output.str();
}

int main() {
    try {
        const std::string random_1000 = MakeRandomText(1000, 4, 1);
        const std::string random_10000 = MakeRandomText(10000, 8, 2);
        const std::string random_30000 = MakeRandomText(30000, 16, 3);
        const std::string periodic_30000 = MakePeriodicText(30000, "abacaba");
        const std::string same_20000(20000, 'a');

        const std::vector<TestCase> cases = {
            {"unique_match", "unique substrings (low match density)",
            MakeRandomText(30000, 26, 42),
            []() -> std::vector<std::string> {
                std::vector<std::string> patterns;

                const std::string base = MakeRandomText(30000, 26, 123);

                const std::size_t step = 3000;
                const std::size_t len = 10;

                for (std::size_t i = 0; i + len <= base.size(); i += step) {
                    patterns.push_back(base.substr(i, len));
                }

                return patterns;
            }()},

            {"random_1000", "random alphabet=4",
            random_1000,
            MakeMixedPatterns(random_1000, 100, 3, 8, 11)},

            {"random_10000", "random alphabet=8",
            random_10000,
            MakeMixedPatterns(random_10000, 500, 4, 10, 12)},

            {"random_30000", "random alphabet=16",
            random_30000,
            MakeMixedPatterns(random_30000, 1000, 5, 12, 13)},

            {"periodic_30000", "periodic abacaba",
            periodic_30000,
            MakeMixedPatterns(periodic_30000, 500, 3, 14, 14)},

            {"same_20000", "all equal",
            same_20000,
            MakeRepeatedPatterns(20)}
        };

        std::cout
            << "case,scenario,text_length,patterns,repeats,matches,"
            << "build_raw_us,suffix_tree_search_raw_us,std_find_raw_us,"
            << "build_median_us,suffix_tree_search_median_us,std_find_median_us,"
            << "std_find_over_suffix_tree_search\n";

        for (const TestCase& test_case : cases) {
            std::vector<std::uint64_t> build_times;
            std::vector<std::uint64_t> suffix_tree_search_times;
            std::vector<std::uint64_t> std_find_times;
            std::size_t matches = 0;

            for (std::size_t repeat = 0; repeat < REPEATS; ++repeat) {
                const TimedRun run = RunOnce(test_case);

                if (repeat == 0)
                    matches = run.matches;
                else if (matches != run.matches)
                    throw std::runtime_error("match count changed between repeats");

                build_times.push_back(run.build_us);
                suffix_tree_search_times.push_back(run.suffix_tree_search_us);
                std_find_times.push_back(run.std_find_us);
            }

            const std::uint64_t build_median = Median(build_times);
            const std::uint64_t suffix_tree_search_median = Median(suffix_tree_search_times);
            const std::uint64_t std_find_median = Median(std_find_times);
            const double ratio = suffix_tree_search_median == 0
                ? 0.0
                : static_cast<double>(std_find_median) / suffix_tree_search_median;

            std::cout
                << test_case.name << ','
                << '"' << test_case.scenario << '"' << ','
                << test_case.text.size() << ','
                << test_case.patterns.size() << ','
                << REPEATS << ','
                << matches << ','
                << '"' << JoinRaw(build_times) << '"' << ','
                << '"' << JoinRaw(suffix_tree_search_times) << '"' << ','
                << '"' << JoinRaw(std_find_times) << '"' << ','
                << build_median << ','
                << suffix_tree_search_median << ','
                << std_find_median << ','
                << std::fixed << std::setprecision(3) << ratio << '\n';
        }
    } catch (const std::exception& error) {
        std::cerr << "benchmark error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
