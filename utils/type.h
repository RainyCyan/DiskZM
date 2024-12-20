#include <array>
#include <vector>
//the dimension should be determined in compile time for performance consideration
template <class T,size_t Dim>
using point_t = std::array<T, Dim>;

template <class T,size_t Dim>
using points_t = std::vector<point_t<T,Dim>>;