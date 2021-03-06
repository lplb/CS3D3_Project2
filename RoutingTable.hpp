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
	int nextNodePort = -1;
};

class RoutingTable {

	private:
	    // map containing the entries used for routing
		std::map<char,Entry> entries;
		std::map<char,int> neighbours;
		std::map<char,double> directCostsToNeighbours;
		char name;
		
		// prints the current time, the routing table's state before change, the distance vector 
		// producing the change and the updated routing table
		void printChange(std::string stateBefore, std::map<char,double> dv, char src, int srcPort);

	public:
	    // initializes the routing table using the input file
        void init(char name, std::string initFilePath);
        
        // updates the routing table with a new distance vector
		void update(char src, int srcPort, std::map<char,double> dv);
		
		// sets the cost to the specified node as infinity
		void setUnreachable(char dest);
		
		// returns a list of the reachable neighbouring nodes' ports
		std::map<char, int> getNeighbours();
		
		// returns the distance vector to send to the other nodes
		std::map<char,double> getDV();
				
		// returns the port used for the next node towards the destination
		int getPortTwrdsDest(char dest);
		
		// returns a string representation of the table's current state
		std::string toString();
};


#endif
