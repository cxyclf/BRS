#include "vehicle.h"


std::vector<int> sort_vector(const std::vector<int>& original) {
    if (original.empty()) {
        return original; 
    }
    std::vector<int> sorted = original; 
    std::sort(sorted.begin(), sorted.end()); 
    return sorted;
}




bus::bus(int ID, int current_position) {
    bus_current_position = current_position;
    bus_ID = ID;
    bus_state = -1;//Indicates that the vehicle is empty
    bus_insert_state = 0;
    bus_current_time = 0;
}



int random_number(int min, int max) {
    return min + rand() % (max - min + 1);
}


vector<bus> creat_bus_list(int bus_number) {
    vector<bus> buss;
    srand(time(nullptr)); 
    for (int j = 1; j < bus_number; j++) {
        int num_buses = random_number(0, GRAPH_SIZE-1);
        bus b(j, num_buses);
        buss.push_back(b);
    }

    std::cout << buss.size() << std::endl;
    return buss;
}



void merge_schedule_and_ddl(vector<int>& bus_schedule, vector<time_t>& ddl) {
    if (bus_schedule.size() != ddl.size()) {
        cout << "Error: Size of bus_schedule and ddl must be the same." << endl;
        return;
    }
    vector<int> new_bus_schedule;
    vector<time_t> new_ddl;
    new_bus_schedule.push_back(bus_schedule[0]);
    new_ddl.push_back(ddl[0]);
    for (size_t i = 1; i < bus_schedule.size(); ++i) {
        if (bus_schedule[i] == bus_schedule[i - 1]) {
            if (ddl[i] < ddl[i - 1]) {
                new_ddl.back() = ddl[i];
            }
        }
        else {
            new_bus_schedule.push_back(bus_schedule[i]);
            new_ddl.push_back(ddl[i]);
        }
    }
    bus_schedule = new_bus_schedule;
    ddl = new_ddl;
}


void bus::insert_passenger(const WeightedGraph& graph, time_t current_time, passenger p) {
    passenger_list.insert(passenger_list.begin(), p);
    if (bus_schedule.empty()) { 
        if (bus_current_position == p.up_position) {
            bus_schedule.push_back(bus_current_position);
            bus_schedule.push_back(p.down_position);
            capacity = capacity - p.passenger_count; 
            build_arr_ddl_slk(graph.shortest_distance_matrix, current_time, p);         
            update_actual_schedule(graph, current_time, bus_schedule[0], bus_schedule[1]);
             
        }
        if (bus_current_position != p.up_position) {
            bus_schedule.push_back(bus_current_position);
            bus_schedule.push_back(p.up_position);
            bus_schedule.push_back(p.down_position);
            capacity = capacity - p.passenger_count;  
            build_arr_ddl_slk(graph.shortest_distance_matrix, current_time, p);
            update_actual_schedule(graph, current_time, bus_schedule[0], bus_schedule[1]);
            
        }
        
    }

    else if (!bus_schedule.empty()&&!p.Pluggable_set.empty()&&capacity>0) {
        for (auto pair : p.Pluggable_set[bus_ID]) {
            size_t up_position = pair.first;
            size_t down_position = pair.second;
            if (up_position < bus_schedule.size()&& down_position < bus_schedule.size()) {
                bus_schedule.insert(bus_schedule.begin() + down_position, p.down_position);
                ddl.insert(ddl.begin() + down_position, p.end_time);
                bus_schedule.insert(bus_schedule.begin() + up_position, p.up_position);
                ddl.insert(ddl.begin() + up_position, p.start_time);
                capacity = capacity - p.passenger_count; 
                merge_schedule_and_ddl(bus_schedule, ddl);
                update_arr_slk(graph.shortest_distance_matrix, bus_current_time);
                update_actual_schedule(graph, bus_current_time, bus_schedule[0], bus_schedule[1]);
                bus_insert_state = 1;
                break;
            }
            if (up_position < bus_schedule.size() && down_position >= bus_schedule.size()) {
                bus_schedule.push_back(p.down_position);
                ddl.push_back(p.end_time);
                bus_schedule.insert(bus_schedule.begin() + up_position, p.up_position);
                ddl.insert(ddl.begin() + up_position, p.start_time);
                capacity = capacity - p.passenger_count;
                merge_schedule_and_ddl(bus_schedule, ddl);
                update_arr_slk(graph.shortest_distance_matrix, bus_current_time);
                update_actual_schedule(graph, bus_current_time, bus_schedule[0], bus_schedule[1]);
                bus_insert_state = 1;
                break;
            }
            else
            {
                bus_schedule.push_back(p.up_position);
                ddl.push_back(p.start_time);
                bus_schedule.push_back(p.down_position);
                ddl.push_back(p.end_time);
                capacity = capacity - p.passenger_count;
                merge_schedule_and_ddl(bus_schedule, ddl);
                update_arr_slk(graph.shortest_distance_matrix, bus_current_time);
                update_actual_schedule(graph, bus_current_time, bus_schedule[0], bus_schedule[1]);
                bus_insert_state = 1;
                break;
            }
            
        }      
    }    
}

