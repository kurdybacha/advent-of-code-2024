#include <fstream>
#include <print>
#include <vector>

#include "../util/point.h"
#include "../utils.h"

using namespace std;
using namespace utils;

using Calibration = pair<uint64_t, vector<uint64_t>>;
using Calibrations = vector<Calibration>;

Calibrations parse_file(const string &file_name) {
    fstream file{file_name};
    string line;
    Calibrations cals;
    while (getline(file, line)) {
        vector<uint64_t> numbers;
        uint64_t test_value{};
        stringstream ss{line};
        ss >> test_value;
        char c;  // skip ;
        ss >> c;
        uint64_t n;
        while (ss >> n) numbers.emplace_back(n);
        cals.emplace_back(Calibration{test_value, numbers});
    }
    return cals;
}

uint64_t concatenate(uint64_t a, uint64_t b) {
    for (uint64_t i = b; i; i /= 10) a *= 10;
    return a + b;
}

bool dsf(const auto &numbers, int index, uint64_t result, uint64_t target,
         bool concat) {
    if (index == numbers.size()) return result == target;
    return dsf(numbers, index + 1, result * numbers[index], target, concat) ||
           dsf(numbers, index + 1, result + numbers[index], target, concat) ||
           (concat && dsf(numbers, index + 1,
                          concatenate(result, numbers[index]), target, concat));
}

uint64_t run(const Calibrations &cals, bool concat) {
    uint64_t result = 0;
    for (const auto &cal : cals) {
        uint64_t target = cal.first;
        const auto &numbers = cal.second;
        if (dsf(numbers, 1, numbers[0], target, concat)) result += target;
    }
    return result;
}

uint64_t part1(const Calibrations &cals) { return run(cals, false); }
uint64_t part2(const Calibrations &cals) { return run(cals, true); }

int main(int argc, char **argv) {
    const auto &cals = parse_file(argv[1]);
    timeit([&]() { return part1(cals); });
    timeit([&]() { return part2(cals); });
    return 0;
}
