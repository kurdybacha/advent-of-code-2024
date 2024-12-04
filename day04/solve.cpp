#include <fstream>
#include <print>
#include <vector>

using namespace std;

using Matrix = vector<string>;

Matrix parse(const string &file_name) {
    fstream file{file_name};
    string line;
    Matrix matrix;
    while (getline(file, line)) matrix.emplace_back(line);
    return matrix;
}

struct Pos {
    int x;
    int y;
    void operator+=(const Pos pos) {
        x += pos.x;
        y += pos.y;
    }
};

bool inside(const Matrix &m, const Pos &pos) {
    return pos.x >= 0 && pos.x < m[0].size() && pos.y >= 0 && pos.y < m.size();
}

int count_xmases_1(const Matrix &m, const Pos &pos) {
    vector<Pos> moves = {{-1, 0}, {-1, -1}, {0, -1}, {1, -1},
                         {1, 0},  {1, 1},   {0, 1},  {-1, 1}};

    if (m[pos.y][pos.x] != 'X') return 0;
    int count = 0;
    const string &pattern = "MAS";
    for (auto move : moves) {
        Pos dir = pos;
        dir += move;
        for (int i = 0; i < pattern.size(); ++i) {
            if (!inside(m, dir)) break;
            if (m[dir.y][dir.x] != pattern[i])
                break;
            else
                dir += move;
            if (i == pattern.size() - 1) ++count;
        }
    }
    return count;
}

int count_xmases_2(const Matrix &m, const Pos &pos) {
    if (m[pos.y][pos.x] != 'A') return 0;

    Pos p1a = pos;
    p1a += {-1, -1};
    Pos p1b = pos;
    p1b += {1, 1};
    Pos p2a = pos;
    p2a += {1, -1};
    Pos p2b = pos;
    p2b += {-1, 1};

    auto match = [&m](Pos a, Pos b) -> bool {
        if (!(inside(m, a) && inside(m, b))) return false;
        char a1 = m[a.y][a.x];
        char a2 = m[b.y][b.x];
        if (a1 != a2 && (a1 == 'M' || a1 == 'S') && (a2 == 'M' || a2 == 'S'))
            return true;
        return false;
    };
    if (match(p1a, p1b) && match(p2a, p2b)) return 1;
    return 0;
}

int main(int argc, char **argv) {
    const Matrix &matrix = parse(argv[1]);
    int part1 = 0;
    int part2 = 0;
    for (int i = 0; i < matrix.size(); ++i)
        for (int j = 0; j < matrix[0].size(); ++j) {
            part1 += count_xmases_1(matrix, {j, i});
            part2 += count_xmases_2(matrix, {j, i});
        }
    println("Part1: {}", part1);
    println("Part2: {}", part2);
}
