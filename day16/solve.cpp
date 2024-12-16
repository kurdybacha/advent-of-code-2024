#include <math.h>

#include <cstdint>
#include <fstream>
#include <limits>
#include <print>
#include <queue>
#include <ranges>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using point = aoc::point<int>;
using point_set = aoc::point_set<int>;
using bounds = aoc::point<int>;

struct R {
    point loc;
    point dir;
};

struct node {
    char type;
    bool visited = false;
    uint64_t result = numeric_limits<uint64_t>::max();
};

using grid = vector<vector<node>>;

tuple<grid, point> parse_file(const string &file_name) {
    ifstream file{file_name};
    grid grid;
    string line;
    while (getline(file, line)) {
        vector<node> nodes;
        for (char c : line) nodes.emplace_back(node{c});
        grid.emplace_back(nodes);
    }
    return {grid, point{1, (int)grid.size() - 2}};
}

bool in_bounds(const point &p, const bounds &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

const auto dirs =
    array<point, 4>{point{0, -1}, point{1, 0}, point{0, 1}, point{-1, 0}};

uint64_t dirs_to_score(const point &curr_dir, const point &move_dir) {
    if (curr_dir == move_dir) return 1;
    return 1000 + 1;
}

void print(grid grid, point loc) {
    grid[loc.y][loc.x].type = 'S';
    println("***Grid***");
    for (auto &&row : grid) {
        for (auto &&n : row) println("{}", n.type);
        println();
    }
}

void dsf(grid &grid, R &r, const bounds &bounds) {
    for (const auto &dir : dirs) {
        auto next = r.loc + dir;
        if (!in_bounds(next, bounds)) continue;
        if (grid[next.y][next.x].type == '#') continue;
        if (grid[next.y][next.x].type == '.') {
            auto score = dirs_to_score(r.dir, dir);
            auto next_result = grid[r.loc.y][r.loc.x].result + score;
            if (next_result > grid[next.y][next.x].result) continue;
            grid[next.y][next.x].result = next_result;
            auto r_backup = r;
            r.loc = next;
            r.dir = dir;
            dsf(grid, r, bounds);
            r = r_backup;
        } else if (grid[next.y][next.x].type == 'E') {
            auto score = dirs_to_score(r.dir, dir);
            auto next_result = grid[r.loc.y][r.loc.x].result + score;
            if (next_result > grid[next.y][next.x].result) continue;
            grid[next.y][next.x].result = next_result;
        }
    }
}

uint64_t run(grid grid, R r) {
    const bounds bounds{(int)grid[0].size(), (int)grid.size()};
    grid[r.loc.y][r.loc.x].result = 0;
    dsf(grid, r, bounds);
    return grid[1][bounds.x - 2].result;
}

int main(int argc, char **argv) {
    const auto &[grid, start] = parse_file(argv[1]);
    R r{start, {1, 0}};
    timeit([&]() { return run(grid, r); });
    return 0;
}
