#include <chrono>
#include <fstream>
#include <print>
#include <sstream>
#include <string>
#include <vector>

namespace utils {

void timeit(auto func) {
    const auto start{std::chrono::steady_clock::now()};
    auto result = func();
    const auto end{std::chrono::steady_clock::now()};
    const auto elapsed_time =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    println("{} ({})", result, elapsed_time);
}

}  // namespace utils
