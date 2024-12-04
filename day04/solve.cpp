#include <fstream>
#include <print>
#include <vector>

#include "../utils.h"

using namespace std;
using namespace utils;

struct Pos {
    int x;
    int y;
    void operator+=(const Pos &p) {
        x += p.x;
        y += p.y;
    }
};
Pos operator+(const Pos &p1, const Pos &p2) {
    return {p1.x + p2.x, p1.y + p2.y};
}

struct Matrix {
    vector<string> m;
    size_t width() const { return m.at(0).size(); }
    size_t height() const { return m.size(); }
    bool contains(const Pos &pos) const {
        return pos.x >= 0 && pos.x < width() && pos.y >= 0 && pos.y < height();
    }
    char at(const Pos &pos) const { return m[pos.y][pos.x]; }
};

Matrix matrixFromFile(const string &file_name) {
    fstream file{file_name};
    string line;
    Matrix matrix;
    while (getline(file, line)) matrix.m.emplace_back(line);
    return matrix;
}

const string pattern = "MAS";

int count_xmases_1(const Matrix &matrix, const Pos &pos) {
    vector<Pos> moves = {{-1, 0}, {-1, -1}, {0, -1}, {1, -1},
                         {1, 0},  {1, 1},   {0, 1},  {-1, 1}};

    if (matrix.at(pos) != 'X') return 0;
    int count = 0;
    for (const auto &move : moves) {
        Pos curr_pos = pos + move;
        for (char c : pattern) {
            if (!matrix.contains(curr_pos)) break;
            if (matrix.at(curr_pos) != c)
                break;
            else
                curr_pos += move;
            if (c == 'S') ++count;
        }
    }
    return count;
}

int count_xmases_2(const Matrix &matrix, const Pos &pos) {
    if (matrix.at(pos) != 'A') return 0;

    Pos p1a = pos + Pos{-1, -1};
    Pos p1b = pos + Pos{1, 1};
    Pos p2a = pos + Pos{1, -1};
    Pos p2b = pos + Pos{-1, 1};

    auto match = [&matrix](Pos a, Pos b) -> bool {
        if (!(matrix.contains(a) && matrix.contains(b))) return false;
        char ca = matrix.at(a);
        char cb = matrix.at(b);
        if (ca != cb && (ca == 'M' || ca == 'S') && (cb == 'M' || cb == 'S'))
            return true;
        return false;
    };
    if (match(p1a, p1b) && match(p2a, p2b)) return 1;
    return 0;
}

int run(const Matrix &matrix, auto counterFunc) {
    int result = 0;
    for (int i = 0; i < matrix.width(); ++i)
        for (int j = 0; j < matrix.height(); ++j) {
            result += counterFunc(matrix, {i, j});
        }
    return result;
}

int main(int argc, char **argv) {
    const Matrix &matrix = matrixFromFile(argv[1]);
    timeit([&]() -> int { return run(matrix, count_xmases_1); });
    timeit([&]() -> int { return run(matrix, count_xmases_2); });
}
