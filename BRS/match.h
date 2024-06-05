#ifndef TEST_H5
#define TEST_H5
#include <bits/stdc++.h>
#include "vehicle.h"
#include "passenger.h"
#include <unordered_map> 
#include <vector>
#include <unordered_set>
#include "global.h"
#include <future>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/concurrent_unordered_set.h> 
#include <algorithm>
#include <limits> 


struct MatchInfo {
    int up_position;
    int down_position;
    int detour_distance; 

    MatchInfo(int up, int down, int dist) : up_position(up), down_position(down), detour_distance(dist) {}
};

bool match(const bus &b, passenger &p,const vector<vector<int>>& shortest_distance_matrix, const time_t& current_time);


bool chooseBestMatch(const vector<MatchInfo>& match_up_down, passenger& p, int bus_ID);


vector<passenger> find_unmatched_passengers(const vector<passenger>& current_passenger_list, const unordered_set<int>& matched_passengers);


unordered_map<int, vector<int>> bipartite_graph(const vector<bus>& current_bus_list, vector<passenger>& current_passenger_list, const vector<vector<int>>& shortest_distance_matrix, const time_t current_time, unordered_set<int>& matched_passengers);


unordered_map<int, vector<int>> bipartite_graph_parallel(const vector<bus>& current_bus_list, vector<passenger>& current_passenger_list, const vector<vector<int>>& shortest_distance_matrix, const time_t current_time, unordered_set<int>& matched_passengers);


#endif
