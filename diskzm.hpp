#pragma once
#include <stddef.h>
#include <vector>
#include "pgm/pgm_index.hpp"
// customizable point struct for zm-index
struct point
{
    double *coords;
    size_t *rank; // sorted by the selected dim and the dim's val's rank
    size_t z_addr;                  // mapping val by the rank
};

template <size_t Dim, size_t Epsilon>
class DiskZM
{
    using pgm = pgm::PGMIndex<size_t, Epsilon>;

public:
    inline void argsort(const std::vector<point> &points)
    {
        // compare rule for sort
        const auto comp = [&points](int pos1, int pos2, int d_i)
        { return points[pos1][d_i] < points[pos2][d_i]; };
        auto n = points.size();
        for (size_t i = 0; i < Dim; i++)
        {
            size_t* rank_i = new size_t[n];
            std::iota(rank_i, rank_i + n, 0);
            sort(rank_i, rank_i + n, i,comp);
            // assign rank_i to p
            for (int j = 0; j < n; j++)
                points[j].rank[i] = rank_i[j];
        }
    }

public:
    DiskZM() : pgm_(nullptr) {};

    template <class RandomIt>
    void build(RandomIt first, RandomIt last);

    DiskZM(std::vector<point> &points);
    ~DiskZM()
    {
        if (pgm_)
            delete pgm_;
    }

private:
    std::vector<point> data_; // data point loaded in memory
    size_t N;                 // points' count(equals to data_.size)
    pgm *pgm_;                // one-dimensional learned index,we here use pgm
};

template <size_t Dim, size_t Epsilon>
template <class RandomIt>
void DiskZM<Dim, Epsilon>::build(RandomIt first, RandomIt last)
{
    auto start = chrono::high_resolution_clock::now(); // start clock
    // use sort,maybe a c++ argsort is more suitable
};

template <size_t Dim, size_t Epsilon>
DiskZM<Dim, Epsilon>::DiskZM(std::vector<point> &points)
{
    this->data_ = points;
    this->N = points.size();

    build(data_.begin(), data.end());
}