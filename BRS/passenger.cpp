#include "passenger.h"


vector<passenger> creat_passenger_list(const string& filename, const vector<vector<int>> shortest_distance_matrixt,int bus_max_id) {
    vector<passenger> passengers;
    ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件： " << filename << std::endl;
        return passengers;
    }
    std::string line;
   
    int i = bus_max_id +1;//Avoid using the same vehicle number as the passenger
    std::getline(file, line);
    
    while (getline(file, line)) {
        istringstream iss(line);
        vector<string> words;
        string word;
        while (getline(iss, word, ',')) {
            words.push_back(word);
        }
        
	    std::istringstream s1(words[0]);
	    std::time_t publishtime;
	    s1 >> publishtime;

	    std::istringstream s2(words[1]);
	    std::time_t starttime;
	    s2 >> starttime;

	    std::istringstream s3(words[2]);
	    std::time_t endtime;
	    s3 >> endtime;
        
        int upposition = stoi(words[3]);

        int downposition = stoi(words[4]);

        int shortest_distance = stoi(words[5]);
        
        int count = stoi(words[6]);
       
        passenger p(i, publishtime, starttime, endtime, upposition, downposition, shortest_distance,count,shortest_distance_matrixt);

        passengers.push_back(p);

        i++;
        cout << i << endl;
    }

    file.close();
    return passengers;
}




passenger::passenger(int passengerid, time_t publishtime, time_t starttime, time_t endtime, int upposition, int downposition, int dis , int count, const vector<vector<int>>& shortest_distance_matrix) {
    passenger_id = passengerid;
    publish_time = publishtime;
    start_time = starttime;
    end_time = endtime;
    up_position = upposition;
    down_position = downposition;
    distance = dis;
    passenger_count=count;
    tie(near_up_position, near_down_position) = creat_near_positions(shortest_distance_matrix);
}


pair<vector<int>, vector<int>> passenger::creat_near_positions(const vector<vector<int>>& shortest_distance_matrix) {
    vector<int> near_up_position;
    vector<int> near_down_position;
    for (size_t i = 0; i < shortest_distance_matrix.size(); ++i) {
        if (shortest_distance_matrix[up_position][i] <= static_cast<int>(up_wait_time * speed) ){
            near_up_position.push_back(i);
        }
        if (shortest_distance_matrix[down_position][i] <= static_cast<int>(down_wait_time * speed) ){
            near_down_position.push_back(i);
        }
    }
    return make_pair(near_up_position, near_down_position);
}






