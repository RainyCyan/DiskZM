#pragma once
#include <cstddef>
#include <array>

const long BLOCK_SIZE = 8192/2;
template <typename T, size_t Dim>
using point = std::array<T, Dim>;

template <typename T, size_t Dim>
using box = std::pair<point<T, Dim>, point<T, Dim>>;