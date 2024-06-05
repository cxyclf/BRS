#include "match.h"



vector<passenger> find_unmatched_passengers(const vector<passenger>& current_passenger_list, const unordered_set<int>& matched_passengers) {
    vector<passenger> unmatched_passengers; 
    for (const auto& p : current_passenger_list) {
        if (matched_passengers.find(p.passenger_id) == matched_passengers.end()) {
            unmatched_passengers.push_back(p); 
        }
    }

    return unmatched_passengers;
}


bool chooseBestMatch(const vector<MatchInfo>& match_up_down, passenger& p,int bus_ID) {
    if (match_up_down.empty()) {
        return false; 
    }
    
    const MatchInfo& best_match = *min_element(match_up_down.begin(), match_up_down.end(),
        [](const MatchInfo& a, const MatchInfo& b) {
            return a.detour_distance < b.detour_distance;
        });
    p.Pluggable_set[bus_ID].push_back(make_pair(best_match.up_position, best_match.down_position));
    return true; 
}



bool match(const bus& b, passenger& p, const vector<vector<int>>& shortest_distance_matrix, const time_t& current_time) {
    if (b.bus_schedule.empty()) {
        for (const auto& w : p.near_up_position) {
            if (b.bus_current_position == w) {
                if (b.bus_current_position == p.up_position) {
                    time_t down_time = static_cast<time_t>(current_time + shortest_distance_matrix[p.up_position][p.down_position] / speed);
                    if (down_time < p.end_time) {
                        return true; 
                    }
                }
                else if (b.bus_current_position != p.up_position) {
                    time_t up_time = static_cast<time_t>(current_time + shortest_distance_matrix[b.bus_current_position][p.up_position] / speed);
                    if (up_time < p.start_time) {
                        time_t down_time = static_cast<time_t>(up_time + shortest_distance_matrix[p.up_position][p.down_position] / speed);
                        if (down_time < p.end_time) {
                            return true; 
                        }
                    }
                }
                return false; 
            }
        }
     
    }
    vector<MatchInfo>  match_up_down;
    if (!b.bus_schedule.empty() && p.near_up_position_bus.find(b.bus_ID) != p.near_up_position_bus.end()) {
        vector<int> up_insert_position;
        for (auto pair : p.near_up_position_bus[b.bus_ID]) {
            if (pair.second == 0) {
                up_insert_position.push_back(pair.second + 1);
            }
            else
            {
                auto it = find(up_insert_position.begin(), up_insert_position.end(), pair.second);
                if (it == up_insert_position.end()) {
                    up_insert_position.push_back(pair.second);
                }
                auto it2 = find(up_insert_position.begin(), up_insert_position.end(), pair.second + 1);
                if (it2 == up_insert_position.end()) {
                    up_insert_position.push_back(pair.second + 1);
                }
            }
        }
        pair<size_t, int> min_up_insert = make_pair(std::numeric_limits<size_t>::max(), numeric_limits<int>::max()); 
        for (size_t up : up_insert_position) {
            if (up < b.bus_schedule.size()) {
                int up_detour_distance = shortest_distance_matrix[b.bus_schedule[up - 1]][p.up_position] + shortest_distance_matrix[p.up_position][b.bus_schedule[up]] - shortest_distance_matrix[b.bus_schedule[up - 1]][b.bus_schedule[up]];
                time_t up_detour_time = static_cast<time_t>(up_detour_distance / speed);
                time_t up_time;
                if (up == 1) {
                    up_time = static_cast<time_t>(b.bus_current_time + shortest_distance_matrix[b.bus_schedule[up - 1]][p.up_position] / speed);
                }
                else {
                    up_time = static_cast<time_t>(b.arr[up - 1] + shortest_distance_matrix[b.bus_schedule[up - 1]][p.up_position] / speed);
                }
                if (up_detour_time < b.slk[up] && up_time <= p.start_time&& up_detour_distance < min_up_insert.second) {
                    min_up_insert = make_pair(up, up_detour_distance);
                }
            }
            else {
                time_t up_time = static_cast<time_t>(b.arr.back() + shortest_distance_matrix[b.bus_schedule.back()][p.up_position] / speed);
                if (up_time < p.start_time) {
                    time_t down_time = static_cast<time_t>(b.arr.back() + (shortest_distance_matrix[b.bus_schedule.back()][p.up_position] + shortest_distance_matrix[p.up_position][p.down_position]) / speed);
                    if (down_time < p.end_time) {
                        match_up_down.push_back(MatchInfo(up, up, (shortest_distance_matrix[b.bus_schedule.back()][p.up_position] + shortest_distance_matrix[p.up_position][p.down_position])));
                    }
                }

            }
        }
        if (min_up_insert.first != std::numeric_limits<size_t>::max()) {
            size_t up = min_up_insert.first;
            int up_detour_distance = min_up_insert.second;
            vector<int> down_insert_position;
            for (auto pair : p.near_down_position_bus[b.bus_ID]) {
	            if (static_cast<size_t>(pair.second) >= up) {
		            if (pair.second == 0) {
			            down_insert_position.push_back(pair.second + 1);
		            }
		            else
		            {
			            auto it = find(down_insert_position.begin(), down_insert_position.end(), pair.second);
			            if (it == down_insert_position.end()) {
			            down_insert_position.push_back(pair.second);
			            }
			            auto it2 = find(down_insert_position.begin(), down_insert_position.end(), pair.second + 1);
			            if (it2 == down_insert_position.end()) {
			            down_insert_position.push_back(pair.second + 1);
			            }
		            }
 		        }
                
            }
            

            for (size_t down : down_insert_position) {
                if (down < up) {
                    continue;
                }
                if (up == down) {
                    int up_down_detour_distance = shortest_distance_matrix[b.bus_schedule[up - 1]][p.up_position] + shortest_distance_matrix[p.up_position][p.down_position] + shortest_distance_matrix[p.down_position][b.bus_schedule[up]] - shortest_distance_matrix[b.bus_schedule[up - 1]][b.bus_schedule[up]];
                    time_t up_down_down_detour_time = static_cast<time_t>(up_down_detour_distance / speed);
                    if (up_down_down_detour_time < b.slk[up]) {
                        if (up == 1) {
                            time_t down_time = static_cast<time_t>(b.bus_current_time + (shortest_distance_matrix[b.bus_schedule[up - 1]][p.up_position] + shortest_distance_matrix[p.up_position][p.down_position]) / speed);
                            if (down_time < p.end_time) {
                                match_up_down.push_back(MatchInfo(up, down, up_down_detour_distance));
                            }
                        }
                        else {
                            time_t down_time = static_cast<time_t>(b.arr[up - 1] + (shortest_distance_matrix[b.bus_schedule[up - 1]][p.up_position] + shortest_distance_matrix[p.up_position][p.down_position]) / speed);
                            if (down_time < p.end_time) {
                                match_up_down.push_back(MatchInfo(up, down, up_down_detour_distance));
                            }
                        }
                    }
                }
                if (up < down) {
                    if (down < b.bus_schedule.size()) {
                        time_t now_down_slk = b.slk[down] - (up_detour_distance/speed);
                        int donw_detour_distance = shortest_distance_matrix[b.bus_schedule[down - 1]][p.down_position] + shortest_distance_matrix[p.down_position][b.bus_schedule[down]] - shortest_distance_matrix[b.bus_schedule[down - 1]][b.bus_schedule[down]];
                        time_t down_detour_time = static_cast<time_t>(donw_detour_distance / speed);
                        if (down_detour_time < now_down_slk) {
                            time_t down_time = static_cast<time_t>(b.arr[down - 1] + (up_detour_distance / speed) + (shortest_distance_matrix[b.bus_schedule[down - 1]][p.down_position]) / speed);
                            if (p.end_time > down_time) {
                                match_up_down.push_back(MatchInfo(up, down, (up_detour_distance + donw_detour_distance)));
                            }
                        }
                    }
                    else {
                        int donw_detour_distance = shortest_distance_matrix[b.bus_schedule.back()][p.down_position];
                        time_t down_time = static_cast<time_t>(b.arr.back() + (up_detour_distance / speed) + (donw_detour_distance / speed));
                        if (p.end_time > down_time) {
                            match_up_down.push_back(MatchInfo(up, down, (up_detour_distance + donw_detour_distance)));
                        }
                    }
                }
            }     
        }
    }
    if (chooseBestMatch(match_up_down, p, b.bus_ID)) {
        return true; 
    }
    else {
        return false; 
    }
}

