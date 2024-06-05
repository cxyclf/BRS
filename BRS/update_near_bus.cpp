#include"update_near_bus.h"



void updateNearPositionBus(passenger& p, const bus& b, const vector<vector<int>>& shortest_distance_matrix) {
    unordered_map<int, size_t> station_index_map;
    for (size_t i = 0; i < b.bus_schedule.size(); ++i) {
        station_index_map[b.bus_schedule[i]] = i;
    }

    if (difftime(p.start_time, p.publish_time) < 599 && ((b.bus_state == 0 && b.bus_insert_state == 1) || (b.bus_state == 1 && b.bus_insert_state == 1) ||(b.bus_state == 1 && b.bus_insert_state == 0))&&b.bus_state!=-1) {
        p.near_up_position_bus.erase(b.bus_ID);
        p.near_down_position_bus.erase(b.bus_ID);
        p.Pluggable_set.erase(b.bus_ID);

        for (const auto& station : b.bus_schedule) {
            if (shortest_distance_matrix[p.up_position][station] <= static_cast<int>(up_wait_time * speed)) {
                auto distance_to_station = station_index_map[station];
                p.near_up_position_bus[b.bus_ID].push_back(make_pair(station, distance_to_station));
            }
            if (shortest_distance_matrix[p.down_position][station] <= static_cast<int>(down_wait_time * speed)) {
                auto distance_to_station = station_index_map[station];
                p.near_down_position_bus[b.bus_ID].push_back(make_pair(station, distance_to_station));
            }
        }
        if (p.near_down_position_bus.find(b.bus_ID) == p.near_down_position_bus.end()) {
            p.near_up_position_bus.erase(b.bus_ID);
        }
    }

    if (difftime(p.start_time, p.publish_time) > 599) {
        for (const auto& station : b.bus_schedule) {
            if (shortest_distance_matrix[p.up_position][station] <= static_cast<int>(up_wait_time * speed)) {
                auto distance_to_station = station_index_map[station];
                p.near_up_position_bus[b.bus_ID].push_back(make_pair(station, distance_to_station));
            }
            if (shortest_distance_matrix[p.down_position][station] <= static_cast<int>(down_wait_time * speed)) {
                auto distance_to_station = station_index_map[station];
                p.near_down_position_bus[b.bus_ID].push_back(make_pair(station, distance_to_station));
            }
        }
        if (p.near_down_position_bus.find(b.bus_ID) == p.near_down_position_bus.end()) {
            p.near_up_position_bus.erase(b.bus_ID);
        }
    }
}


void update_near_bus(vector<passenger>& current_passenger_list, const vector<bus>& current_bus_list,const vector<vector<int>>& shortest_distance_matrix) {

     for (size_t i = 0; i < current_passenger_list.size(); ++i) {
        auto& p = current_passenger_list[i];
	    for (const auto& b : current_bus_list) {
			if (!b.bus_schedule.empty() && (shortest_distance_matrix[p.up_position][b.bus_schedule[0]]<=static_cast<int>(up_wait_time * speed))) {
				if(b.bus_schedule.size()<=2){
					if(shortest_distance_matrix[p.down_position][b.bus_schedule[0]]<=static_cast<int>(down_wait_time * speed)||shortest_distance_matrix[p.down_position][b.bus_schedule[1]]<=static_cast<int>(down_wait_time * speed)){                               
		        		 updateNearPositionBus(p,b,shortest_distance_matrix);
		        		 continue;  
					}
		            
				}else{
		    
		    		int first = b.bus_schedule[0];
	    			int middle = b.bus_schedule[b.bus_schedule.size() / 2];
	    			int last = b.bus_schedule[b.bus_schedule.size() - 1];
					if(shortest_distance_matrix[p.down_position][first]<=static_cast<int>(down_wait_time * speed)||shortest_distance_matrix[p.down_position][middle]<=static_cast<int>(down_wait_time * speed)||shortest_distance_matrix[p.down_position][last]<=static_cast<int>(down_wait_time * speed)){
		        		 updateNearPositionBus(p,b,shortest_distance_matrix);
		        		continue;  
					}
					else if(shortest_distance_matrix[p.down_position][middle] < shortest_distance_matrix[p.down_position][first] && shortest_distance_matrix[p.down_position][last]<shortest_distance_matrix[p.down_position][middle]){
		        		 updateNearPositionBus(p,b,shortest_distance_matrix);
					}
				} 
			} 
			else {
				p.near_up_position_bus.erase(b.bus_ID);
				p.near_down_position_bus.erase(b.bus_ID);
			} 
		}
	   
     }  
   
}


void update_near_bus_parallel(std::vector<passenger>& current_passenger_list, const std::vector<bus>& current_bus_list, const std::vector<std::vector<int>>& shortest_distance_matrix) {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, current_passenger_list.size()),
        [&](const tbb::blocked_range<size_t>& r) {
            for (size_t i = r.begin(); i != r.end(); ++i) {
                auto& p = current_passenger_list[i];
				for (const auto& b : current_bus_list) {
		     	
					if (!b.bus_schedule.empty() && (shortest_distance_matrix[p.up_position][b.bus_schedule[0]]<=static_cast<int>(up_wait_time * speed))) {
						if(b.bus_schedule.size()<=2){
							if(shortest_distance_matrix[p.down_position][b.bus_schedule[0]]<=static_cast<int>(down_wait_time * speed)||shortest_distance_matrix[p.down_position][b.bus_schedule[1]]<=static_cast<int>(down_wait_time * speed)){                               
							 updateNearPositionBus(p,b,shortest_distance_matrix);
							 continue;  
						}
				    
						}else{
			    
			    			int first = b.bus_schedule[0];
		    				int middle = b.bus_schedule[b.bus_schedule.size() / 2];
		    				int last = b.bus_schedule[b.bus_schedule.size() - 1];
							if(shortest_distance_matrix[p.down_position][first]<=static_cast<int>(down_wait_time * speed)||shortest_distance_matrix[p.down_position][middle]<=static_cast<int>(down_wait_time * speed)||shortest_distance_matrix[p.down_position][last]<=static_cast<int>(down_wait_time * speed)){
								 updateNearPositionBus(p,b,shortest_distance_matrix);
								continue;  
							}
							else if(shortest_distance_matrix[p.down_position][middle] < shortest_distance_matrix[p.down_position][first] && shortest_distance_matrix[p.down_position][last]<shortest_distance_matrix[p.down_position][middle]){
								 updateNearPositionBus(p,b,shortest_distance_matrix);
							}
						} 
					} 
					else {
						p.near_up_position_bus.erase(b.bus_ID);
						p.near_down_position_bus.erase(b.bus_ID);
					} 
				}
            }
        }
    );
}




