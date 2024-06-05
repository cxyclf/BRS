#ifndef TEST
#define TEST
#include <bits/stdc++.h>
#include <fstream>
#include "passenger.h"
#include "time.h"
#include "vehicle.h"
#include <functional>
#include "HopcroftKarp.h"
#include "match.h"
#include "global.h"
#include <algorithm>
#include <execution>
#include"update_near_bus.h"
#include "adjacency_list.h"
#include <omp.h>
#include <tbb/concurrent_vector.h>
#include <sstream>
#include <chrono>
#include <numeric> // std::accumulate
using namespace std;




// 记录执行时间到文件
void recordExecutionTimeToFile(const std::chrono::steady_clock::time_point& start, const std::chrono::steady_clock::time_point& end, const std::string& description, std::ofstream& outputFile) {
    // 计算时间差
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // 将描述字符串、执行时间和单位写入文件
    outputFile << description << ": " << duration.count() << " milliseconds" << std::endl;
}



std::unordered_map<int, std::vector<int>> remove_passengers_and_empty_buses(
    const std::unordered_map<int, std::vector<int>>& bus_passenger_bipartite_graph,
    const std::unordered_set<int>& passenger_ids) {

    std::unordered_map<int, std::vector<int>> new_graph;

    for (const auto& entry : bus_passenger_bipartite_graph) {
        int bus_id = entry.first;
        const std::vector<int>& passengers = entry.second;

        std::vector<int> filtered_passengers;
        // 过滤掉已经存在于passenger_ids中的乘客
        std::copy_if(passengers.begin(), passengers.end(), std::back_inserter(filtered_passengers),
                     [&passenger_ids](int passenger_id) {
                         return passenger_ids.find(passenger_id) == passenger_ids.end();
                     });

        // 只有在过滤后的乘客列表不为空时才添加到新的图中
        if (!filtered_passengers.empty()) {
            new_graph[bus_id] = std::move(filtered_passengers);
        }
    }

    return new_graph;
}


