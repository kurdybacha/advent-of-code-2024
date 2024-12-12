#include <math.h>

#include <fstream>
#include <print>
#include <queue>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

struct plot {
    char type;
    bool visited = false;
};

using point = aoc::point<int>;
using point_set = aoc::point_set<int>;
using bounds = aoc::point<int>;

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

int count_plots_and_parimeter_bfs(grid &grid, const point &start,
                                  const bounds &bounds) {
    queue<point> q;
    q.emplace(start);
    char type = grid[start.y][start.x].type;
    int no_of_plots = 0;
    int parimeter = 0;
    while (!q.empty()) {
        auto p = q.front();
        q.pop();
        if (grid[p.y][p.x].visited) continue;
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
        }
    }
    return no_of_plots * parimeter;
}

uint64_t run(grid &grid) {
    const bounds bounds{(int)grid[0].size(), (int)grid.size()};
    int cost = 0;
    for (int x = 0; x < grid[0].size(); ++x)
        for (int y = 0; y < grid.size(); ++y)
            if (!grid[y][x].visited)
                cost += count_plots_and_parimeter_bfs(grid, {x, y}, bounds);
    return cost;
}

int main(int argc, char **argv) {
    auto grid = parse_file(argv[1]);
    timeit([&]() { return run(grid); });
    // timeit([&]() { return run(stones, 75); });
    return 0;
}
