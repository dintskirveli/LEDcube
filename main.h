#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>

#include <set>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
//boost::tuple<int,int,int>
typedef boost::tuple<int,int,int> coord;
extern std::set< coord > onCoords;

extern pthread_mutex_t depth_mutex;

#endif