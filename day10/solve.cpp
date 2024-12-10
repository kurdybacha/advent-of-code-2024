#include <fstream>
#include <print>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using point = aoc::point<int>;
using point_set = aoc::point_set<int>;
using grid = vector<vector<int>>;
using bounds = point;

tuple<grid, bounds, vector<point>> parse_file(const string &file_name) {
    fstream file{file_name};
    string line;
    grid grid;
    vector<point> starts;
    bounds bounds{0, 0};
    while (getline(file, line)) {
        vector<int> row;
        bounds.x = 0;
        for (auto c : line) {
            int v = c - '0';
            if (v == 0) starts.push_back(bounds);
            row.emplace_back(c - '0');
            ++bounds.x;
        }
        grid.emplace_back(row);
        ++bounds.y;
    }
    return {grid, bounds, starts};
}

const vector<point> dirs = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

bool in_bounds(const point &p, const bounds &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

uint64_t dsf(const grid &grid, const bounds &bounds, const point &loc,
             point_set &nines, point_set &visited) {
    int height = grid[loc.y][loc.x];
    if (height == 9) {
        nines.emplace(loc);
        return 1;
    }
    uint64_t result = 0;
    visited.emplace(loc);
    for (const auto &dir : dirs) {
        point next_loc = loc + dir;
        if (!in_bounds(next_loc, bounds)) continue;
        if (visited.contains(next_loc)) continue;
        if (grid[next_loc.y][next_loc.x] == height + 1) {
            result += dsf(grid, bounds, next_loc, nines, visited);
        }
    }
    visited.erase(loc);
    return result;
}

uint64_t run(const grid &grid, const bounds &bounds,
             const vector<point> &starts) {
    uint64_t part1 = 0;
    uint64_t part2 = 0;
    for (const auto &start : starts) {
        point_set visited;
        point_set nines;
        visited.emplace(start);
        part2 += dsf(grid, bounds, start, nines, visited);
        part1 += (int)nines.size();
    }
    println("Part1: {}", part1);
    println("Part2: {}", part2);
    return 0;
}

int main(int argc, char **argv) {
    const auto &[grid, bounds, starts] = parse_file(argv[1]);
    timeit([&]() { return run(grid, bounds, starts); });
    return 0;
}
