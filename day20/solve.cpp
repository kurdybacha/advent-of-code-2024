#include <algorithm>
#include <fstream>
#include <limits>
#include <print>
#include <queue>
#include <ranges>
#include <unordered_map>

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

template <>
struct std::formatter<grid> : formatter<std::string> {
    auto format(const grid &g, format_context &ctx) const {
        string str;
        for (auto &&r : g) {
            for (auto &&n : r) {
                str += n.type;
            }
            str += '\n';
        }
        return format_to(ctx.out(), "{}", str);
    }
};

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
        for (char c : line) {
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

point_set dijkstra_shortest_path(grid &grid, const point &start,
                                 const point &end, const bounds &bounds) {
    unordered_map<point, point> prev;
    priority_queue<state> queue;
    node &start_node = grid[start.y][start.x];
    start_node.distance = 0;
    queue.emplace(start);
    prev[start] = {0, 0};
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
            if (next_node.type == '#') continue;
            auto alt_dist = node.distance + 1;
            if (alt_dist < next_node.distance) {
                next_node.distance = alt_dist;
                prev[next] = loc;
                if (!next_node.visited) queue.emplace(::state{next, alt_dist});
            }
        }
    }

    point_set shortest_path;
    auto u = end;
    while (prev.contains(u)) {
        shortest_path.insert(u);
        u = prev[u];
    }
    return shortest_path;
}

int check_shortcuts(const grid &grid, const point &loc,
                    const point_set &shortest_path, const bounds &bounds,
                    int cheat_size, int min_saving) {
    point_set visited;
    queue<pair<point, int>> queue;
    queue.push({loc, cheat_size});
    int shortcuts = 0;
    auto &&start_node = grid[loc.y][loc.x];
    while (!queue.empty()) {
        auto p = queue.front();
        queue.pop();
        const auto &[ploc, pdistance] = p;
        if (visited.contains(ploc)) continue;
        visited.insert(ploc);
        if (shortest_path.contains(ploc) &&
            grid[ploc.y][ploc.x].distance > start_node.distance) {
            int diff = grid[ploc.y][ploc.x].distance -
                       (start_node.distance + (cheat_size - pdistance));
            if (diff >= min_saving) ++shortcuts;
        }
        if (pdistance == 0) continue;
        for (const auto &dir : dirs) {
            auto next = ploc + dir;
            if (!in_bounds(next, bounds)) continue;
            if (visited.contains(next)) continue;
            queue.push({next, pdistance - 1});
        }
    }
    return shortcuts;
}

int run(::grid grid, const point &start, const point &end, const bounds &bounds,
        int cheat_size) {
    const point_set &shortest_path =
        dijkstra_shortest_path(grid, start, end, bounds);
    return ranges::fold_left(
        shortest_path | views::transform([&](const point &loc) {
            return check_shortcuts(grid, loc, shortest_path, bounds, cheat_size,
                                   100);
        }),
        0, plus<>());
}

int main(int argc, char **argv) {
    const auto &[grid, start, end, bounds] = parse_file(argv[1]);
    timeit([&]() { return run(grid, start, end, bounds, 2); });
    timeit([&]() { return run(grid, start, end, bounds, 20); });
    return 0;
}
