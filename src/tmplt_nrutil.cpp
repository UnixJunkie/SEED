/*
*    This file is part of SEED.
*
*    Copyright (C) 2017, Caflisch Lab, University of Zurich
*
*    SEED is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    SEED is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <cmath>
#include <nrutil.h>

template <class T>
T max_el(T *v, int l, int r)
{
    /*
    Returns max element of a vector
  */
    T max;

    max = v[l];
    if (l == r)
    {
        return max;
    }
    for (auto i = l + 1; i <= r; i++)
    {
        if (v[i] > max)
        {
            max = v[i];
        }
    }
    return max;
}

template <class T>
T max_abs_el(T *v, int l, int r)
{
    /*
    Returns max  absolute element of a vector
  */
    T max;

    max = std::abs(v[l]);
    if (l == r)
    {
        return max;
    }
    for (auto i = l + 1; i <= r; i++)
    {
        if (std::abs(v[i]) > max)
        {
            max = std::abs(v[i]);
        }
    }
    return max;
}

template <class T>
void sum_vec(T *sum, T *a, T *b, int start, int end)
{
    for (auto i=start; i <= end; i++)
    {
        sum[i] = a[i] + b[i];
    }
}

template <class T>
void sub_vec(T *sub, T *a, T *b, int start, int end)
{
    for (auto i = start; i <= end; i++)
    {
        sub[i] = a[i] - b[i];
    }
}

// template <class T, class T2>
// void mul_vec(T alpha, T2 *vec, int start, int end)
// {
//     for (auto i = start; i <= end; i++)
//     {
//         vec[i] = vec[i] * alpha;
//     }
// }
