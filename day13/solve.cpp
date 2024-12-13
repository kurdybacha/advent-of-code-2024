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

using button = aoc::point<int64_t>;
using prize = aoc::point<int64_t>;

struct machine {
    button a;
    button b;
    ::prize prize;
};

template <>
struct std::formatter<machine> : formatter<std::string> {
    auto format(const machine &m, format_context &ctx) const {
        return format_to(ctx.out(), "[ a: {}, b: {}, prize: {} ]", m.a, m.b,
                         m.prize);
    }
};

using machines = vector<machine>;

button button_from_str(const string &line, char delim = '+') {
    auto s = line.find_first_of(delim) + 1;
    auto e = line.find_first_of(',', s);
    int64_t x = stol(line.substr(s, e));
    int64_t y = stol(line.substr(e + 4));
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

int64_t find_min_pushes(const machine &machine, int64_t add) {
    const prize prize = machine.prize + add;
    const auto &a = machine.a;
    const auto &b = machine.b;
    double a_p =
        double(prize.x * b.y - prize.y * b.x) / (b.y * a.x - b.x * a.y);
    double b_p =
        double(prize.x * a.y - prize.y * a.x) / (b.x * a.y - b.y * a.x);
    if (floor(a_p) != a_p || floor(b_p) != b_p) return 0;
    return a_p * 3 + b_p;
}

int64_t run(const machines &machines, int64_t add = 0) {
    return ranges::fold_left(
        machines | views::transform([add](const machine &machine) {
            return find_min_pushes(machine, add);
        }),
        0, plus<>());
}

int main(int argc, char **argv) {
    const auto &machines = parse_file(argv[1]);
    timeit([&]() { return run(machines); });
    timeit([&]() { return run(machines, 10000000000000); });
    return 0;
}
