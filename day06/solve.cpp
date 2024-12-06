#include <fstream>
#include <print>
#include <vector>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using point = aoc::point<int>;
using point_set = aoc::point_set<int>;

struct node {
    node(char c) : type{c} {}
    char type;
    int last_dir = -1;
    operator char() const { return type; }
};

using Grid = vector<vector<node>>;

const auto dirs = vector<point>{{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

struct Guard {
    point pos;
    int dir_idx = 0;
    point next_pos() { return pos + dirs[dir_idx]; }
    void turn_right() { dir_idx = (dir_idx + 1) % dirs.size(); }
    void move_forward() { pos = next_pos(); }
};

pair<Grid, Guard> fromFile(const string &file_name) {
    fstream file{file_name};
    string line;
    Grid grid{};
    Guard guard{};
    int line_no = 0;
    while (getline(file, line)) {
        vector<node> nodes;
        for (int i = 0; i < line.size(); ++i) {
            nodes.emplace_back(node{line[i]});
            if (line[i] == '^') guard.pos = {i, line_no};
        }
        grid.emplace_back(nodes);
        ++line_no;
    }
    return {grid, guard};
}

point_set walk(Grid grid, Guard guard) {
    point_set path;
    while (true) {
        path.emplace(guard.pos);
        const point &next_pos = guard.next_pos();
        if (next_pos.x < 0 || next_pos.x >= grid[0].size() || next_pos.y < 0 ||
            next_pos.y >= grid.size())
            break;
        if (grid[next_pos.y][next_pos.x] == '#')
            guard.turn_right();
        else
            guard.move_forward();
    }
    return path;
}

bool check_loop(Grid &grid, Guard guard) {
    if (grid[guard.pos.y][guard.pos.x] == '#') return false;
    while (grid[guard.pos.y][guard.pos.x].last_dir != guard.dir_idx) {
        const point &next_pos = guard.next_pos();
        if (next_pos.x < 0 || next_pos.x >= grid[0].size() || next_pos.y < 0 ||
            next_pos.y >= grid.size())
            return false;
        if (grid[next_pos.y][next_pos.x] == '#') {
            guard.turn_right();
        } else {
            grid[guard.pos.y][guard.pos.x].last_dir = guard.dir_idx;
            guard.move_forward();
        }
    }
    return grid[guard.pos.y][guard.pos.x].last_dir == guard.dir_idx;
}

int part1(const Grid &grid, const Guard &guard) {
    return walk(grid, guard).size();
}

int part2(const Grid &grid, const Guard &guard) {
    auto path = walk(grid, guard);
    int result = 0;
    for (const point &p : path) {
        Grid g = grid;
        g[p.y][p.x] = '#';
        if (check_loop(g, guard)) ++result;
    }
    return result;
}

int main(int argc, char **argv) {
    const auto &[grid, guard] = fromFile(argv[1]);
    timeit([&]() -> int { return part1(grid, guard); });
    timeit([&]() -> int { return part2(grid, guard); });
    return 0;
}