void bus::build_arr_ddl_slk(const vector<vector<int>>& shortest_distance_matrix, time_t current_time,passenger p) {
    arr = vector<time_t>(bus_schedule.size(), 0); 
    ddl = vector<time_t>(bus_schedule.size(), 0); 
    slk = vector<time_t>(bus_schedule.size(), 0); 
    if (bus_schedule.size() == 2) {
        ddl[1] = p.end_time;
    }
    else if(bus_schedule.size() == 3){
        ddl[1] = p.start_time;
        ddl[2] = p.end_time;
    }
    for (size_t i = 1; i < bus_schedule.size(); ++i) {
        if (i == 1) {
            arr[i] = current_time + static_cast<time_t>(shortest_distance_matrix[bus_schedule[i - 1]][bus_schedule[i]] / speed);
        }
        else {
            arr[i] = arr[i - 1] + static_cast<time_t>(shortest_distance_matrix[bus_schedule[i - 1]][bus_schedule[i]] / speed);
        }
          
        slk[i] = ddl[i] - arr[i];
    }
    for (size_t i = slk.size() - 2; i >= 1; --i) {
        slk[i] = min(slk[i], slk[i + 1]);
    }
    
}



void bus::update_arr_slk(const vector<vector<int>>& shortest_distance_matrix, time_t current_time) {
    arr = vector<time_t>(bus_schedule.size(), 0); 
    slk = vector<time_t>(bus_schedule.size(), 0); 

    for (size_t i = 1; i < bus_schedule.size(); ++i) {
        if (i == 1) {
            arr[i] = current_time + static_cast<time_t>(shortest_distance_matrix[bus_schedule[i - 1]][bus_schedule[i]] / speed);
        }
        else {
            arr[i] = arr[i - 1] + static_cast<time_t>(shortest_distance_matrix[bus_schedule[i - 1]][bus_schedule[i]] / speed);
        }
    }
    for (size_t i = 1; i < slk.size(); ++i) {
        slk[i] = ddl[i] - arr[i];
    }
    for (size_t i = slk.size() - 2; i >= 1; --i) {
        slk[i] = min(slk[i], slk[i + 1]);
    }

}


// Find the shortest path from the beginning to the end
vector<int> findShortestPath(const WeightedGraph& graph, int start, int end) { 
      if (start == end) {
        return {start};
    }
    vector<int> path = {start};
    int current = start;
    int previous = start;
    unordered_set<int> visited;
    unordered_set<int> invalidNodes; 
    while (current != end) {
        int next = -1;
        bool found = false; 
        for (auto neighbor : graph.adj_list[current]) {
            int nextNode = neighbor.to;
            int distance = neighbor.weight + graph.shortest_distance_matrix[nextNode][end];
            if (distance == graph.shortest_distance_matrix[current][end] && visited.find(nextNode) == visited.end() && invalidNodes.find(nextNode) == invalidNodes.end()) {
                next = nextNode;
                found = true;
                break;
            }
        }
        if (found) {
            path.push_back(next);
            visited.insert(current);
            previous = current;
            current = next;
        } else {
            path.pop_back();
            invalidNodes.insert(current); 
            visited.erase(previous); 
            current = previous;
        }

        if (visited.find(current) != visited.end()) {
            break;
        }
    }

    return path;
}

