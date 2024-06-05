#ifndef TEST_H1
#define TEST_H1

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_map> 
#include <unordered_set>
#include "global.h"
using namespace std;




class WeightedGraph {
public:
    struct Edge {
        int to;
        int weight;
    };
    vector<vector<Edge>> adj_list;

    vector<vector<int>>shortest_distance_matrix;

    WeightedGraph(int n);
   
    void add_edge(int from, int to, int weight);

    void print();

};

std::vector<std::vector<int>> creat_shortest_distance_matrix();

WeightedGraph create_adjacency_list(int val);

#endif

