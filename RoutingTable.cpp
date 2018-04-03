#include "RoutingTable.hpp"

void RoutingTable::init(char name, std::string intFilePath) {
	std::ifstream ifs(intFilePath);
	std::string line;
	while (std::getline(ifs, line)) {
		if (line[0] == name) {
			Entry entry;
			entry.nextNode = line[2];
			entry.nextNodePort = std::stoi(line.substr(4, 5));
			entry.cost = std::stod(line.substr(10));
			this->entries[line[2]] = entry;
		}
	}
}

void RoutingTable::update(char src, int srcPort, std::map<char, int> dv) {
	for (std::map<char, int>::iterator it = dv.begin(); it != dv.end(); ++it) {
		char dest = it->first;
	    int newCost = it->second + this->entries[src].cost;
		if (src == this->entries[dest].nextNode) {
		    // if dv is from nextNode, always update cost
		    this->entries[dest].cost = newCost; 
		} else {
		    if (newCost < entries[dest].cost ) {
		        // if new cost is lower than old one, update entry
			    this->entries[dest].cost = newCost;
			    this->entries[dest].nextNode = src;
			    this->entries[dest].nextNodePort = srcPort;
		    }
		}
	}
}

void RoutingTable::setUnreachable(char dest) {
    this->entries[dest].cost = std::numeric_limits<double>::infinity();
}

std::map<char, int> RoutingTable::getDV() {
	std::map<char, int> dv;
	for (std::map<char, Entry>::iterator it = entries.begin(); it != entries.end(); ++it) {
		dv[it->first] = it->second.cost;
	}
	return dv;
}

std::map<char, int> RoutingTable::getNeighbours() {
    std::map<char, int> neighbours;
    for (std::map<char, Entry>::iterator it = entries.begin(); it != entries.end(); ++it) {
		Entry entry = it->second;
		if (it->first == entry.nextNode && entry.cost < std::numeric_limits<double>::infinity()) {
		    neighbours[it->first] = it->second.nextNodePort;
		}
	}
	return neighbours;
}

std::string RoutingTable::toString() {
    std::string str = "Destination, Cost, Next Node, Next Node's Port Number\r\n";
    for (std::map<char, Entry>::iterator it = entries.begin(); it != entries.end(); ++it) {
		Entry entry = it->second;
		str += std::string("") + (it->first) + ", " + std::to_string(entry.cost) + ", " + entry.nextNode + ", " + std::to_string(entry.nextNodePort) + "\r\n";
	}
	return str;
}
