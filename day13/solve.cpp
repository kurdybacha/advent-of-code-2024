#include <math.h>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <print>
#include <queue>
#include <ranges>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using button = aoc::point<uint64_t>;
using prize = aoc::point<uint64_t>;

struct machine {
    button a;
    button b;
    ::prize prize;
};

using machines = vector<machine>;

button button_from_str(const string &line, char delim = '+') {
    auto s = line.find_first_of(delim) + 1;
    auto e = line.find_first_of(',', s);
    uint64_t x = stol(line.substr(s, e));
    uint64_t y = stol(line.substr(e + 4));
    return {x, y};
}

machines parse_file(const string &file_name) {
    ifstream file{file_name};
    machines machines;
    string line;
    while (getline(file, line)) {
        button button_a = button_from_str(line);
        getline(file, line);
        button button_b = button_from_str(line);
        getline(file, line);
        prize prize = button_from_str(line, '=');
        machines.emplace_back(machine{button_a, button_b, prize});
        getline(file, line);  // skip empty line
    }
    return machines;
}

uint64_t find_min_pushes(const machine &machine, uint64_t multiply) {
    prize prize = machine.prize * multiply;
    uint64_t min = numeric_limits<uint64_t>::max();
    uint64_t max_a = ::min(prize.x / machine.a.x, prize.y / machine.a.y);
    uint64_t max_b = ::min(prize.x / machine.b.x, prize.y / machine.b.y);
    for (uint64_t a = 0; a <= max_a; ++a) {
        for (uint64_t b = 0; b <= max_b; ++b) {
            ::prize sum{0, 0};
            sum = machine.a * a;
            sum += machine.b * b;
            if (sum == prize) {
                min = ::min(min, a * 3 + b);
                break;
            }
            if (sum.x > prize.x || sum.y > prize.y) {
                break;
            }
        }
    }
    if (min == numeric_limits<uint64_t>::max()) return 0;
    return min;
}

uint64_t run(const machines &machines, uint64_t multiply = 1) {
    uint64_t result = 0;
    for (const auto &machine : machines) {
        result += find_min_pushes(machine, multiply);
    }
    return result;
}

int main(int argc, char **argv) {
    const auto &machines = parse_file(argv[1]);
    timeit([&]() { return run(machines); });
    // timeit([&]() { return run(machines, 10000000000000); });
    return 0;
}
