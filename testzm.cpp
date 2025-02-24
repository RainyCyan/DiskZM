//g++ multidimensional.cpp -mbmi2 -std=c++17 -I../include -o multidimensional
#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include "diskzm.hpp"
#ifndef DIM
#define DIM 2
#endif

int main() {
    std::vector<point<DIM>> data;
    data.reserve(1000000);
    //generate random points using point template array of size DIM
    srand(time(0));
    for (size_t i = 0; i < 1000000; ++i)
    {
        point<DIM> p;
        for (size_t d = 0; d < DIM; ++d)
        {
            p[d] = std::rand() % 100000;
        }
        data.emplace_back(p);
    }
    
    constexpr size_t eps=64;
    DiskZM<DIM,eps> diskzm(data);
    
    //range query
    std::vector<box<DIM>> query_boxes;
    query_boxes.reserve(1000);
    for (size_t i = 0; i < 1000; ++i)
    {
        point<DIM> min_corner;
        point<DIM> max_corner;
        for (size_t d = 0; d < DIM; ++d)
        {
            min_corner[d] = std::rand() % 100000;
            //static query box size
            max_corner[d] = min_corner[d] + 10;
            // max_corner[d] = std::rand() % 100000;
        }
        query_boxes.emplace_back(min_corner, max_corner);
    }
    auto start = std::chrono::steady_clock::now();
    for (auto &box : query_boxes)
    {
        // auto result = diskzm.range_query(box);
        diskzm.range_query(box);
        // std::cout << "range query result size: " << result.size() << std::endl;
    }
    auto end = std::chrono::steady_clock::now();
    //cout the average time for range query
    
   
    std::cout << "Range Query Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000 << " [μs]" << std::endl;
    
    //test knn query
    std::vector<point<DIM>> query_points;
    for(int i=0;i<1000;i++)
    {
        point<DIM> p;
        for (size_t d = 0; d < DIM; ++d)
        {
            p[d] = std::rand() % 100000;
        }
        query_points.emplace_back(p);
    }
    start = std::chrono::steady_clock::now();
    // auto result = diskzm.knn_query(p, 10);
    for(auto &p:query_points)
    {
        diskzm.knn_query(p, 10);
    }
    // std::cout << "knn query result size: " << result.size() << std::endl;
    end = std::chrono::steady_clock::now();
    std::cout << "KNN Query Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000 << " [μs]" << std::endl;
    return 0;
}
