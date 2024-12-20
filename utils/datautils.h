#include"type.h"
#include<fstream>
#include<iostream>

template <class T,size_t dim>
size_t read_points(points_t<T,dim> &out_points, const std::string &fname)
{
    std::ifstream in(fname, std::ios::binary);
    if (!in.is_open())
    {
        std::cerr << "Failed to open file: " << fname << std::endl;
        return 0;
    }

    size_t N = 0;
    point_t<T,dim> tmp;
    while (in.read(reinterpret_cast<char*>(tmp.data()), sizeof(T) * dim))
    {
        out_points.emplace_back(tmp);
        N++;
    }

    in.close();
    return N;
}