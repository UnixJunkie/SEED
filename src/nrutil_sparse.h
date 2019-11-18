#ifndef _NR_UTILS_SPARSE_H_
#define _NR_UTILS_SPARSE_H_

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

template <class T>
// using sparse_3D = boost::numeric::ublas::compressed_matrix<T>; // type alias (C++11)
using sparse_3D = boost::numeric::ublas::coordinate_matrix<T>; // type alias (C++11)

typedef boost::numeric::ublas::compressed_matrix<char>::iterator1 it1_t;
typedef boost::numeric::ublas::compressed_matrix<char>::iterator2 it2_t;

#endif