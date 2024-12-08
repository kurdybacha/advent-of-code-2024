#include <fstream>
#include <print>
#include <unordered_map>
#include <vector>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using point = aoc::point<int>;
using point_set = aoc::point_set<int>;

using Antenas = unordered_map<char, vector<point>>;

tuple<Antenas, point> parse_file(const string &file_name) {
    fstream file{file_name};
    string line;
    Antenas antenas;
    int line_no = 0;
    point bounds{0, 0};
    while (getline(file, line)) {
        if (bounds.x == 0) bounds.x = line.size();
        for (int i = 0; i < line.size(); ++i) {
            char antena = line[i];
            if (antena != '.') {
                antenas[antena].emplace_back(point{i, line_no});
            }
        }
        ++line_no;
    }
    bounds.y = line_no;
    return {antenas, bounds};
}

bool in_bounds(const point &p, const point &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

template <bool resonant_harmonics>
uint64_t run(const Antenas &antenas, const point &bounds) {
    point_set antinodes;
    for (const auto &[antena, locs] : antenas) {
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
