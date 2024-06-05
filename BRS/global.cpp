
#include "global.h"





int TIME_LAG = 60;   //Where the time interval is changed, the value here affects the time change, such as changing the time to 30 seconds to update once, or 60 seconds to 120 seconds, and so on

int GRAPH_SIZE=5079;  //Number of bus stops

time_t startTimestamp = 1479484800; // time_t time of 00:00 November 19, 2016, the start time of the loop

double speed = 8.4;  //The speed of the bus is 5.6m/s, which is 20km/h. 8.4 is 30KM/h. 13.3 is 48km/h

int up_wait_time = 600;    //The waiting time for passengers is measured in seconds  SNOL

int down_wait_time = 600;   //SNDL

int max_capacity = 32;  //Maximum vehicle capacity

int bus_number=14000;   //Maximum number of vehicles

int parallel=1;//Control whether parallel, 0 is off, 1 is on

std::string adjlsit_list_filename = "adjlsit_list.csv";      

std::string shortest_distances_filename="shortest_distances.csv";

std::string passenger_filename="passenger.csv";
