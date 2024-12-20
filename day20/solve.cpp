#include <math.h>

#include <fstream>
#include <limits>
#include <print>
#include <queue>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using point = aoc::point<int>;
using point_set = aoc::point_set<int>;
using bounds = aoc::point<int>;

const auto INF = numeric_limits<int>::max();

struct node {
    char type = '.';
    int distance = INF;
    bool visited = false;
};

struct state {
    point loc;
    int distance = INF;
};

inline bool operator<(const state &lhs, const state &rhs) {
    return lhs.distance > rhs.distance;
}

using grid = vector<vector<node>>;

tuple<grid, point, point, bounds> parse_file(const string &file_name) {
    ifstream file{file_name};
    grid grid;
    string line;
    vector<node> row;
    point start;
    point end;
    bounds bounds{0, 0};
    while (getline(file, line)) {
        row.clear();
        bounds.x = 0;
        for(char c : line) {
            if (c == 'S')
                start = bounds;
            else if (c == 'E')
                end = bounds;
            row.emplace_back(node{c});
            ++bounds.x;
        }
        grid.emplace_back(row);
        ++bounds.y;
    }
    return {grid, start, end, bounds};
}

bool in_bounds(const point &p, const bounds &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

const auto dirs =
    array<point, 4>{point{0, -1}, point{1, 0}, point{0, 1}, point{-1, 0}};

point_set dijkstra(grid &grid, const point & start, const bounds &bounds) {
    priority_queue<state> queue;
    node &start_node = grid[start.y][start.x];
    start_node.distance = 0;
    queue.emplace(start);
    point_set candidates;
    while (!queue.empty()) {
        auto state = queue.top();
        queue.pop();
        auto loc = state.loc;
        ::node &node = grid[loc.y][loc.x];
        node.visited = true;
        for (const auto &dir : dirs) {
            auto next = loc + dir;
            if (!in_bounds(next, bounds)) continue;
            ::node &next_node = grid[next.y][next.x];
            if (next_node.type == '#') {
                auto next_next = next + dir;
                if (!in_bounds(next_next, bounds)) continue;
                if (grid[next_next.y][next_next.x].type == '.')
                    candidates.emplace(next);
                continue;
            }
            auto alt_dist = node.distance + 1;
            if (alt_dist < next_node.distance) {
                next_node.distance = alt_dist;
                if (!next_node.visited) queue.emplace(::state{next, alt_dist});
            }
        }
    }
    return candidates;
}

int run(::grid grid, const point & start, const point & end, const bounds & bounds) {
    ::grid clean_grid = grid;
    const point_set & candidates = dijkstra(grid, start, bounds);
    int min_dist = grid[end.y][end.x].distance;
    int result = 0;
    for(auto && point : candidates) {
        grid = clean_grid;
        grid[point.y][point.x].type = '.';
        dijkstra(grid, start, bounds);
        int min = grid[end.y][end.x].distance;
        if (min != INF && (min_dist - min) >= 100) {
            ++result;
        }
    }
    return result;
}

// int run2(::grid grid, const vector<point> &points, int time) {
//     const bounds bounds{(int)grid[0].size(), (int)grid.size()};
//     point start = {0, 0};
//     for (int i = 0; i < time; ++i) {
//         const point &p = points[i];
//         node &n = grid[p.y][p.x];
//         n.type = '#';
//     }
//     int begin = time;
//     int end = points.size();
//     while (true) {
//         if ((end - begin) <= 1) return begin;
//         int mid = (end - begin) / 2;
//         ::grid new_grid = grid;
//         for (int i = time; i < begin + mid; ++i) {
//             const point &p = points[i];
//             new_grid[p.y][p.x].type = '#';
//         }
//         dijkstra(new_grid, start, bounds);
//         if (new_grid[bounds.y - 1][bounds.x - 1].distance == INF) {
//             end = begin + mid;
//         } else {
//             begin = begin + mid;
//         }
//     }
//     return INF;
// }

int main(int argc, char **argv) {
    const auto &[grid, start, end, bounds] = parse_file(argv[1]);
    timeit([&]() { return run(grid, start, end, bounds); });
    // timeit([&]() {
    //     auto idx = run2(grid, points, 1024 + 1);
    //     return points[idx];
    // });
    return 0;
}
