#include "adjacency_list.h"


WeightedGraph ::WeightedGraph(int n) {
    adj_list.resize(n);
}

void WeightedGraph::add_edge(int from, int to, int weight) {  
    Edge edge1 = { to, weight };
    Edge edge2 = { from, weight };
    adj_list[from].push_back(edge1);
    adj_list[to].push_back(edge2);
}

void WeightedGraph :: print() {
    for (size_t i = 0; i < adj_list.size(); i++) {
        cout << "point" << i << ": ";
        for (auto edge : adj_list[i]) {
            cout << "(" << edge.to << ", " << edge.weight << ") ";
        }
        cout << endl;
    }
}


WeightedGraph create_adjacency_list(int val){
    WeightedGraph g(val);
    ifstream file(adjlsit_list_filename);
    vector<vector<string>> data;
    int rowCount = 0; 
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            vector<string> row;
            stringstream ss(line);
            string cell;
            while (getline(ss, cell, ',')) {
                row.push_back(cell);
            }

            data.push_back(row);
            rowCount++; 
        }
        file.close();
    }
    else {
        cout << "123" << endl;
    }
    
    for (int i = 1;i <rowCount; i++) {
        int x = stoi(data[i][0]);
        int y = stoi(data[i][1]);
        int z = stoi(data[i][2]);
        g.add_edge(x, y, z);
    }
    
   
    g.shortest_distance_matrix = creat_shortest_distance_matrix();
    

    return g;

}


std::vector<std::vector<int>> creat_shortest_distance_matrix() {
    ifstream file(shortest_distances_filename);
    string line;
    vector<vector<string>> matrix;

    while (getline(file, line)) {
        vector<string> row;
        stringstream ss(line);
        string cell;

        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        matrix.push_back(row);
    }


    file.close();
    std::vector<std::vector<int>> output;
    for (const auto& row : matrix) {
        std::vector<int> newRow;
        for (const auto& element : row) {
            int num;
            std::stringstream ss(element);
            ss >> num;
            newRow.push_back(num);
        }
        output.push_back(newRow);
    }

    return output;
}
