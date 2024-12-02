#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using Row = std::vector<int64_t>;
using Table = std::vector<Row>;

Table parse(const std::string &file_name) {
    std::fstream file{file_name};
    std::string line;
    Table table;
    uint64_t n{};
    while (getline(file, line)) {
        std::istringstream ss(line);
        Row row;
        while (ss >> n) row.emplace_back(n);
        table.emplace_back(row);
    }
    return table;
}
