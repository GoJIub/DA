#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define main solution_main
#include "main.cpp"
#undef main

namespace {

using Clock = std::chrono::steady_clock;

const std::uint64_t NAIVE_AUTO_LIMIT = 100000000;

struct InputData {
    std::vector<std::string> pattern;
    std::vector<std::string> text;
};

struct Options {
    std::string case_name = "manual";
    std::string scenario = "manual";
    std::size_t inserted_matches = 0;
    std::size_t seed = 0;
    std::size_t repeats = 5;
    std::string naive_mode = "auto";
};

struct ZRun {
    std::uint64_t prepare_us = 0;
    std::uint64_t compute_us = 0;
    std::uint64_t total_us = 0;
    std::uint64_t matches = 0;
};

std::uint64_t ToUint64(const std::string& value, const std::string& name) {
    char* end = nullptr;
    const unsigned long long parsed = std::strtoull(value.c_str(), &end, 10);

    if (end == value.c_str() || *end != '\0')
        throw std::runtime_error("invalid integer for " + name + ": " + value);

    return static_cast<std::uint64_t>(parsed);
}

Options ParseOptions(int argc, char** argv) {
    Options options;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--case-name" && i + 1 < argc)
            options.case_name = argv[++i];
        else if (arg == "--scenario" && i + 1 < argc)
            options.scenario = argv[++i];
        else if (arg == "--inserted" && i + 1 < argc)
            options.inserted_matches = ToUint64(argv[++i], "--inserted");
        else if (arg == "--seed" && i + 1 < argc)
            options.seed = ToUint64(argv[++i], "--seed");
        else if (arg == "--repeats" && i + 1 < argc)
            options.repeats = ToUint64(argv[++i], "--repeats");
        else if (arg == "--naive" && i + 1 < argc)
            options.naive_mode = argv[++i];
        else
            throw std::runtime_error("unknown or incomplete argument: " + arg);
    }

    if (options.repeats == 0)
        throw std::runtime_error("--repeats must be positive");
    if (options.naive_mode != "auto" && options.naive_mode != "on" && options.naive_mode != "off")
        throw std::runtime_error("--naive must be one of: auto, on, off");

    return options;
}

void ParseWords(const std::string& line, std::vector<std::string>& output) {
    std::stringstream stream(line);
    std::string word;

    while (stream >> word) {
        toLowerCase(word);
        output.push_back(word);
    }
}

InputData ReadInput() {
    InputData input;
    std::string line;

    if (!std::getline(std::cin, line))
        throw std::runtime_error("input has no pattern line");

    ParseWords(line, input.pattern);

    if (input.pattern.empty())
        throw std::runtime_error("pattern is empty");

    while (std::getline(std::cin, line))
        ParseWords(line, input.text);

    return input;
}

std::uint64_t NowUs() {
    const auto now = Clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
}

ZRun RunZSearch(const InputData& input) {
    ZRun run;

    const std::uint64_t prepare_start = NowUs();
    std::vector<std::string> combined;
    combined.reserve(input.pattern.size() + input.text.size() + 1);
    combined.insert(combined.end(), input.pattern.begin(), input.pattern.end());
    combined.push_back("{sentinel}");
    combined.insert(combined.end(), input.text.begin(), input.text.end());
    const std::uint64_t prepare_end = NowUs();

    const std::uint64_t compute_start = NowUs();
    const std::vector<int> z = computeZ(combined);
    const int pattern_size = static_cast<int>(input.pattern.size());
    std::uint64_t matches = 0;

    for (std::size_t i = input.pattern.size() + 1; i < combined.size(); ++i) {
        if (z[i] >= pattern_size)
            ++matches;
    }

    const std::uint64_t compute_end = NowUs();

    run.prepare_us = prepare_end - prepare_start;
    run.compute_us = compute_end - compute_start;
    run.total_us = run.prepare_us + run.compute_us;
    run.matches = matches;

    return run;
}

