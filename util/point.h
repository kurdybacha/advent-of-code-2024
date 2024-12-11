#include <format>
#include <unordered_set>

namespace aoc {

template <typename T>
struct point {
    T x;
    T y;
    bool operator==(const point &) const = default;
    point operator-() noexcept { return {-x, -y}; }
    point &operator+=(const point &p) noexcept {
        x += p.x;
        y += p.y;
        return *this;
    }
};

template <typename T>
point<T> operator+(const point<T> &lhs, const point<T> &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

template <typename T>
point<T> operator-(const point<T> &lhs, const point<T> &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
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

template <typename T>
struct formatter<aoc::point<T>> : formatter<std::string> {
    auto format(const aoc::point<T> &p, format_context &ctx) const {
        return format_to(ctx.out(), "{{{}, {}}}", p.x, p.y);
    }
};

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

}  // namespace std
