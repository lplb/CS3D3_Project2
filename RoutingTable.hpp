#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include <string>
#include <map>
#include <fstream>
#include <limits>

struct Entry {
	char dest;
	double cost = std::numeric_limits<double>::infinity();
	char nextNode;
	int nextNodePort;
};

class RoutingTable {

	private:
		std::map<char,Entry> entries;

	public:
        void init(char name, std::string intiFilePath);
		void update(char src, int srcPort, std::map<char,int> dv);
		std::map<char,int> getDV();

};


#endif
