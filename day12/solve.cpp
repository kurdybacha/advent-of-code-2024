#include <math.h>

#include <fstream>
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

struct plot {
    char type;
    bool visited = false;
    point_set visited_dirs;
};

using grid = vector<vector<plot>>;

grid parse_file(const string &file_name) {
    ifstream file{file_name};
    grid grid;
    string line;
    while (getline(file, line)) {
        vector<plot> r;
        for (char c : line) r.emplace_back(plot{c});
        grid.emplace_back(r);
    }
    return grid;
}

bool in_bounds(const point &p, const bounds &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

const auto dirs = vector<point>{{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

point rotate_left(int dir_idx) { return dirs[(dir_idx - 1) % dirs.size()]; }
point rotate_right(int dir_idx) { return dirs[(dir_idx + 1) % dirs.size()]; }

void mark_all_along_side(grid &grid, const point &p, int dir_idx,
                         const bounds &bounds) {
    char type = grid[p.y][p.x].type;
    for (auto d : {rotate_right(dir_idx), rotate_left(dir_idx)}) {
        auto nbr = p + d;
        while (true) {
            if (!::in_bounds(nbr, bounds)) break;
            if (grid[nbr.y][nbr.x].visited_dirs.contains(dirs[dir_idx])) break;
            if (grid[nbr.y][nbr.x].type != type) break;
            auto nbr_dir = nbr + dirs[dir_idx];
            if (!::in_bounds(nbr_dir, bounds) ||
                grid[nbr_dir.y][nbr_dir.x].type != type) {
                grid[nbr.y][nbr.x].visited_dirs.emplace(dirs[dir_idx]);
                nbr = nbr + d;
            } else {
                break;
            }
        }
    }
}

tuple<int, int> count_plots_and_parimeter_bfs(grid &grid, const point &start,
                                              const bounds &bounds) {
    queue<point> q;
    q.emplace(start);
    char type = grid[start.y][start.x].type;
    int no_of_plots = 0;
    int parimeter = 0;
    int sides = 0;
    point_set visited;
    while (!q.empty()) {
        auto p = q.front();
        q.pop();
        if (grid[p.y][p.x].visited) continue;
        visited.emplace(p);
        ++no_of_plots;
        grid[p.y][p.x].visited = true;
        for (auto dir : dirs) {
            auto next = p + dir;
            const auto in_bounds = ::in_bounds(next, bounds);
            if (in_bounds && grid[next.y][next.x].type == type &&
                !grid[next.y][next.x].visited)
                q.emplace(next);
            if (!in_bounds || grid[next.y][next.x].type != type) {
                ++parimeter;
            }
            for (auto &&[dir_idx, dir] : views::enumerate(dirs)) {
                auto next = p + dir;
                const auto in_bounds = ::in_bounds(next, bounds);
                if (!grid[p.y][p.x].visited_dirs.contains(dir) &&
                    (!in_bounds || grid[next.y][next.x].type != type)) {
                    mark_all_along_side(grid, p, dir_idx, bounds);
                    ++sides;
                }
                grid[p.y][p.x].visited_dirs.emplace(dir);
            }
        }
    }
    return {no_of_plots * parimeter, no_of_plots * sides};
}

void run(grid grid) {
    const bounds bounds{(int)grid[0].size(), (int)grid.size()};
    int part1 = 0;
    int part2 = 0;
    for (int x = 0; x < grid[0].size(); ++x)
        for (int y = 0; y < grid.size(); ++y)
            if (!grid[y][x].visited) {
                auto [p1, p2] =
                    count_plots_and_parimeter_bfs(grid, {x, y}, bounds);
                part1 += p1;
                part2 += p2;
            }
    println("Part 1: {}", part1);
    println("Part 2: {}", part2);
}

int main(int argc, char **argv) {
    auto grid = parse_file(argv[1]);
    timeit([&]() {
        run(grid);
        return 0;
    });
    return 0;
}
