#include <print>
#include <regex>

#include "../utils.h"

using namespace std;

uint64_t parse_and_count(const string file_name, bool enable_inst = false) {
    regex expr_regex("do\\(\\)|don't\\(\\)|mul\\((\\d{1,3}),(\\d{1,3})\\)");
    fstream file{file_name};
    string line;
    uint64_t result = 0;
    bool enabled = true;
    while (getline(file, line)) {
        auto it = sregex_iterator(line.cbegin(), line.cend(), expr_regex);
        auto end = sregex_iterator();
        for (sregex_iterator i = it; i != end; ++i) {
            smatch match = *i;
            string match_str = match.str();
            if (match_str.starts_with("mul") && enabled) {
                result += stol(match[1].str()) * stol(match[2].str());
            } else if (enable_inst) {
                enabled = match_str == "do()";
            }
        }
    }
    return result;
}

void part1(const string &file_name) {
    println("Part 1: {}", parse_and_count(file_name));
}

void part2(const string &file_name) {
    println("Part 2: {}", parse_and_count(file_name, true));
}

int main(int argc, char **argv) {
    part1(argv[1]);
    part2(argv[1]);
    return 0;
}
