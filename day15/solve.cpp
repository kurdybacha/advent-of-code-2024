#include <algorithm>
#include <fstream>
#include <print>
#include <ranges>

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

tuple<grid, point, point> tranform_grid(const grid &grid) {
    ::grid new_grid(grid.size(), string(2 * grid[0].size(), '.'));
    point start{-1, -1};
    for (const auto &[ridx, row] : views::enumerate(grid)) {
        for (const auto &[idx, c] : views::enumerate(row)) {
            if (c == '#' || c == '.') {
                new_grid[ridx][2 * idx] = c;
                new_grid[ridx][2 * idx + 1] = c;
            } else if (c == 'O') {
                new_grid[ridx][2 * idx] = '[';
                new_grid[ridx][2 * idx + 1] = ']';
            } else if (c == '@') {
                start.x = 2 * idx;
                start.y = ridx;
                new_grid[ridx][2 * idx] = '@';
                new_grid[ridx][2 * idx + 1] = '.';
            }
        }
    }
    return {new_grid, point{(int)new_grid[0].size(), (int)new_grid.size()},
            start};
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

uint64_t part1(grid grid, const bounds &bounds, const point start,
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

bool can_push_box(const grid &grid, const point &box_loc, const point &dir,
                  const bounds &bounds) {
    if (dir == point{1, 0}) {
        auto next = box_loc + dir + dir;
        if (!in_bounds(next, bounds)) return false;
        if (grid[next.y][next.x] == '[')
            return can_push_box(grid, next, dir, bounds);
        if (grid[next.y][next.x] == '#') return false;
        return true;
    } else if (dir == point{-1, 0}) {
        auto next = box_loc + dir;
        if (!in_bounds(next, bounds)) return false;
        if (grid[next.y][next.x] == ']')
            return can_push_box(grid, next + dir, dir, bounds);
        if (grid[next.y][next.x] == '#') return false;
        return true;
    } else {
        auto next = box_loc + dir;
        if (!in_bounds(next, bounds)) return false;
        if (grid[next.y][next.x] == '#') return false;
        auto next2 = next + point{1, 0};
        if (!in_bounds(next2, bounds)) return false;
        if (grid[next2.y][next2.x] == '#') return false;
        if (grid[next.y][next.x] == '[')
            return can_push_box(grid, next, dir, bounds);
        bool l = true;
        if (grid[next.y][next.x] == ']')
            l = can_push_box(grid, next + point{-1, 0}, dir, bounds);
        bool r = true;
        if (grid[next2.y][next2.x] == '[')
            r = can_push_box(grid, next2, dir, bounds);
        return l & r;
    }
}

void move(grid &grid, const point &loc, const point &dir) {
    if (dir == point{1, 0}) {
        auto next = loc + dir + dir;
        if (grid[next.y][next.x] != '.') move(grid, next, dir);
        auto loc2 = loc + dir;
        grid[next.y][next.x] = grid[loc2.y][loc2.x];
        grid[loc2.y][loc2.x] = grid[loc.y][loc.x];
        grid[loc.y][loc.x] = '.';
    } else if (dir == point{-1, 0}) {
        auto next = loc + dir;
        if (grid[next.y][next.x] != '.') move(grid, next + dir, dir);
        auto loc2 = loc + point{1, 0};
        grid[next.y][next.x] = grid[loc.y][loc.x];
        grid[loc.y][loc.x] = grid[loc2.y][loc2.x];
        grid[loc2.y][loc2.x] = '.';
    } else {
        auto next = loc + dir;
        auto next2 = next + point{1, 0};
        if (grid[next.y][next.x] == ']') move(grid, next + point{-1, 0}, dir);
        if (grid[next.y][next.x] == '[') move(grid, next, dir);
        if (grid[next2.y][next2.x] == '[') move(grid, next2, dir);
        if (grid[next.y][next.x] == '.' && grid[next2.y][next2.x] == '.') {
            grid[next.y][next.x] = grid[loc.y][loc.x];
            auto loc2 = loc + point{1, 0};
            grid[next2.y][next2.x] = grid[loc2.y][loc2.x];
            grid[loc.y][loc.x] = '.';
            grid[loc2.y][loc2.x] = '.';
        }
    }
}

bool try_to_move2(grid &grid, const point &loc, const point &dir,
                  const bounds &bounds) {
    auto next = loc + dir;
    if (!in_bounds(next, bounds) || grid[next.y][next.x] == '#') return false;
    if (grid[next.y][next.x] == '[' && can_push_box(grid, next, dir, bounds)) {
        move(grid, next, dir);
        grid[loc.y][loc.x] = '.';
        grid[next.y][next.x] = '@';
        return true;
    } else if (grid[next.y][next.x] == ']' &&
               can_push_box(grid, next + point{-1, 0}, dir, bounds)) {
        move(grid, next + point{-1, 0}, dir);
        grid[loc.y][loc.x] = '.';
        grid[next.y][next.x] = '@';
        return true;
    } else if (grid[next.y][next.x] == '.') {
        grid[loc.y][loc.x] = '.';
        grid[next.y][next.x] = '@';
        return true;
    }
    return false;
}

uint64_t part2(grid grid, const bounds &bounds, const point start,
               const moves &moves) {
    point loc = start;
    for (const auto &[idx, move] : views::enumerate(moves)) {
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
        if (try_to_move2(grid, loc, dir, bounds)) loc += dir;
    }

    uint64_t result = 0;
    for (int y = 0; y < bounds.y; ++y) {
        for (int x = 0; x < bounds.x; ++x) {
            if (grid[y][x] == '[') {
                result += y * 100 + x;
            }
        }
    }
    return result;
}

int main(int argc, char **argv) {
    const auto &[grid, bounds, start, moves] = parse_file(argv[1]);
    // timeit([&]() { return part1(grid, bounds, start, moves); });
    const auto &[new_grid, new_bounds, new_start] = tranform_grid(grid);
    timeit([&]() { return part2(new_grid, new_bounds, new_start, moves); });
    return 0;
}
