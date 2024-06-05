
#include "HopcroftKarp.h"

bool HopcroftKarp::bfs() {
    queue<int> q;
    for (const auto& pair : bipartite_graph) {
        int u = pair.first;
        if (matching[u] == 0) {
            distance[u] = 0;
            q.push(u);
        } else {
            distance[u] = numeric_limits<int>::max();
        }
    }
    distance[0] = numeric_limits<int>::max();
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        if (u != 0) {
            for (int v : bipartite_graph[u]) {
                if (distance[matching[v]] == numeric_limits<int>::max()) {
                    distance[matching[v]] = distance[u] + 1;
                    q.push(matching[v]);
                }
            }
        }
    }
    return (distance[0] != numeric_limits<int>::max());
}

bool HopcroftKarp::dfs(int u) {
    if (u != 0) {
        for (int v : bipartite_graph[u]) {
            if (distance[matching[v]] == distance[u] + 1 && dfs(matching[v])) {
                matching[u] = v;
                matching[v] = u;
                return true;
            }
        }
        distance[u] = numeric_limits<int>::max();
        return false;
    }
    return true;
}

unordered_map<int, int> HopcroftKarp::max_matching() {
    for (const auto& pair : bipartite_graph) {
        matching[pair.first] = 0;
        for (int v : pair.second) {
            if (matching.find(v) == matching.end()) {
                matching[v] = 0;
            }
        }
    }
    matching[0] = 0;

    int matching_size = 0;
    while (bfs()) {
        for (const auto& pair : bipartite_graph) {
            if (matching[pair.first] == 0 && dfs(pair.first)) {
                matching_size++;
            }
        }
    }
    unordered_map<int, int> bus_passenger_matching;
    for (const auto& pair : matching) {
        if (bipartite_graph.find(pair.first) != bipartite_graph.end() && pair.second != 0) {
            bus_passenger_matching[pair.first] = pair.second;
        }
    }
    return bus_passenger_matching;
}


