#pragma once

#include <cstddef>

template <std::size_t N>
inline __attribute__((always_inline)) void nop()
{
    asm volatile("nop");
    nop<N - 1>();
}

template <>
inline __attribute__((always_inline)) void nop<0>()
{ }
