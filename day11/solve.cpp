#include <math.h>

#include <algorithm>
#include <fstream>
#include <print>
#include <ranges>
#include <unordered_map>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using stone = uint64_t;
using stones = vector<uint64_t>;
using memory = unordered_map<pair<stone, uint64_t>, uint64_t, pair_hash>;

stones parse_file(const string &file_name) {
    ifstream file{file_name};
    string line;
    getline(file, line);
    istringstream ss{line};
    return views::istream<uint64_t>(ss) | ranges::to<stones>();
}

uint64_t blink_at_stone(const stone stone, int blink_number, int max_blinks,
                        memory &memory) {
    if (blink_number == max_blinks) return 1;
    pair<::stone, uint64_t> sb{stone, blink_number};
    if (memory.contains(sb)) return memory.at(sb);
    uint64_t r{};
    if (stone == 0) {
        r = blink_at_stone(1, blink_number + 1, max_blinks, memory);
    } else {
        unsigned digits = aoc::digit_count(stone);
        if (digits % 2 == 0) {
            uint64_t denom = pow(10, double(digits) / 2);
            r = blink_at_stone(stone / denom, blink_number + 1, max_blinks,
                               memory) +
                blink_at_stone(stone % denom, blink_number + 1, max_blinks,
                               memory);
        } else {
            r = blink_at_stone(stone * 2024, blink_number + 1, max_blinks,
                               memory);
        }
    }
    memory[sb] = r;
    return r;
}

uint64_t run(const stones &stones, int max_blinks) {
    memory memory;
    return ranges::fold_left(stones | views::transform([&](const stone &stone) {
                                 return blink_at_stone(stone, 0, max_blinks,
                                                       memory);
                             }),
                             0, plus<>());
}

int main(int argc, char **argv) {
    const auto &stones = parse_file(argv[1]);
    timeit([&]() { return run(stones, 25); });
    timeit([&]() { return run(stones, 75); });
    return 0;
}
