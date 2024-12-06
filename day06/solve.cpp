#include <fstream>
#include <print>
#include <vector>

#include "../utils.h"

using namespace std;
using namespace utils;

struct Pos {
    int x;
    int y;
    void operator+=(const Pos &p) {
        x += p.x;
        y += p.y;
    }
};
Pos operator+(const Pos &p1, const Pos &p2) {
    return {p1.x + p2.x, p1.y + p2.y};
}
bool operator==(const Pos &p1, const Pos &p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

struct Node {
    Node(char c) : type{c} {}
    char type;
    bool visited = false;
    Pos last_dir = {0, 0};
    operator char() const { return type; }
};

using Grid = vector<vector<Node>>;

const auto dirs = vector<Pos>{{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

struct Guard {
    Pos pos;
    Pos dir = {0, -1};
    int dir_idx = 0;
    void turn_right() {
        dir_idx = (dir_idx + 1) % dirs.size();
        dir = dirs[dir_idx];
    }
    void move() { pos += dir; }
};

pair<Grid, Guard> fromFile(const string &file_name) {
    fstream file{file_name};
    string line;
    Grid grid{};
    Guard guard{};
    int line_no = 0;
    while (getline(file, line)) {
        vector<Node> nodes;
        for (int i = 0; i < line.size(); ++i) {
            if (line[i] == '^') {
                guard.pos = {i, line_no};
                nodes.emplace_back(Node{'.'});
            } else {
                nodes.emplace_back(Node{line[i]});
            }
        }
        grid.emplace_back(nodes);
        ++line_no;
    }
    return {grid, guard};
}

vector<Pos> walk(Grid grid, Guard guard) {
    vector<Pos> path;
    while (true) {
        if (!grid[guard.pos.y][guard.pos.x].visited) {
            grid[guard.pos.y][guard.pos.x].visited = true;
            path.push_back(guard.pos);
        }
        auto next_pos = guard.pos + guard.dir;
        if (next_pos.x < 0 || next_pos.x >= grid[0].size() || next_pos.y < 0 ||
            next_pos.y >= grid.size())
            break;
        if (grid[next_pos.y][next_pos.x] == '#')
            guard.turn_right();
        else
            guard.move();
    }
    return path;
}

bool check_loop(Grid &grid, Guard guard) {
    if (grid[guard.pos.y][guard.pos.x] == '#') return false;
    while (true) {
        auto next_pos = guard.pos + guard.dir;
        if (next_pos.x < 0 || next_pos.x >= grid[0].size() || next_pos.y < 0 ||
            next_pos.y >= grid.size())
            return false;
        if (grid[next_pos.y][next_pos.x] == '#') {
            guard.turn_right();
            if (grid[guard.pos.y][guard.pos.x].last_dir == guard.dir)
                return true;
        } else {
            grid[guard.pos.y][guard.pos.x].last_dir = guard.dir;
            guard.move();
        }
    }
}

int part1(const Grid &grid, const Guard &guard) {
    return walk(grid, guard).size();
}

int part2(const Grid &grid, const Guard &guard) {
    auto path = walk(grid, guard);
    int result = 0;
    for (Pos p : path) {
        Grid new_grid = grid;
        new_grid[p.y][p.x] = '#';
        if (check_loop(new_grid, guard)) {
            ++result;
        }
    }
    return result;
}

int main(int argc, char **argv) {
    const auto &[grid, guard] = fromFile(argv[1]);
    timeit([&]() -> int { return part1(grid, guard); });
    timeit([&]() -> int { return part2(grid, guard); });
    return 0;
}
