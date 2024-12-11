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
    if (stone == 0) {
        auto r = blink_at_stone(1, blink_number + 1, max_blinks, memory);
        memory[sb] = r;
        return r;
    } else if (auto s = to_string(stone); s.size() % 2 == 0) {
        uint64_t n1 = stol(s.substr(0, s.size() / 2));
        uint64_t n2 = stol(s.substr(s.size() / 2));
        auto r = blink_at_stone(n1, blink_number + 1, max_blinks, memory) +
                 blink_at_stone(n2, blink_number + 1, max_blinks, memory);
        memory[sb] = r;
        return r;
    } else {
        auto r =
            blink_at_stone(stone * 2024, blink_number + 1, max_blinks, memory);
        memory[sb] = r;
        return r;
    }
}

uint64_t run(const stones &stones, int max_blinks) {
    uint64_t result = 0;
    memory memory;
    for (auto stone : stones)
        result += blink_at_stone(stone, 0, max_blinks, memory);
    return result;
}

int main(int argc, char **argv) {
    const auto &stones = parse_file(argv[1]);
    timeit([&]() { return run(stones, 25); });
    timeit([&]() { return run(stones, 75); });
    return 0;
}
