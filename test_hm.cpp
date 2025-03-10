#include "diskhm.hpp"
#ifndef DIM
#define DIM 2
#endif

#define KeyType long long;
#define ValueType long long;

using namespace std;
// generate random points of DIM 2
template <class T, size_t Dim>
void generate_random_points(vector<point<T, Dim>> &points, size_t Num)
{
    points.reserve(Num);
    for (size_t i = 0; i < Num; ++i)
    {
        point<T, Dim> p;
        for (size_t d = 0; d < Dim; ++d)
        {
            p[d] = std::rand() % 100000;
        }
        points.emplace_back(p);
    }
}

int main()
{

    vector<point<double, DIM>> points;
    generate_random_points(points, 1000000);

    DiskHM<double, DIM> diskhm(points);

    // test range query
    std::vector<box<double,DIM>> query_boxes;
    query_boxes.reserve(1000);
    for (size_t i = 0; i < 1000; ++i)
    {
        point<double,DIM> min_corner;
        point<double,DIM> max_corner;
        for (size_t d = 0; d < DIM; ++d)
        {
            min_corner[d] = std::rand() % 100000;
            // static query box size
            max_corner[d] = min_corner[d] + 100;
            // max_corner[d] = std::rand() % 100000;
        }
        query_boxes.emplace_back(min_corner, max_corner);
    }

    auto start=std::chrono::steady_clock::now();
    int tc=0;
    for (auto &box : query_boxes)
    {
        tc+=diskhm.range_query(box);
    }

    auto end=std::chrono::steady_clock::now();

    std::cout << "Range Query Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000 << " [μs]" << std::endl;
    std::cout<<"Avg Block IO:"<<tc/1000<<std::endl;
    return 0;
};