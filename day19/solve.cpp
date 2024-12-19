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

int run(const towels &towels, const patterns &patterns) {

    return 0;
}

int main(int argc, char **argv) {
    const auto &[towels, patterns] = parse_file(argv[1]);
    timeit([&]() { return run(towels, patterns); });
    return 0;
}