// A function to build a bipartite graph
unordered_map<int, vector<int>> bipartite_graph(const vector<bus>& current_bus_list,  vector<passenger>& current_passenger_list, const vector<vector<int>>& shortest_distance_matrix, const time_t current_time, unordered_set<int>& matched_passengers) {
    unordered_map<int, vector<int>> graph; 
    for (const auto& b : current_bus_list) {
        if (b.capacity <= 0) {
            continue; 
        }
        for ( auto& p : current_passenger_list) {
            if((b.capacity-p.passenger_count)<0){
            	continue; 
            }
            if (match(b, p, shortest_distance_matrix, current_time)) { 
                graph[b.bus_ID].push_back(p.passenger_id); 
                matched_passengers.insert(p.passenger_id);
            }
        }
    }

    return graph;
}


//Run in parallel to build bipartite graphs
unordered_map<int, vector<int>> bipartite_graph_parallel(const vector<bus>& current_bus_list, vector<passenger>& current_passenger_list, const vector<vector<int>>& shortest_distance_matrix, const time_t current_time, unordered_set<int>& matched_passengers) {
    tbb::concurrent_unordered_map<int, tbb::concurrent_vector<int>> graph; 
    tbb::concurrent_unordered_set<int> local_matched_passengers;
    tbb::parallel_for(tbb::blocked_range<size_t>(0, current_passenger_list.size()),
        [&](const tbb::blocked_range<size_t>& range) {
            for (size_t i = range.begin(); i != range.end(); ++i) {
                auto& p = current_passenger_list[i];
                for (const auto& b : current_bus_list) {
                   if((b.capacity-p.passenger_count)<0){
		    	continue; 
		    }
                    if (match(b, p, shortest_distance_matrix, current_time)) { 
                         graph[b.bus_ID].push_back(p.passenger_id); 
                         local_matched_passengers.insert(p.passenger_id); 
                    }
                }
            }
        });
       
   for (const auto& pair : local_matched_passengers) {
       matched_passengers.insert(pair);
   }
   unordered_map<int, vector<int>> result_graph;
   for (const auto& pair : graph) {
      result_graph[pair.first] = vector<int>(pair.second.begin(), pair.second.end());
   }
    
    return result_graph;
}

