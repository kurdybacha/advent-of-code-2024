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
    point dir;
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

char dir_to_arrow(const point & dir) {
    if (dir == point{0, -1})
        return '^';
    if (dir == point{1, 0})
        return '>';
    if (dir == point{0, 1})
        return 'v';
    if (dir == point{-1, 0})
        return '<';
    return 'O';
}

void print(grid grid, point loc) {
    grid[loc.y][loc.x].type = 'S';
    println("***Grid***");
    for (auto &&row : grid) {
        for (auto &&n : row) {
            print("{}", n.visited ? 'O' : n.dir != point{0, 0} ? dir_to_arrow(n.dir) : n.type );
        }
        println();
    }
}

void fix_path(grid & grid, const bounds & bounds) {
    R r{{1, bounds.y-2}, {1, 0}};
    for(auto && r: grid)
        for(auto && n : r)
            n.dir = point{0, 0};
    grid[r.loc.y][r.loc.x].dir = r.dir;
    uint64_t result = 0;
    while(true) {
        for (const auto &dir : dirs) {
            if (r.dir == dir*-1) continue;
            auto next = r.loc + dir;
            if (!in_bounds(next, bounds)) continue;
            if (grid[next.y][next.x].type == '#') continue;
            if (grid[next.y][next.x].visited) {
                if (r.dir != dir) {
                    result += 1000;
                    grid[r.loc.y][r.loc.x].result = result;
                }
                result += 1;
                grid[next.y][next.x].result = result;
                grid[next.y][next.x].dir = dir;
                r.loc = next;
                r.dir = dir;
                if (grid[next.y][next.x].type == 'E') return;
                break;
            }
        }
    }
}

void dsf(grid &grid, R &r, const bounds &bounds, point_set &best_path) {
    grid[r.loc.y][r.loc.x].visited = true;
    for (const auto &dir : dirs) {
        if (r.dir == dir*-1) continue;
        auto next = r.loc + dir;
        if (!in_bounds(next, bounds)) continue;
        if (grid[next.y][next.x].type == '#') continue;
        if (grid[next.y][next.x].type == '.') {
            auto score = dirs_to_score(r.dir, dir);
            auto next_result = grid[r.loc.y][r.loc.x].result + score;
            if (next_result >= grid[next.y][next.x].result) continue;
            grid[next.y][next.x].result = next_result;
            auto r_backup = r;
            r.loc = next;
            r.dir = dir;
            dsf(grid, r, bounds, best_path);
            r = r_backup;
        } else if (grid[next.y][next.x].type == 'E') {
            grid[next.y][next.x].visited = true;
            auto score = dirs_to_score(r.dir, dir);
            auto next_result = grid[r.loc.y][r.loc.x].result + score;
            if (next_result >= grid[next.y][next.x].result) continue;
            grid[next.y][next.x].result = next_result;
            best_path = point_set();
            for (int y = 0; y < bounds.y; ++y)
                for (int x = 0; x < bounds.x; ++x)
                    if (grid[y][x].visited) best_path.emplace(point{x, y});
            grid[next.y][next.x].visited = false;
        }
    }
    grid[r.loc.y][r.loc.x].visited = false;
}

void dsf2(grid &grid, R &r, const bounds &bounds, const point_set & best_path, point_set &visited, uint64_t &result) {
    grid[r.loc.y][r.loc.x].visited = true;
    // print(grid, r.loc);
    for (const auto &dir : dirs) {
        auto next = r.loc + dir;
        if (!in_bounds(next, bounds)) continue;
        if (grid[next.y][next.x].type == '#') continue;
        if (grid[next.y][next.x].visited) continue;
        if (grid[next.y][next.x].type == '.') {
            auto score = dirs_to_score(r.dir, dir);
            auto next_result = result + score;
            if (next_result <= grid[next.y][next.x].result && best_path.contains(next)) {
                for (int y = 0; y < bounds.y; ++y)
                    for (int x = 0; x < bounds.x; ++x)
                        if (grid[y][x].visited) visited.emplace(point{x, y});
            }
            if ((next_result > grid[next.y][next.x].result)) {
                if (!best_path.contains(next))
                    continue;
            }
            result += score;
            auto r_backup = r;
            r.loc = next;
            r.dir = dir;
            dsf2(grid, r, bounds, best_path, visited, result);
            r = r_backup;
            result -= score;
        } else if (grid[next.y][next.x].type == 'E') {
            auto score = dirs_to_score(r.dir, dir);
            auto next_result = result + score;
            if (next_result > grid[next.y][next.x].result) continue;
            if (next_result <= grid[next.y][next.x].result && best_path.contains(next)) {
                visited.emplace(next);
                for (int y = 0; y < bounds.y; ++y)
                    for (int x = 0; x < bounds.x; ++x)
                        if (grid[y][x].visited) visited.emplace(point{x, y});
            }
            grid[next.y][next.x].result = next_result;
        }
    }
    grid[r.loc.y][r.loc.x].visited = false;
}

uint64_t run(::grid grid, R r) {
    const bounds bounds{(int)grid[0].size(), (int)grid.size()};
    grid[r.loc.y][r.loc.x].result = 0;
    ::R r2 = r;
    point_set best_path;
    dsf(grid, r, bounds, best_path);
    point_set visited;
    uint64_t result = 0;
    dsf2(grid, r2, bounds, best_path, visited, result);
    // print(grid, r.loc);
    for(auto && n : best_path)
        visited.emplace(n);
    println("Part 1: {}", grid[1][bounds.x - 2].result);
    println("Part 2: {}", visited.size());
    // for (auto && p : best_path)
    //     c[p.y][p.x].type = 'x';
    // print(c, r.loc);
    return grid[1][bounds.x - 2].result;
}

int main(int argc, char **argv) {
    const auto &[grid, start] = parse_file(argv[1]);
    R r{start, {1, 0}};
    timeit([&]() { return run(grid, r); });
    return 0;
}
