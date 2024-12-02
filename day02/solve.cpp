#include <cstdint>
#include <fstream>
#include <print>
#include <sstream>
#include <vector>

using namespace std;

using Level = vector<int64_t>;
using Levels = vector<Level>;

Levels parse(const string &file_name) {
    fstream file{file_name};
    string line;
    Levels levels;
    uint64_t n{};
    while (getline(file, line)) {
        istringstream ss(line);
        Level level;
        while (ss >> n) {
            level.emplace_back(n);
        }
        levels.emplace_back(level);
    }
    return levels;
}

bool is_level_safe(const Level &level) {
    bool increase = (level[1] - level[0]) > 0;
    for (int i = 1; i < level.size(); ++i) {
        auto diff = level[i] - level[i - 1];
        if (diff == 0) return false;
        if ((diff > 0) != increase) return false;
        if (abs(diff) < 1 || abs(diff) > 3) return false;
    }
    return true;
}

void part1(const Levels &levels) {
    uint64_t safe_levels = 0;
    for (const auto &level : levels)
        if (is_level_safe(level)) ++safe_levels;
    println("Safe levels: {}", safe_levels);
}

void part2(const Levels &levels) {
    uint64_t safe_levels = 0;
    for (const auto &l : levels) {
        for (int skip = -1; skip < (int)l.size(); ++skip) {
            Level level = l;
            if (skip != -1) level.erase(level.begin() + skip);
            if (is_level_safe(level)) {
                ++safe_levels;
                break;
            }
        }
    }
    println("Safe levels: {}", safe_levels);
}

int main(int argc, char **argv) {
    Levels levels = parse(argv[1]);
    part1(levels);
    part2(levels);
    return 0;
}
