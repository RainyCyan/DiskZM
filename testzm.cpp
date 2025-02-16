#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

template<size_t Dim>
class point {
public:
    point() = default;

    friend std::ostream& operator<<(std::ostream& out, const point& p) {
        out << "coords:[ ";
        for (size_t d = 0; d < Dim; d++)
            out << p.coords[d] << " ";
        out << "], rank:[ ";
        for (size_t d = 0; d < Dim; d++)
            out << p.rank[d] << " ";
        out << "];";
        return out;
    }

    double* coords;
    size_t* rank;  // sorted by the selected dim and the dim's val's rank
    size_t z_addr; // mapping val by the rank
};

template<size_t Dim>
inline void argsort(std::vector<point<Dim>>& points) {
    auto n = points.size();
    for (size_t i = 0; i < Dim; i++) {
        std::vector<size_t> indices(n);
        std::iota(indices.begin(), indices.end(), 0);

        // Sort indices based on the values in the i-th dimension
        std::sort(indices.begin(), indices.end(), [&points, i](size_t pos1, size_t pos2) {
            return points[pos1].coords[i] < points[pos2].coords[i];
        });

        // Assign ranks to the points based on the sorted order
        for (size_t j = 0; j < n; j++) {
            points[indices[j]].rank[i] = j;
        }
    }
}

int main() {
    constexpr size_t Dim = 2;
    std::vector<point<Dim>> pts(5);

    // Allocate space for point
    for (int i = 0; i < 5; i++) {
        pts[i].coords = new double[Dim];
        pts[i].rank = new size_t[Dim];
        for (size_t d = 0; d < Dim; d++)
            pts[i].coords[d] = std::fmod((i + 1) * 13.0 / 5, 7);
    }

    argsort(pts);

    // Print points
    for (const auto& p : pts) {
        std::cout << p << std::endl;
    }

    // Free allocated memory
    for (auto& p : pts) {
        delete[] p.coords;
        delete[] p.rank;
    }

    return 0;
}
