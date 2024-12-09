#include <fstream>
#include <list>
#include <print>
#include <ranges>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

struct block {
    int64_t id;
    uint64_t count;
};
using disk = list<block>;

disk parse_file(const string &file_name) {
    fstream file{file_name};
    string line;
    disk disk;
    getline(file, line);
    int64_t block_id = 0;
    for (const auto &[index, c] : views::enumerate(line)) {
        bool free_blocks = index % 2;
        int64_t id = free_blocks ? -1 : block_id;
        uint64_t count = (uint64_t)c - '0';
        if (count > 0) disk.emplace_back(block{id, (uint64_t)c - '0'});
        if (!free_blocks) ++block_id;
    }
    return disk;
}

uint64_t checksum(const auto &disk) {
    uint64_t count = 0;
    uint64_t result = 0;
    for (auto it = disk.begin(); it != disk.end(); ++it) {
        if ((*it).id == -1) {
            count += (*it).count;
            continue;
        }
        uint64_t end = count + (*it).count;
        for (; count < end; ++count) {
            result += count * (*it).id;
        }
    }
    return result;
}

template <bool part2>
uint64_t run(disk disk) {
    auto begin = disk.begin();
    for (auto end = prev(disk.end()); end != begin; --end) {
        if ((*end).id == -1) continue;
        for (; begin != end; ++begin) {
            if ((*begin).id != -1) continue;
            if ((*begin).count >= (*end).count) {
                (*begin).count -= (*end).count;
                disk.insert(begin, *end);
                (*end).id = -1;
                if ((*begin).count == 0) begin = disk.erase(begin);
                break;
            } else {
                if constexpr (!part2) {
                    (*end).count -= (*begin).count;
                    (*begin).id = (*end).id;
                }
            }
        }
        if constexpr (part2) {
            begin = disk.begin();
        }
    }
    return checksum(disk);
}

int main(int argc, char **argv) {
    disk disk = parse_file(argv[1]);
    timeit([&]() { return run<false>(disk); });
    timeit([&]() { return run<true>(disk); });
    return 0;
}
