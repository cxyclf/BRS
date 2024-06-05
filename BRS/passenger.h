#ifndef TEST_H6
#define TEST_H6
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include "global.h"

#include <unordered_map> 
#include <unordered_set> 
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
using namespace std;



class passenger {
public:
	int passenger_id;

	time_t publish_time;

	time_t start_time;

	time_t end_time;

	int up_position;//Or

	int down_position;//dr
	
	int distance;//Minimum distance
	
	int passenger_count;//Number of travelers
	
	vector<int> near_up_position;//SNOL

	vector<int> near_down_position;//SNDL

	unordered_map<int, vector<pair<int, int>>> near_up_position_bus; //Record the passenger schedule near the pick-up point, with the car id in front and the location and order of the stops behind

	unordered_map<int, vector<pair<int, int>>> near_down_position_bus; //Record the passenger schedule near the drop-off point, with the car id in front and the location and order of the stops behind

	unordered_map<int, vector<pair<int, int>>>  Pluggable_set;//The match with the smallest distance records the insertion position of the car that can be loaded, the car id in front is the insertion position of the boarding point and the exit point is the insertion position
        
	passenger(int passengerid, time_t publishtime, time_t starttime, time_t endtime, int upposition, int downposition, int dis , int count, const vector<vector<int>>& shortest_distance_matrix);

	pair<vector<int>, vector<int>> creat_near_positions(const vector<vector<int>>& shortest_distance_matrix);

};

vector<passenger> creat_passenger_list(const string& filename,  const vector<vector<int>> dist, int bus_max_id);



#endif
