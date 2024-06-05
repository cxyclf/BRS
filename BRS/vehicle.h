#ifndef TEST_H9
#define TEST_H9
#include <vector>
#include "passenger.h"
#include "time.h"
#include "global.h"
#include <ctime>
#include <cstdlib> 
#include "adjacency_list.h"
using namespace std;

class bus {

public:
	
	int bus_ID;


	int bus_state;//Record whether the status of the actual vehicle exercise table has changed
	
	int bus_insert_state;//Record whether the vehicle insertion status changes
	
	int bus_current_position;//bs.L
	
	time_t bus_current_time;
	
	vector <int> bus_schedule;
	
	vector<pair<int, time_t>> actual_schedule;

	int capacity=max_capacity;

	vector <passenger> passenger_list;//List of passengers on board the vehicle

	vector<time_t> arr;

	vector<time_t> ddl;

	vector<time_t> slk;

	vector<double> seat_utilization;

	bus(int ID, int current_position);

	void update_state(const WeightedGraph& graph, time_t current_time);

	void insert_passenger(const WeightedGraph& graph, time_t current_time, passenger p);

	void build_arr_ddl_slk(const vector<vector<int>>& shortest_distance_matrix, time_t current_time,passenger p);
	
	void update_arr_slk(const vector<vector<int>>& shortest_distance_matrix, time_t current_time);

	void update_actual_schedule(const WeightedGraph& graph, time_t current_time, int start, int end);

};

std::vector<int> sort_vector(const std::vector<int>& original);

vector<int> findShortestPath(const WeightedGraph& graph, int start, int end);

vector<bus> creat_bus_list(int bus_number);

int random_number(int min, int max) ;

void merge_schedule_and_ddl(vector<int>& bus_schedule, vector<time_t>& ddl);

#endif
