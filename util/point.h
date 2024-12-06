#include <unordered_set>

namespace aoc {

template <typename T>
struct point {
    T x;
    T y;
    bool operator==(const point &) const = default;
};

template <typename T>
point<T> operator+(const point<T> &lhs, const point<T> &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

template <typename T>
using point_set = std::unordered_set<point<T>>;

}  // namespace aoc

namespace std {
template <typename T>
struct hash<aoc::point<T>> {
    std::size_t operator()(const aoc::point<T> &p) const noexcept {
        std::size_t h1 = std::hash<T>{}(p.x);
        std::size_t h2 = std::hash<T>{}(p.y);
        return h1 ^ (h2 << 1);
    }
};

}  // namespace std
