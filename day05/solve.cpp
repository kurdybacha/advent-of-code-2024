#include <array>
#include <cassert>
#include <fstream>
#include <print>
#include <ranges>
#include <vector>

#include "../utils.h"

using namespace std;
using namespace utils;

using Grid = array<array<bool, 100>, 100>;
using Updates = vector<vector<int>>;

pair<Grid, Updates> fromFile(const string &file_name) {
    ifstream file{file_name};
    string line;
    Grid grid{};
    Updates updates{};
    bool next_section = false;
    while (getline(file, line)) {
        if (line.empty()) {
            next_section = true;
            continue;
        }
        auto ints = line | views::split(next_section ? ',' : '|') |
                    views::transform([](auto v) {
                        return std::stoi(std::string(v.begin(), v.end()));
                    }) |
                    ranges::to<vector<int>>();
        if (next_section)
            updates.emplace_back(ints);
        else
            grid[ints[0]][ints[1]] = true;
    }
    return {grid, updates};
}

int part1(const Grid &grid, const Updates &updates) {
    int result = 0;
    for (const auto &update : updates) {
        bool correct = true;
        for (int i = 0; i < update.size() - 1; ++i) {
            if (!grid[update[i]][update[i + 1]]) {
                correct = false;
                break;
            }
        }
        if (correct) {
            result += update[update.size() / 2];
        }
    }
    return result;
}

int part2(const Grid &grid, const Updates &updates) {
    int result = 0;
    for (const auto &update : updates) {
        bool correct = true;
        for (int i = 0; i < update.size() - 1; ++i) {
            if (!grid[update[i]][update[i + 1]]) {
                correct = false;
                break;
            }
        }
        if (!correct) {
            vector<int> new_update;
            new_update.push_back(update[0]);
            for (int i = 1; i < update.size(); ++i) {
                bool inserted = false;
                for (int j = 0; j < new_update.size(); ++j) {
                    if (grid[update[i]][new_update[j]]) {
                        new_update.insert(new_update.begin() + j, update[i]);
                        inserted = true;
                        break;
                    }
                }
                if (!inserted) new_update.emplace_back(update[i]);
            }
            result += new_update[new_update.size() / 2];
        }
    }
    return result;
}

int main(int argc, char **argv) {
    const auto &[grid, updates] = fromFile(argv[1]);
    timeit([&]() { return part1(grid, updates); });
    timeit([&]() { return part2(grid, updates); });
    return 0;
}
