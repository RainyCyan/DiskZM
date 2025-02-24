#pragma once
#include <stddef.h>
#include <vector>
#include <array>
#include "pgm/pgm_index_variants.hpp"
// customizable point struct for zm-index
// struct point
// {
//     double *coords;
//     size_t *rank; // sorted by the selected dim and the dim's val's rank
//     size_t z_addr;                  // mapping val by the rank
// };
template <size_t Dim>
using point = std::array<double, Dim>;

template <size_t Dim>
using box = std::pair<point<Dim>, point<Dim>>;

template <size_t Dim, size_t Epsilon>
class DiskZM
{
    using mpgm = pgm::MultidimensionalPGMIndex<Dim, size_t, Epsilon>;
    using morton = mortonnd::MortonNDBmi<Dim, uint64_t>;
    using value_type = decltype(morton::Decode(0));
private:
    //tool function
    // turn a double point to ints to compute the z-value
    inline size_t to_id(double val, size_t I)
    {
        if (val <= this->mins[I])
        {
            return 0;
        }
        if (val >= this->maxs[I])
        {
            return (this->maxs[I] - this->mins[I]) / this->widths[I];
        }
        return static_cast<size_t>((val - this->mins[I]) / this->widths[I]);
    }

    template <typename Array, std::size_t... I>
    inline auto a2t_impl(const Array &a, std::index_sequence<I...>)
    {
        return std::make_tuple(to_id(a[I], I)...);
    }

    template <typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
    inline auto a2t(const std::array<T, N> &a)
    {
        return a2t_impl(a, Indices{});
    }

    template <typename tuple_t>
    constexpr auto get_array_from_tuple(tuple_t &&tuple)
    {
        constexpr auto get_array = [](auto &&...x)
        { return std::array{std::forward<decltype(x)>(x)...}; };
        return std::apply(get_array, std::forward<tuple_t>(tuple));
    }


public:
    DiskZM() : mpgm_(nullptr) {};
    ~DiskZM()
    {
        if (mpgm_)
            delete mpgm_;
    }
    DiskZM(std::vector<point<Dim>> &points)
    {
        this->data_ = points;
        this->N = points.size();
        this->index_handler = nullptr;
        //init mins and maxs
        std::fill(mins.begin(), mins.end(), std::numeric_limits<double>::max());
        std::fill(maxs.begin(), maxs.end(), std::numeric_limits<double>::min());

        build(data_);
    }

    void build(std::vector<point<Dim>> &points)
    {   
        auto start=std::chrono::steady_clock::now();
        this->resolution = static_cast<size_t>(pow(data_.size(), 1.0 / Dim));

        for (size_t i = 0; i < Dim; ++i)
        {
            for(auto &p:points)
            {
                mins[i] = std::min(p[i], mins[i]);
                maxs[i] = std::max(p[i], maxs[i]);
            }
        }

        // the grid resolution to calculate the Z-value is set to N^{1/d}
        this->resolution = static_cast<size_t>(pow(N, 1.0 / Dim));

        // widths of each dimension
        for (size_t i = 0; i < Dim; ++i)
        {
            widths[i] = (maxs[i] - mins[i]) / this->resolution;
        }

        std::vector<value_type> tuples;
        tuples.reserve(points.size());
        for (auto &p : points)
        {
            tuples.emplace_back(a2t(p));
        }
        char *index_name = "test1";
        this->index_handler = fopen(index_name, "w+b");
        mpgm_ = new mpgm(tuples.begin(), tuples.end(),true,index_handler);//on-disk
        auto end=std::chrono::steady_clock::now();

        std::cout<<"Build Time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<" [ms]"<<std::endl;
    }

    void range_query(box<Dim> &box)
    {
        auto min_tup = a2t(box.first);
        auto max_tup = a2t(box.second);

        std::vector<std::array<size_t, Dim>> temp;
        int tc=0;
        FILE *tmp_index_handler=fopen("test1","r+b");
        auto it=mpgm_->range_query(tmp_index_handler,min_tup,max_tup,tc);
        for(auto &p:it)
        {
            temp.emplace_back(get_array_from_tuple(p));
        }

        std::vector<point<Dim>> result;
        result.reserve(temp.size());
        for (auto &tp : temp)
        {
            point<Dim> p;
            for (size_t d = 0; d < Dim; ++d)
            {
                p[d] = tp[d];
            }
            result.emplace_back(p);
        }
    }

    void knn_query(point<Dim> &p, size_t k)
    {
        auto zp = a2t(p);
        auto result=mpgm_->knn(zp,k);
    }
   
    void insert(point<Dim> &p)
    {
        auto zp = a2t(p);
        mpgm_->insert(zp);
    }

    private:
    // the grid resolution to compute the z address
    // by default, it is set to N^{1/d}
    size_t resolution;
    std::vector<point<Dim>> data_; // data point loaded in memory
    // boundaries of each dimension
    std::array<double, Dim> mins;
    std::array<double, Dim> maxs;
    std::array<double, Dim> widths;
    size_t N;                 // points' count(equals to data_.size)
    mpgm *mpgm_;                // one-dimensional learned index,we here use pgm
    FILE* index_handler; // file handler for on-disk index
};


