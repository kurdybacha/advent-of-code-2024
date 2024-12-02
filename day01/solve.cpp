#include <chrono>
#include <cstdint>
#include <fstream>
#include <print>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

using Column = vector<uint64_t>;
using Columns = pair<Column, Column>;

Columns parse(const string &file_name) {
    fstream file{file_name};
    string line;
    Column column1;
    Column column2;
    uint64_t n1{};
    uint64_t n2{};
    while (getline(file, line)) {
        istringstream ss(line);
        ss >> n1 >> n2;
        column1.emplace_back(n1);
        column2.emplace_back(n2);
    }
    return {column1, column2};
}

void part1(Columns &columns) {
    sort(columns.first.begin(), columns.first.end());
    sort(columns.second.begin(), columns.second.end());
    uint64_t result = 0;
    for (size_t i = 0; i < columns.first.size(); ++i) {
        auto n1 = columns.first[i];
        auto n2 = columns.second[i];
        if (n2 < n1) swap(n1, n2);
        result += n2 - n1;
    }
    println("Part1: {}", result);
}

void part2(Columns &columns) {
    unordered_map<uint64_t, uint64_t> counts;
    for (auto n : columns.second) {
        counts[n]++;
    }
    uint64_t result = 0;
    for (auto n : columns.first) {
        result += n * counts[n];
    }
    println("Part2: {}", result);
}

int main(int argc, char **argv) {
    Columns columns = parse(argv[1]);
    {
        const auto start{std::chrono::steady_clock::now()};
        part1(columns);
        const auto end{std::chrono::steady_clock::now()};
        const auto elapsed_time =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        println("elapsed_time: {}", elapsed_time);
    }
    {
        const auto start{std::chrono::steady_clock::now()};
        part2(columns);
        const auto end{std::chrono::steady_clock::now()};
        const auto elapsed_time =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        println("elapsed_time: {}", elapsed_time);
    }
    return 0;
}
