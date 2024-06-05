#ifndef TEST_H4
#define TEST_H4
#include <iostream>  
#include <vector>  
#include <queue>  
#include <unordered_map>  
#include <utility>  
#include <ctime>  
#include "vehicle.h"
#include "passenger.h"

using namespace std;

//busid must not be 0, starting from 1, passengerid must be larger than busid, it cannot be the same, there will be problems
class HopcroftKarp {
private:
    unordered_map<int, vector<int>> bipartite_graph; 
    unordered_map<int, int> matching; 
    unordered_map<int, int> distance; 
    bool bfs();

    bool dfs(int u);

public:
    HopcroftKarp(const unordered_map<int, vector<int>>& graph) : bipartite_graph(graph) {}
    unordered_map<int, int> max_matching();
};
#endif
