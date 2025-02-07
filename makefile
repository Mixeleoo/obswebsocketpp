
all:
	g++ -std=c++17 -Iexternal main.cpp -o app -lboost_system -lboost_thread -lpthread -ljsoncpp
