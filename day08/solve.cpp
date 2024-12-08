#include <fstream>
#include <print>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using point = aoc::point<int>;
using bounds = point;
using point_set = aoc::point_set<int>;

using antenas = unordered_map<char, vector<point>>;

tuple<antenas, bounds> parse_file(const string &file_name) {
    fstream file{file_name};
    string line;
    antenas antenas;
    bounds bounds{0, 0};
    while (getline(file, line)) {
        bounds.x = 0;
        for (char c : line) {
            if (c != '.') antenas[c].emplace_back(bounds);
            ++bounds.x;
        }
        ++bounds.y;
    }
    return {antenas, bounds};
}

bool in_bounds(const point &p, const bounds &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

template <bool resonant_harmonics>
uint64_t run(const antenas &antenas, const bounds &bounds) {
    point_set antinodes;
    for (const auto &locs : antenas | views::values) {
        for (int i = 0; i < locs.size() - 1; ++i) {
            for (int j = i + 1; j < locs.size(); ++j) {
                point diff = locs[j] - locs[i];
                auto f = [&](point p, const point &d) {
                    if constexpr (resonant_harmonics) antinodes.emplace(p);
                    while (true) {
                        p += d;
                        if (!in_bounds(p, bounds)) break;
                        antinodes.emplace(p);
                        if constexpr (!resonant_harmonics) break;
                    }
                };
                f(locs[i], -diff);
                f(locs[j], diff);
            }
        }
    }
    return antinodes.size();
}

int main(int argc, char **argv) {
    const auto &[antenas, bounds] = parse_file(argv[1]);
    timeit([&]() { return run<false>(antenas, bounds); });
    timeit([&]() { return run<true>(antenas, bounds); });
    return 0;
}
