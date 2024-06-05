#ifndef TEST_H8
#define TEST_H8
#include "passenger.h"
#include <bits/stdc++.h>
#include<unordered_set>
#include "global.h"
#include "vehicle.h"
#include <tbb/parallel_for.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_sort.h>
#include <tbb/parallel_for_each.h>
#include <omp.h>

void update_near_bus(vector<passenger>& current_passenger_list, const vector<bus>& current_bus_list,const vector<vector<int>>& shortest_distance_matrix);

void update_near_bus_parallel(std::vector<passenger>& current_passenger_list, const std::vector<bus>& current_bus_list, const std::vector<std::vector<int>>& shortest_distance_matrix);

void updateNearPositionBus(passenger& p, const bus& b, const vector<vector<int>>& shortest_distance_matrix);
#endif
