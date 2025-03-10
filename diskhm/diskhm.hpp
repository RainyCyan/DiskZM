#pragma once
/// \author RainyCyan
/// \date 2025.3.3
/// \brief DiskHM class
/// \details DiskHM is a class that implements the Hilbert-curves based multidimensional learned index

#include <vector>
#include <iostream>
#include <string>
#include <limits>
#include <cmath>
#include <chrono>
#include <cassert>

#include "utils.h"
#include "pgm/pgm_index_dynamic.hpp"
// #include "pgm/piecewise_linear_model.hpp"
#include "hilbert.h"

template <class T, size_t Dim>
class DiskHM
{
public:
    //kv pair on disk: key is point,value is hilbert value
    typedef struct
    {
        point<T, Dim> key;
        long long value;
    } ItemOnDisk;
 
public:
    DiskHM() { this->index_name = "DiskHM"; };
    DiskHM(std::vector<point<T, Dim>> &points)
    {
        this->index_name = "DiskHM";
        this->data_ = points;
        this->N = points.size();
        // this->index_handler = nullptr;
        // init mins and maxs
        std::fill(mins.begin(), mins.end(), std::numeric_limits<T>::max());
        std::fill(maxs.begin(), maxs.end(), std::numeric_limits<T>::min());
        this->resolution = static_cast<size_t>(pow(this->N, 1.0 / Dim));

        // print the index info
        std::cout << "Index Info: " << this->index_name << " Dim: " << Dim << " Epsilon: " << std::endl;
        std::cout << "Resolution: " << this->resolution << std::endl;
        // print the data info
        std::cout << "Data Info: " << std::endl;
        std::cout << "Points' count: " << this->N << std::endl;

        build_hm(data_);
    }
    ~DiskHM() {}

    /// \param points the data points to build the index
    void build_hm(std::vector<point<T, Dim>> &points)
    {
        std::cout << "Building DiskHM index..." << std::endl;
        auto start = std::chrono::steady_clock::now();

        for (size_t i = 0; i < Dim; ++i)
        {
            for (auto &p : points)
            {
                mins[i] = std::min(p[i], mins[i]);
                maxs[i] = std::max(p[i], maxs[i]);
            }
        }
        // widths of each dimension
        for (size_t i = 0; i < Dim; ++i)
        {
            widths[i] = (maxs[i] - mins[i]) / this->resolution;
        }
        //print mins,maxs
        std::cout << "mins: ";
        for (size_t i = 0; i < Dim; ++i)
        {
            std::cout << mins[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "maxs: ";
        for (size_t i = 0; i < Dim; ++i)
        {
            std::cout << maxs[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "widths: ";
        for (size_t i = 0; i < Dim; ++i)
        {
            std::cout << widths[i] << " ";
        }
        std::cout << std::endl;

        // compute the grid id for each point
        std::vector<point<long long, Dim>> grid_data;
        grid_data.reserve(this->N);
        for (auto &p : points)
        {
            point<long long, Dim> ids;
            _grid_id(p, ids);
            grid_data.emplace_back(ids);
        }

        // hilbert only support Dim=2 now,so add check here
        if (Dim != 2)
        {
            std::cerr << "Hilbert curve only support Dim=2 now!" << std::endl;
            return;
        }

        long long side = pow(2, ceil(log(this->resolution) / log(2)));
        // compute the hilbert value for each point
        std::vector<long long> curve_vals;
        curve_vals.reserve(this->N);
        for (auto &p : grid_data)
        {
            long long curve_val = compute_Hilbert_value(p[0], p[1], side);
            curve_vals.emplace_back(curve_val);
        }
        // sort the curve values
        std::sort(curve_vals.begin(), curve_vals.end());
        auto count=curve_vals.size();
        std::vector<std::pair<long long,long long>> data(count);
        for (int i = 0; i < count; i++) {
            data[i].first = curve_vals[i];
            data[i].second = curve_vals[i] + 1;
        }
        // build the index
        bool _inner_disk = true;
        // pgm = decltype(pgm)(curve_vals.begin(), curve_vals.end(), _inner_disk, index_handler);
        pgm=new pgm::DynamicPGMIndex<long long,long long>(true,_inner_disk,data.begin(),data.end());

        auto end = std::chrono::steady_clock::now();
        // print the build time
        std::cout << "Build Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " [ms]" << std::endl;
    }

    /// \param query_box the query box
    int range_query(box<T, Dim> &query_box)
    {
        // auto start = std::chrono::steady_clock::now();
        point<long long, Dim> min_corner;
        point<long long, Dim> max_corner;
        _grid_id(query_box.first, min_corner);
        _grid_id(query_box.second, max_corner);
        long long min_hilbert_val=compute_Hilbert_value(min_corner[0],min_corner[1],this->resolution);
        long long max_hilbert_val=min_hilbert_val;
        //bruce traverse
        for(long long x=min_corner[0];x<max_corner[0];x++)
        {
            for(long long y=min_corner[1];y<max_corner[1];y++)
            {
                long long hilbert_val=compute_Hilbert_value(x,y,this->resolution);
                min_hilbert_val=std::min(hilbert_val,min_hilbert_val);
                max_hilbert_val=std::max(hilbert_val,max_hilbert_val);
            }
        }
        int c=0;
        auto dc=range_filter(min_hilbert_val,max_hilbert_val,true,&c);
        return dc;
        // auto end = std::chrono::steady_clock::now();
        // std::cout << "Range Query Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " [μs]" << std::endl;
    }
private:
    std::string index_name;
    // the grid resolution to compute the z address
    // by default, it is set to N^{1/d}
    size_t resolution;
    std::vector<point<T, Dim>> data_; // data point loaded in memory
    // boundaries of each dimension
    point<T, Dim> mins;
    point<T, Dim> maxs;
    point<T, Dim> widths;
    size_t N; // points' count(equals to data_.size)
    // pgm::PGMIndex<int, Epsilon> pgm;
    pgm::DynamicPGMIndex<long long,long long> *pgm;
    /// \brief the size of a block in bytes
    const int32_t ItemOnDiskSize = sizeof(ItemOnDisk);
    const int32_t ItemCountPerBlock = int32_t(BLOCK_SIZE / ItemOnDiskSize);

    int range_filter(const long long &lo,const long long &hi,bool _on_disk,int *c){
        int fsize = 0;
        auto len=hi-lo;
        pgm::DynamicPGMIndex<long long,long long>::ItemOnDisk *a = new pgm::DynamicPGMIndex<long long,long long>::ItemOnDisk [100];
        pgm::DynamicPGMIndex<long long,long long>::ItemOnDisk *b = new pgm::DynamicPGMIndex<long long,long long>::ItemOnDisk [100];
        return pgm->range_on_disk(lo,hi,len,c,a,b,&fsize);
    }

    void _grid_id(point<T, Dim> &p, point<long long, Dim> &ids)
    {
        for (int i = 0; i < Dim; i++)
        {
            if (p[i] < this->mins[i])
                ids[i] = 0;
            else if (p[i] > this->maxs[i])
                ids[i] = this->resolution - 1;
            else
                ids[i] = static_cast<long long>((p[i] - this->mins[i]) / this->widths[i]);
        }
    }
};