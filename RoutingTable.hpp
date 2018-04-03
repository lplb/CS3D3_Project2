#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <limits>
#include <ctime>

struct Entry {
	double cost = std::numeric_limits<double>::infinity();
	char nextNode;
	int nextNodePort;
};

class RoutingTable {

	private:
		std::map<char,Entry> entries;

	public:
	    // initializes the routing table using the input file
        void init(char name, std::string intiFilePath);
        
        // updates the routing table with a new distance vector
		void update(char src, int srcPort, std::map<char,double> dv);
		
		// sets the cost to the specified node as infinity
		void setUnreachable(char dest);
		
		// returns a list of the reachable neighbouring nodes' ports
		std::map<char, int> getNeighbours();
		
		// returns the distance vector to send to the other nodes
		std::map<char,double> getDV();
		
		// prints the current time, the routing table's state before change, the distance vector 
		// producing the change and the updated routing table
		void printChange(std::string stateBefore, std::map<char,double> dv, char src, int srcPort);
		
		// returns a string representation of the table's current state
		std::string toString();

};


#endif
