# Bus-Ridesharing-For-Millions-of-Passengers
A Large-Scale Bus Ridesharing Service
 
## Generate bus map：
The vertex set $V$ is the set of bus stops in the city, and the edge set $E$ is composed of any two adjacent bus stops. Here, we define the non-adjacent bus stops as any path between the two stops must pass through at least one bus stop. To get $G$ we need the road network of the city and the location of the various stations on the road. With this information, we can construct the edge set E using the following algorithm.
The algorithm for constructing edge set $E$:
Enter: City road network, the location of each bus stop on the road network.
Output: $E$
Let A= urban road network and $E$= empty set
For each bus stop s:
If it is in the middle of road ($u,v$), then remove road ($u,v$) from A and add two roads ($u$,$s_1$),($s_2$,$v$) to $A$, where $s_1$, $s_2$ both represent $s$;
If s is at a vertex u, then:
Note k of all connected edges of u: {($u$,$v_i$):1≤i≤k}, remove these roads from $A$;
And add {($s_i$,$v_i$):1≤i≤k} this k path to $A$, where $s_i$ is $k$ vertices but all represent $s$;
Find the connected component of $A$ using the set and record which bus stops are in each connected component.
For each bus stop $s$:
Let s's set of adjacent vertices $N$= empty set
Get all the representatives of s $s_i$, $1≤i≤k$
For each $s_i$, the connected component $A_i$ of $s_i$ is obtained, and the bus stop contained in $A_i$ is added to $N$,
$N=N-{s}$
For each bus stop $v$ in $N$, calculate the distance d between $s$ and $v$ in the urban road network, adding edges $(s,v,d)$ to $E$.

Note on the input of the above algorithm: We download the urban road network from OpenStreetMap, including the longitude and latitude of the road apex and the road length. In addition, we climbed the longitude and latitude of the city's bus station from the Autonavi map and projected the bus station onto the nearest road.

If you want to port to another city, you need to download the city's road network from OpenStreetMap and get the latitude and longitude of the bus stops, which are specified in the input code.
## Generate passenger data
After getting the bus map, you can enter the city bus road network according to the a-road format given in the code, you can control how the passenger order is generated according to your own Settings, the specific parameter modification has an interface, of course, you can also add or delete some variables according to your own needs.
## BRS
To run the program, you need to download the BRS_data in tag, extract it and put it in the same folder as the program. The code provides the global.cpp interface, where you can adjust various parameters, such as time interval, SNOL,SNDL, bus speed, bus number, and the date to be simulated. The default code is to simulate 2016.11.19 (please note the timestamp modification when using other passenger data sets), the code is run in the g++ environment of ubuntu 20.04 (g++ version should not be less than 11), if parallel processing is required, please install OpenMP and TBB in advance.
