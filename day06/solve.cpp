#include <fstream>
#include <functional>
#include <print>
#include <vector>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using point = aoc::point<int>;
using point_set = aoc::point_set<int>;

const auto dirs = vector<point>{{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

struct Guard {
    point pos;
    int dir = 0;
    point next_pos() { return pos + dirs[dir]; }
    void turn_right() { dir = (dir + 1) % dirs.size(); }
    void move_forward() { pos = next_pos(); }
};

bool operator==(const Guard &lhs, const Guard &rhs) {
    return lhs.pos == rhs.pos && lhs.dir == rhs.dir;
};
struct hash_Guard {
    std::size_t operator()(const Guard &g) const noexcept {
        std::size_t h1 = std::hash<point>{}(g.pos);
        std::size_t h2 = std::hash<int>{}(g.dir);
        return h1 ^ (h2 << 1);
    }
};

using guard_set = unordered_set<Guard, hash_Guard>;

tuple<point_set, Guard, point> fromFile(const string &file_name) {
    fstream file{file_name};
    string line;
    Guard guard{};
    point_set obstacles;
    int line_no = 0;
    point bounds{-1, -1};
    while (getline(file, line)) {
        if (bounds.x == -1)
            bounds.x = line.size();
        for (int i = 0; i < line.size(); ++i) {
            if (line[i] == '^')
                guard.pos = {i, line_no};
            else if (line[i] == '#')
                obstacles.emplace(point{i, line_no});
        }
        ++line_no;
    }
    bounds.y = line_no;
    return {obstacles, guard, bounds};
}

inline bool in_bounds(const point &p, const point &bounds) {
    return p.x >= 0 && p.x < bounds.x && p.y >= 0 && p.y < bounds.y;
}

bool walk(const point_set &obstacles, Guard guard, const point &bounds,
          std::function<bool(const Guard &p)> visit = {}) {
    while (in_bounds(guard.pos, bounds)) {
        if (visit)
            if (visit(guard)) return true;
        if (obstacles.contains(guard.next_pos())) guard.turn_right();
        else guard.move_forward();
    }
    return false;
}

point_set visited_points(const point_set &obstacles, const Guard &guard,
                         const point &bounds) {
    point_set visited;
    walk(obstacles, guard, bounds, [&visited](const Guard &g) {
        visited.emplace(g.pos);
        return false;
    });
    return visited;
}

int part1(const point_set &obstacles, const Guard &guard, const point &bounds) {
    return visited_points(obstacles, guard, bounds).size();
}

int part2(const point_set &obstacles, const Guard &guard, const point &bounds) {
    auto visited = visited_points(obstacles, guard, bounds);
    visited.erase(guard.pos);
    int result = 0;
    for (const point &p : visited) {
        guard_set gs;
        point_set new_obstacles = obstacles;
        new_obstacles.emplace(p);
        if (walk(new_obstacles, guard, bounds, [&gs](const Guard &g) {
                if (!gs.contains(g)) {
                    gs.emplace(g);
                    return false;
                }
                return true;
            }))
            ++result;
    }
    return result;
}

int main(int argc, char **argv) {
    const auto &[obstacles, guard, bounds] = fromFile(argv[1]);
    timeit([&]() -> int { return part1(obstacles, guard, bounds); });
    timeit([&]() -> int { return part2(obstacles, guard, bounds); });
    return 0;
}
