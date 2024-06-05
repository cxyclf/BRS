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
Note k of all connected edges of u: {($u$,$v_i$):$1≤i≤k$}, remove these roads from $A$;
And add {($s_i$,$v_i$):$1≤i≤k$} this k path to $A$, where $s_i$ is $k$ vertices but all represent $s$;
Find the connected component of $A$ using the set and record which bus stops are in each connected component.
For each bus stop $s$:
Let s's set of adjacent vertices $N$= empty set
Get all the representatives of s $s_i$, $1≤i≤k$
For each $s_i$, the connected component $A_i$ of $s_i$ is obtained, and the bus stop contained in $A_i$ is added to $N$
$N=N-{s}$
For each bus stop $v$ in $N$, calculate the distance d between $s$ and $v$ in the urban road network, adding edges $(s,v,d)$ to $E$.

Note on the input of the above algorithm: We download the urban road network from OpenStreetMap, including the longitude and latitude of the road apex and the road length. In addition, we climbed the longitude and latitude of the city's bus station from the Autonavi map and projected the bus station onto the nearest road.