void bus::update_actual_schedule(const WeightedGraph& graph, time_t current_time, int start, int end) {
    vector<int> path = findShortestPath(graph, start, end);
    actual_schedule.resize(path.size());
    for (size_t i = 0; i < path.size(); ++i) {
        actual_schedule[i].first = path[i]; 
        if (i == 0) {  
            actual_schedule[i].second = current_time;
        }
        else { 
            actual_schedule[i].second = actual_schedule[i - 1].second + static_cast<time_t>(graph.shortest_distance_matrix[path[i]][path[i - 1]] / speed);
        }
        
    }

}


void bus::update_state(const WeightedGraph& graph,time_t current_time) {
    if (!bus_schedule.empty()&&!actual_schedule.empty()) {
        if (bus_schedule[0] != actual_schedule.back().first) {
            int old_bus_schedule_0 = bus_schedule[0];
            for (size_t i = 0; i < actual_schedule.size() - 1; i++) {
                if (current_time >= actual_schedule[i].second && current_time < actual_schedule[i+1].second) {
                    bus_current_position = actual_schedule[i].first;
                    bus_schedule[0] = actual_schedule[i].first;
                    bus_current_time = actual_schedule[i].second;
                }
            }
            if (bus_schedule[0] != old_bus_schedule_0) {
                bus_state = 1;    //When the current position and the previous position of the vehicle change, the state is 1
            }
            else {
                bus_state = 0; //When the vehicle schedule has not changed, it is 0
            }


        }

        if (current_time >= actual_schedule.back().second) {
            bus_current_position = actual_schedule.back().first;
            bus_schedule[0] = bus_current_position;
            bus_state = 1;
            bus_current_time = actual_schedule.back().second;
            bus_schedule.erase(bus_schedule.begin()+1);
            if (bus_schedule.size() == 1) {
                bus_schedule.clear();
                arr.clear();
                ddl.clear();
                slk.clear();
                actual_schedule.clear();
                bus_current_time = 0;     
                bus_state = -1;//Indicates that the vehicle is empty
            }

            if (bus_schedule.size() >= 2) {
                actual_schedule.clear();
                arr.erase(arr.begin() + 1);
                ddl.erase(ddl.begin() + 1);
                slk.erase(slk.begin() + 1);
                update_actual_schedule(graph, bus_current_time, bus_schedule[0], bus_schedule[1]);
            }
            
        }

    }

    if(bus_state!=0){
       vector<pair<int, int>> down_passenger_ids; 
	    for (const auto& p : passenger_list) {
                if (p.down_position == bus_current_position &&std::find(bus_schedule.begin(), bus_schedule.end(), p.up_position) == bus_schedule.end()) {
			    down_passenger_ids.emplace_back(p.passenger_id, p.passenger_count);
	         }
	    }	
	    for (const auto& id_count_pair : down_passenger_ids) {
	        int id = id_count_pair.first;
	        int passenger_count = id_count_pair.second;
	        auto it = std::remove_if(passenger_list.begin(), passenger_list.end(),
	             [id](const auto& p) { return p.passenger_id == id; });
	        if (it != passenger_list.end()) {
	             passenger_list.erase(it, passenger_list.end());
	             capacity += passenger_count;
	        }
	    }
	    if (bus_schedule.size() > 1) {
		    int index = 0; 
		    while (true) {
		        auto it2 = std::find(bus_schedule.begin() + 1, bus_schedule.end(), bus_current_position);
		        if (it2 == bus_schedule.end()) {
			    break;
		        }
		        it2 = bus_schedule.erase(it2); 
		        ddl.erase(ddl.begin() + index + 1);
		        ++index;
		    }
		    if (bus_schedule.size() > 1) {
		        merge_schedule_and_ddl(bus_schedule, ddl);
		        update_arr_slk(graph.shortest_distance_matrix, bus_current_time);
		        update_actual_schedule(graph, bus_current_time, bus_schedule[0], bus_schedule[1]);
		    } else {
		        bus_schedule.clear();
		        arr.clear();
		        ddl.clear();
		        slk.clear();
		        actual_schedule.clear();
		        bus_current_time = 0;
		        bus_state = -1; //Indicates that the vehicle is empty
		    }
	    }
    
    }
     
    if(capacity<max_capacity){
    	double seating_ratio = static_cast<double>(max_capacity-capacity)/max_capacity;
    	seat_utilization.emplace_back(seating_ratio);
    } 
}





