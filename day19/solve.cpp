#include <fstream>
#include <print>
#include <ranges>

#include "../utils.h"

using namespace std;
using namespace utils;

using towels = vector<string>;
using patterns = vector<string>;

tuple<towels, patterns> parse_file(const string &file_name) {
    ifstream file{file_name};
    string line;
    getline(file, line);
    towels towels =
        line | views::split(views::all(", "sv)) | ranges::to<vector<string>>();
    getline(file, line);  // skip empty line
    patterns patterns;
    while (getline(file, line)) {
        patterns.emplace_back(line);
    }
    return {towels, patterns};
}



bool check_pattern(const string & pattern, const towels & towels, size_t pos) {
    if (pos == pattern.size()) return true;
    bool found = false;
    for(auto && towel : towels) {
        if (pattern.substr(pos, towel.size()) == towel) {
            if (check_pattern(pattern, towels, pos + towel.size())) {
                found = true;
                break;
            }
        }
    }
    return found;
}

int run(const towels &towels, const patterns &patterns) {
    int result = 0;
    for( auto && pattern : patterns) {
        if (check_pattern(pattern, towels, 0) )
            ++result;
    }
    return result;
}

int main(int argc, char **argv) {
    const auto &[towels, patterns] = parse_file(argv[1]);
    timeit([&]() { return run(towels, patterns); });
    return 0;
}
