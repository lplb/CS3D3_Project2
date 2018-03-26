#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include <string>
#include <map>
#include <ifstream>
#include <stoi>

struct Entry {
	char dest;
	double cost = INFINITY;
	char nextNode;
	int nextNodePort;
};

class RoutingTable {

	private:
		std::map<char,Entry> entries = new std::map();

	public:
        void init(char name, std::string intiFilePath);
		void update(char src, int srcPort, std::map<char,int> dv);
		std::map<char,int> getDV();

};


#endif