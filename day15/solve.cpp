#include <algorithm>
#include <fstream>
#include <print>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using point = aoc::point<int>;
using grid = vector<string>;
using bounds = point;
using moves = string;

tuple<grid, bounds, point, moves> parse_file(const string &file_name) {
    fstream file{file_name};
    string line;
    grid grid;
    point start{-1, -1};
    bounds bounds{0, 0};
    while (getline(file, line) && !line.empty()) {
        vector<string> row;
        bounds.x = 0;
        if (start == point{-1, -1}) {
            auto pos = line.find_first_of('@');
            if (pos != string::npos) {
                start = {bounds.y, (int)pos};
            }
        }
        grid.emplace_back(line);
        ++bounds.y;
    }
    bounds.x = grid[0].size();
    moves moves;
    while (getline(file, line)) {
        moves += line;
    }
    return {grid, bounds, start, moves};
}

const vector<point> dirs = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

void print(const grid &grid) {
    println("******Grid**********");
    for (const auto &row : grid) println("{}", row);
}

bool in_bounds(const point &p, const bounds &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

bool try_to_move(grid &grid, const point &loc, const point &dir,
                 const bounds &bounds) {
    auto next = loc + dir;
    if (!in_bounds(next, bounds) || grid[next.y][next.x] == '#') return false;
    if (grid[next.y][next.x] == 'O') {
        if (try_to_move(grid, next, dir, bounds)) {
            grid[next.y][next.x] = 'O';
            grid[loc.y][loc.x] = '.';
            return true;
        }
        return false;
    } else {
        grid[next.y][next.x] = 'O';
        grid[loc.y][loc.x] = '.';
    }
    return true;
}

uint64_t run(grid grid, const bounds &bounds, const point start,
             const moves &moves) {
    point loc = start;
    for (auto move : moves) {
        int dir_idx = 0;
        switch (move) {
            case '<':
                dir_idx = 0;
                break;
            case '^':
                dir_idx = 1;
                break;
            case '>':
                dir_idx = 2;
                break;
            case 'v':
                dir_idx = 3;
                break;
        }
        point dir = dirs[dir_idx];
        point next_loc = loc + dir;
        if (!in_bounds(next_loc, bounds) || grid[next_loc.y][next_loc.x] == '#')
            continue;
        if (grid[next_loc.y][next_loc.x] == 'O' &&
            try_to_move(grid, next_loc, dir, bounds)) {
            grid[loc.y][loc.x] = '.';
            loc = next_loc;
            grid[loc.y][loc.x] = '@';
        } else if (grid[next_loc.y][next_loc.x] == '.') {
            grid[loc.y][loc.x] = '.';
            loc = next_loc;
            grid[loc.y][loc.x] = '@';
        }
    }

    uint64_t result = 0;
    for (int y = 0; y < bounds.y; ++y) {
        for (int x = 0; x < bounds.x; ++x) {
            if (grid[y][x] == 'O') {
                result += y * 100 + x;
            }
        }
    }
    return result;
}

int main(int argc, char **argv) {
    const auto &[grid, bounds, start, moves] = parse_file(argv[1]);
    timeit([&]() { return run(grid, bounds, start, moves); });
    return 0;
}
