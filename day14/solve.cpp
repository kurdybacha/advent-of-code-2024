#include <math.h>

#include <algorithm>
#include <cstdint>
#include <format>
#include <fstream>
#include <print>
#include <ranges>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using point = aoc::point<int>;
using point_set = aoc::point_set<int>;
using velocity = aoc::point<int>;

struct robot {
    point p;
    velocity v;
};

using robots = vector<robot>;
const point bounds{101, 103};
using grid = vector<vector<int>>;

template <>
struct std::formatter<robot> : formatter<std::string> {
    auto format(const robot &r, format_context &ctx) const {
        return format_to(ctx.out(), "[ p: {}, v: {} ]", r.p, r.v);
    }
};

point str_to_point(const string &s) {
    auto p = s.find_first_of(',');
    return {stoi(s.substr(0, p)), stoi(s.substr(p + 1))};
}

robots parse_file(const string &file_name) {
    ifstream file{file_name};
    robots robots;
    string line;
    while (getline(file, line)) {
        auto p = line.find_first_of(' ');
        string loc_str = line.substr(2, p);
        string vel_str = line.substr(p + 3);
        robots.emplace_back(
            robot{str_to_point(loc_str), str_to_point(vel_str)});
    }
    return robots;
}

int part1(robots robots, int count = 100) {
    int q1 = 0;
    int q2 = 0;
    int q3 = 0;
    int q4 = 0;
    for (auto &&robot : robots) {
        auto x = robot.p.x + robot.v.x * count;
        robot.p.x = (bounds.x + (x % bounds.x)) % bounds.x;
        auto y = robot.p.y + robot.v.y * count;
        robot.p.y = (bounds.y + (y % bounds.y)) % bounds.y;
        if (robot.p.x < bounds.x / 2 && robot.p.y < bounds.y / 2)
            ++q1;
        else if (robot.p.x > bounds.x / 2 && robot.p.y < bounds.y / 2)
            ++q2;
        else if (robot.p.x < bounds.x / 2 && robot.p.y > bounds.y / 2)
            ++q3;
        else if (robot.p.x > bounds.x / 2 && robot.p.y > bounds.y / 2)
            ++q4;
    }
    return q1 * q2 * q3 * q4;
}

bool in_bounds(const point &p, const point &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

bool is_christmas_tree_tip(const grid &grid, const point &loc) {
    for (int y = 1; y < 8; ++y) {
        for (int x = -(2 * y + 1) / 2; x <= (2 * y + 1) / 2; ++x) {
            if (!in_bounds({loc.y + y, loc.x + x}, bounds)) return false;
            if (grid[loc.y + y][loc.x + x] == 0) return false;
        }
    }

    return true;
}

void print(const grid &grid) {
    println("*****Tree*****");
    for (auto &&r : grid) {
        for (auto c : r) print("{}", c);
        println();
    }
}

int part2(robots robots) {
    grid grid(bounds.y, vector<int>(bounds.x, 0));
    for (auto &&robot : robots) grid[robot.p.y][robot.p.x] += 1;
    for (int i = 1;; ++i) {
        for (auto &&robot : robots) {
            grid[robot.p.y][robot.p.x] -= 1;
            auto x = robot.p.x + robot.v.x;
            robot.p.x = (bounds.x + (x % bounds.x)) % bounds.x;
            auto y = robot.p.y + robot.v.y;
            robot.p.y = (bounds.y + (y % bounds.y)) % bounds.y;
            grid[robot.p.y][robot.p.x] += 1;
            if (is_christmas_tree_tip(grid, robot.p)) {
                print(grid);
                return i;
            }
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    const auto &robots = parse_file(argv[1]);
    timeit([&]() { return part1(robots); });
    timeit([&]() { return part2(robots); });
    return 0;
}
