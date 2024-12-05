#include <algorithm>
#include <fstream>
#include <iterator>
#include <print>
#include <ranges>
#include <vector>

#include "../utils.h"

using namespace std;
using namespace utils;

using Grid = array<array<bool, 100>, 100>;
using Updates = vector<vector<int>>;

pair<Grid, Updates> fromFile(const string &file_name) {
    fstream file{file_name};
    string line;
    Grid grid{};
    Updates updates{};
    bool next_section = false;
    while (getline(file, line)) {
        if (line.empty()) {
            next_section = true;
            continue;
        }
        if (next_section) {
            auto r = line | ranges::views::split(","sv) |
                     ranges::views::transform([](auto &&i) {
                         return std::stoi(i | ranges::to<std::string>());
                     }) |
                     ranges::to<std::vector<int>>();
            updates.emplace_back(r);
        } else {
            auto r = line | ranges::views::split("|"sv) |
                     ranges::views::transform([](auto &&i) {
                         return std::stoi(i | ranges::to<std::string>());
                     }) |
                     ranges::to<std::vector<int>>();
            grid[r[0]][r[1]] = true;
        }
    }
    return {grid, updates};
}

void part1(const Grid &grid, const Updates &updates) {
    int result = 0;
    for (const auto &update : updates) {
        bool correct = true;
        for (int i = 1; i < update.size(); ++i) {
            const auto &row = grid[update[i - 1]];
            if (distance(row.begin(), find(row.cbegin(), row.cend(), true)) ==
                update[i]) {
                correct = false;
                break;
            }
        }
        if (correct) {
            result += update[update.size() / 2];
        }
    }
    println("Part 1: {}", result);
}

int main(int argc, char **argv) {
    const auto &[grid, updates] = fromFile(argv[1]);
    part1(grid, updates);
    return 0;
    // const Matrix &matrix = matrixFromFile(argv[1]);
    // timeit([&]() -> int { return run(matrix, count_xmases_1); });
    // timeit([&]() -> int { return run(matrix, count_xmases_2); });
}