std::uint64_t RunNaive(const InputData& input) {
    std::uint64_t matches = 0;
    const std::size_t pattern_size = input.pattern.size();

    if (pattern_size > input.text.size())
        return 0;

    for (std::size_t start = 0; start + pattern_size <= input.text.size(); ++start) {
        bool equal = true;

        for (std::size_t offset = 0; offset < pattern_size; ++offset) {
            if (input.text[start + offset] != input.pattern[offset]) {
                equal = false;
                break;
            }
        }

        if (equal)
            ++matches;
    }

    return matches;
}

std::uint64_t Median(std::vector<std::uint64_t> values) {
    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

std::string JoinRaw(const std::vector<std::uint64_t>& values) {
    std::ostringstream out;

    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0)
            out << ';';
        out << values[i];
    }

    return out.str();
}

bool ShouldRunNaive(const Options& options, const InputData& input) {
    if (options.naive_mode == "on")
        return true;
    if (options.naive_mode == "off")
        return false;

    const std::uint64_t estimated =
        static_cast<std::uint64_t>(input.text.size()) *
        static_cast<std::uint64_t>(input.pattern.size());

    return estimated <= NAIVE_AUTO_LIMIT;
}

void PrintCsvRow(const Options& options, const InputData& input, bool naive_enabled,
                 const std::vector<ZRun>& z_runs,
                 const std::vector<std::uint64_t>& naive_times,
                 std::uint64_t naive_matches) {
    std::vector<std::uint64_t> prepare_times;
    std::vector<std::uint64_t> compute_times;
    std::vector<std::uint64_t> total_times;

    for (const ZRun& run : z_runs) {
        prepare_times.push_back(run.prepare_us);
        compute_times.push_back(run.compute_us);
        total_times.push_back(run.total_us);
    }

    const std::uint64_t prepare_median = Median(prepare_times);
    const std::uint64_t compute_median = Median(compute_times);
    const std::uint64_t total_median = Median(total_times);
    const std::uint64_t z_matches = z_runs.front().matches;

    std::cout << options.case_name << ','
              << options.scenario << ','
              << input.pattern.size() << ','
              << input.text.size() << ','
              << options.inserted_matches << ','
              << options.seed << ','
              << options.repeats << ','
              << '"' << JoinRaw(prepare_times) << '"' << ','
              << '"' << JoinRaw(compute_times) << '"' << ','
              << '"' << JoinRaw(total_times) << '"' << ','
              << prepare_median << ','
              << compute_median << ','
              << total_median << ','
              << z_matches << ','
              << (naive_enabled ? 1 : 0) << ',';

    if (naive_enabled) {
        const std::uint64_t naive_median = Median(naive_times);
        const double ratio =
            total_median == 0 ? 0.0 : static_cast<double>(naive_median) / total_median;

        std::cout << '"' << JoinRaw(naive_times) << '"' << ','
                  << naive_median << ','
                  << naive_matches << ','
                  << std::fixed << std::setprecision(3) << ratio;
    } else {
        std::cout << ",,,";
    }

    std::cout << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const Options options = ParseOptions(argc, argv);
        const InputData input = ReadInput();
        const bool naive_enabled = ShouldRunNaive(options, input);

        std::vector<ZRun> z_runs;
        std::vector<std::uint64_t> naive_times;
        std::uint64_t naive_matches = 0;

        z_runs.reserve(options.repeats);
        naive_times.reserve(options.repeats);

        for (std::size_t run = 0; run < options.repeats; ++run) {
            z_runs.push_back(RunZSearch(input));

            if (run > 0 && z_runs[run].matches != z_runs[0].matches)
                throw std::runtime_error("Z-search match count changed between runs");
        }

        if (naive_enabled) {
            for (std::size_t run = 0; run < options.repeats; ++run) {
                const std::uint64_t start = NowUs();
                const std::uint64_t matches = RunNaive(input);
                const std::uint64_t end = NowUs();

                if (run == 0)
                    naive_matches = matches;
                else if (matches != naive_matches)
                    throw std::runtime_error("naive match count changed between runs");

                naive_times.push_back(end - start);
            }

            if (naive_matches != z_runs.front().matches)
                throw std::runtime_error("Z-search and naive match counts differ");
        }

        PrintCsvRow(options, input, naive_enabled, z_runs, naive_times, naive_matches);
    } catch (const std::exception& error) {
        std::cerr << "benchmark error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