int main() {

    ofstream outputFile("output.csv");
    WeightedGraph g = create_adjacency_list(GRAPH_SIZE);

   
    vector<bus> bus_list = creat_bus_list(bus_number);

    
    vector<passenger> passenger_list = creat_passenger_list(passenger_filename,g.shortest_distance_matrix,bus_list.back().bus_ID);//Enter the last id of the bus list
    
    int not_served_passengers = 0;
    
    int not_served_passengers_count = 0;
    
    vector<passenger> not_served_passenger_list;//Records of passengers not being served transfer them to the next round
    
    int biggest_capacity=max_capacity;


    auto start_main = std::chrono::steady_clock::now();
   

    static auto last_processed_position = passenger_list.begin();
    
    //Simulate the main program of the day
    for (int i = 0; i < 24 * (3600/TIME_LAG); ++i) { 
        time_t currentTimestamp = startTimestamp + i * TIME_LAG; 
        printDatetime(currentTimestamp);
        string current_time = getCurrentTimestamp(currentTimestamp);
        outputFile << current_time << endl;


        auto start = std::chrono::steady_clock::now();

        vector<passenger> current_passenger_list;
        if (!not_served_passenger_list.empty()) {
            outputFile << "Number of passengers not served：" << not_served_passenger_list.size() << endl;
            copy_if(not_served_passenger_list.begin(), not_served_passenger_list.end(), std::back_inserter(current_passenger_list), [&](const passenger& passenger) {
                time_t x = currentTimestamp - TIME_LAG;
                return passenger.publish_time <= currentTimestamp && passenger.publish_time > x;
                });
            not_served_passenger_list.clear();
        };
    
        for (auto it = last_processed_position; it != passenger_list.end(); ++it) {
	    if (it->publish_time > currentTimestamp) {
	        break; 
	    }
            if (it->publish_time <= currentTimestamp && it->publish_time > (currentTimestamp - TIME_LAG)) {
                current_passenger_list.push_back(*it);
                last_processed_position = it;
            }
           
        }
        
        outputFile << "number of passenger orders taken：" << current_passenger_list.size() << endl;  
        int current_served_passengers_count=0;
        for(auto p:current_passenger_list){
        	current_served_passengers_count=current_served_passengers_count+p.passenger_count;
        }
        outputFile << "Number of passengers on this round of service:" << current_served_passengers_count<< endl;

       
        vector<bus>& current_bus_list = bus_list;
       
       
	int full_load_bus=0;
	int empty_bus=0;
	if(parallel==0){
		for (auto& b : current_bus_list) {
		    
		    b.update_state(g, currentTimestamp);
		}
        }
        else if(parallel==1){
        	#pragma omp parallel for
		for (auto& b : current_bus_list) {
		    b.update_state(g, currentTimestamp);
		}
        }
     
	
	for (auto& b : current_bus_list) {
	    if (b.capacity < biggest_capacity) {
		biggest_capacity = b.capacity;
	    }
	    if(b.capacity==0){
	    	full_load_bus++;
	    }
	    if(b.capacity==max_capacity){
	    	empty_bus++;
	    }
	}
	
       
        unordered_set<int> matched_passengers;
        unordered_map<int, vector<int>>  bus_passenger_bipartite_graph;
        
        if(parallel==0){
        	update_near_bus(current_passenger_list,current_bus_list,g.shortest_distance_matrix);
	        
		bus_passenger_bipartite_graph = bipartite_graph(current_bus_list, current_passenger_list, g.shortest_distance_matrix, currentTimestamp, matched_passengers);
        
        
        }
        else if(parallel==1){
        	update_near_bus_parallel(current_passenger_list,current_bus_list,g.shortest_distance_matrix);
        	bus_passenger_bipartite_graph = bipartite_graph_parallel(current_bus_list, current_passenger_list, g.shortest_distance_matrix, currentTimestamp, matched_passengers);
        }
     
    
        HopcroftKarp hk(bus_passenger_bipartite_graph);

        //Get corresponding vehicle and passenger connection pairs, < vehicle, passenger >
        unordered_map<int, int>  matching = hk.max_matching();
        

	    std::unordered_map<int, passenger> passenger_map;
        for (const auto& p : current_passenger_list) {
	    passenger_map.emplace(p.passenger_id, p);
        }
	
      
     
        if(parallel==0){
            for (std::vector<bus>::size_type i = 0; i < current_bus_list.size(); ++i) {
                auto& b = current_bus_list[i];
                if (b.capacity <= 0) {
	                continue; 
                }
                auto it = matching.find(b.bus_ID);
                if (it != matching.end()) {
	                int matched_passenger_ID = it->second; 
	                auto it = passenger_map.find(matched_passenger_ID);
	                if (it != passenger_map.end()) {
	                const passenger& matched_passenger = it->second;
	                b.insert_passenger(g, currentTimestamp, matched_passenger);
	                }
		               
                }else{
	                b.bus_insert_state=0;
                }
            }
        }
        else if(parallel==1){
	      	#pragma omp parallel for
		    for (std::vector<bus>::size_type i = 0; i < current_bus_list.size(); ++i) {
		        auto& b = current_bus_list[i];
		        if (b.capacity <= 0) {
		            continue; 
		        }
		        auto it = matching.find(b.bus_ID);
		        if (it != matching.end()) {
			        int matched_passenger_ID = it->second; 
			        auto it = passenger_map.find(matched_passenger_ID);
			        if (it != passenger_map.end()) {
				    const passenger& matched_passenger = it->second;
				    b.insert_passenger(g, currentTimestamp, matched_passenger);
			        }
		               
		        }else{
		    	    b.bus_insert_state=0;
		        }
		    }
        }
      	
        unordered_set<int> passenger_ids;
        for (const auto& pair : matching) {
	    passenger_ids.insert(pair.second);
        }
        
        vector<passenger> unmatched_passengers = find_unmatched_passengers(current_passenger_list,passenger_ids);
       
        for (auto& p : unmatched_passengers) {
            p.publish_time = p.publish_time + TIME_LAG;//Let passengers who are not served order release time + a time interval participate in the next round
            if (p.publish_time <= p.start_time-300) {
                not_served_passenger_list.push_back(p);
            }
            if (p.publish_time > p.start_time-300) {
                not_served_passengers = not_served_passengers + 1;
                not_served_passengers_count=not_served_passengers_count+p.passenger_count;
            }
        }
        
        
        
        
   	auto end = std::chrono::steady_clock::now();
        recordExecutionTimeToFile(start, end, "total:", outputFile);
        
        vector<double> current_total_seat_utilization;//Seat utilization
        for (auto& b : current_bus_list) {
        	if(b.capacity!=max_capacity){
    			double seating_ratio = static_cast<double>(max_capacity-b.capacity)/max_capacity;
    			current_total_seat_utilization.emplace_back(seating_ratio);
        	}
	}
        double current_total_sum = std::accumulate(current_total_seat_utilization.begin(), current_total_seat_utilization.end(), 0.0);
        double current_total_average = current_total_sum / current_total_seat_utilization.size();
    
        outputFile << "Average load factor of this round: " <<current_total_average << std::endl;
        outputFile << "Close the cycle" << endl;
        outputFile << "----------------------------------------------------------------------------" << endl;

    }
    

    vector<bus>& current_bus_list = bus_list;
    vector<double> total_seat_utilization;//Seat utilization
    

    
    for (auto& b : current_bus_list) {
	    if(!b.seat_utilization.empty()){
	        double sum = std::accumulate(b.seat_utilization.begin(), b.seat_utilization.end(), 0.0);
	        double average = static_cast<double>(sum / b.seat_utilization.size());
	        total_seat_utilization.emplace_back(average);
	    }
    }
    
    double total_sum = std::accumulate(total_seat_utilization.begin(), total_seat_utilization.end(), 0.0);
    double total_average = total_sum / total_seat_utilization.size();
    
    outputFile << "Average load factor of all vehicles: " <<total_average << std::endl;
    
    outputFile << "The number of passengers being served: " << 4100000-not_served_passengers_count << endl;
    
 
    auto end_main = std::chrono::steady_clock::now();
    auto duration_main = std::chrono::duration_cast<std::chrono::milliseconds>(end_main - start_main);
    outputFile << "Total execution time: " << duration_main.count() << " milliseconds" << std::endl;
    
    // 关闭CSV文件
    outputFile.close();
    

    return 0;
}
    
#endif







