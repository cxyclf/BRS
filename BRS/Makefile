# 编译器和选项
CXX = g++
CXXFLAGS = -Wall -Wextra -O3 -fopenmp 

# 目标文件和源文件
TARGET = main
SRCS = adjacency_list.cpp  time.cpp global.cpp passenger.cpp vehicle.cpp HopcroftKarp.cpp update_near_bus.cpp match.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

# TBB 相关路径
TBB_INCLUDE_PATH = /usr/include/tbb
TBB_LIB_PATH = /usr/lib/x86_64-linux-gnu

# 编译规则
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -L$(TBB_LIB_PATH) -ltbb

# 生成目标文件的规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(TBB_INCLUDE_PATH) -c -o $@ $<

# 清理规则
clean:
	rm -f $(OBJS) $(TARGET)# 依赖规则

adjacency_list.o: adjacency_list.cpp adjacency_list.h
time.o:time.h time.cpp
global.o: time.h  global.cpp global.h time.o
passenger.o: global.h  passenger.cpp passenger.h global.o
vehicle.o:  passenger.h time.h global.h  vehicle.cpp vehicle.h   passenger.o  time.o  global.o
HopcroftKarp.o: vehicle.h passenger.h  HopcroftKarp.cpp HopcroftKarp.h vehicle.o passenger.o
update_near_bus.o: vehicle.h passenger.h  update_near_bus.cpp update_near_bus.h vehicle.o passenger.o
match.o: vehicle.h passenger.h global.h match.cpp match.h  vehicle.o passenger.o global.o
main.o: adjacency_list.h  time.h global.h passenger.h vehicle.h HopcroftKarp.h update_near_bus.h match.h main.cpp  vehicle.o passenger.o global.o update_near_bus.o HopcroftKarp.o time.o  adjacency_list.o


