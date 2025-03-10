#pragma once
#include<stddef.h>

//data process functions
//data read/write function,need to support txt/csv/bin format
template <size_t Dim>
size_t read_points_from_bin(std::vector<point<Dim>> &points, const std::string &fname)
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
        points.emplace_back(tmp);
        N++;
    }

    in.close();
    return N;
}
// template<size_t Dim>
// inline void read_points_from_file(std::vector<point<Dim>> &points, const std::string &filename, const std::string &delim,const std::string &file_type)
// {
//     switch (file_type)
//     {
//     case "csv":
//         // read_points_from_csv(points, filename,delim);
//         break;
//     case "bin":
//         read_points_from_bin(points, filename);
//         break;
    
//     default:
//         break;
//     }
// }
