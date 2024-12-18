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

const auto MAX = numeric_limits<int>::max();

struct node {
    char type = '.';
    int distance = MAX;
};

struct state {
    point loc;
    int distance = MAX;
};

inline bool operator<(const state &lhs, const state &rhs) {
    return lhs.distance < rhs.distance;
}

using grid = vector<vector<node>>;

tuple<grid, vector<point>> parse_file(const string &file_name,
                                      size_t size = 71) {
    ifstream file{file_name};
    grid grid;
    string line;
    vector<point> points;
    while (getline(file, line)) {
        auto it = line.find_first_of(",");
        points.emplace_back(
            point{stoi(line.substr(0, it)), stoi(line.substr(it + 1))});
    }
    auto v = vector<vector<node>>(size, vector<node>(size));
    return {v, points};
}

bool in_bounds(const point &p, const bounds &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

const auto dirs =
    array<point, 4>{point{0, -1}, point{1, 0}, point{0, 1}, point{-1, 0}};

void dijkstra(grid &grid, point start, const bounds &bounds) {
    priority_queue<state> queue;
    node &start_node = grid[start.y][start.x];
    start_node.distance = 0;
    queue.emplace(start);
    while (!queue.empty()) {
        auto state = queue.top();
        queue.pop();
        auto loc = state.loc;
        ::node &node = grid[loc.y][loc.x];
        for (const auto &dir : dirs) {
            auto next = loc + dir;
            if (!in_bounds(next, bounds)) continue;
            ::node &next_node = grid[next.y][next.x];
            if (next_node.type == '#') continue;
            auto alt_dist = node.distance + 1;
            if (alt_dist < next_node.distance) {
                next_node.distance = alt_dist;
                queue.emplace(::state{next, alt_dist});
            }
        }
    }
}

int run(::grid grid, const vector<point> &points, int time) {
    const bounds bounds{(int)grid[0].size(), (int)grid.size()};
    point start = {0, 0};
    for (int i = 0; i < time; ++i) {
        const point &p = points[i];
        node &n = grid[p.y][p.x];
        n.type = '#';
    }
    dijkstra(grid, start, bounds);
    return grid[bounds.y - 1][bounds.x - 1].distance;
}

int main(int argc, char **argv) {
    const auto &[grid, points] = parse_file(argv[1], 71);
    timeit([&]() { return run(grid, points, 1024); });
    return 0;
}
