#include <print>

#include "../utils.h"

using namespace std;

void part1(const Table &) {}

void part2(const Table &) {}

int main(int argc, char **argv) {
    Table table = parse(argv[1]);
    part1(table);
    part2(table);
    return 0;
}
