#include <fstream>
#include <print>
#include <ranges>
#include <unordered_map>

#include "../utils.h"

using namespace std;
using namespace utils;

using towels = vector<string>;
using towel_map = unordered_map<char, vector<string_view>>;
using patterns = vector<string>;

tuple<towels, patterns> parse_file(const string &file_name) {
    ifstream file{file_name};
    string line;
    getline(file, line);
    towels towels =
        line | views::split(views::all(", "sv)) | ranges::to<vector<string>>();
    getline(file, line);  // skip empty line
    patterns patterns;
    while (getline(file, line)) {
        patterns.emplace_back(line);
    }
    return {towels, patterns};
}

bool check_pattern(const string &pattern, const towel_map &towel_map,
                   size_t pos) {
    if (pos == pattern.size()) return true;
    if (!towel_map.contains(pattern[pos])) return false;
    bool found = false;
    const auto &towels = towel_map.at(pattern[pos]);
    for (const auto &towel : towels) {
        if (pattern.substr(pos, towel.size()) == towel) {
            if (check_pattern(pattern, towel_map, pos + towel.size())) {
                found = true;
                break;
            }
        }
    }
    return found;
}

int run(towels towels, const patterns &patterns) {
    int result = 0;
    towel_map towel_map;
    for (auto &&towel : towels) towel_map[towel[0]].emplace_back(towel);
    for (auto &&pattern : patterns) {
        if (check_pattern(pattern, towel_map, 0)) ++result;
    }
    return result;
}

uint64_t combinations(const towels &towels, const string_view &pattern) {
    vector<uint64_t> counters(pattern.size() + 1, 0);
    counters[0] = 1;
    for (int i = 1; i <= pattern.size(); ++i) {
        for (auto &&towel : towels) {
            if (i >= towel.size() &&
                pattern.substr(i - towel.size(), towel.size()) == towel)
                counters[i] += counters[i - towel.size()];
        }
    }
    return counters[pattern.size()];
}

uint64_t run2(const towels &towels, const patterns &patterns) {
    uint64_t result = 0;
    for (auto &&pattern : patterns) {
        result += combinations(towels, pattern);
    }
    return result;
}

int main(int argc, char **argv) {
    const auto &[towels, patterns] = parse_file(argv[1]);
    timeit([&]() { return run(towels, patterns); });
    timeit([&]() { return run2(towels, patterns); });
    return 0;
}
