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
	Entry thisEntry;
	thisEntry.nextNode = name;
	thisEntry.nextNodePort = -1;
	thisEntry.cost = 0;
	this->entries[name] = thisEntry;
}

void RoutingTable::update(char src, int srcPort, std::map<char, double> dv) {
    std::string stateBefore = this->toString();
    
    bool changed = false;
    
	for (std::map<char, double>::iterator it = dv.begin(); it != dv.end(); ++it) {
		char dest = it->first;
		double newCost;
		
		if (it->second >= std::numeric_limits<double>::infinity())
		    newCost = std::numeric_limits<double>::infinity();
		else
    	    newCost = it->second + this->entries[src].cost;
		
		if (src == this->entries[dest].nextNode && newCost != this->entries[dest].cost) {
		    // if dv is from nextNode, always update cost
		    this->entries[dest].cost = newCost;
		    changed = true;
		} else {
		    if (newCost < entries[dest].cost ) {
		        // if new cost is lower than old one, update entry
			    this->entries[dest].cost = newCost;
			    this->entries[dest].nextNode = src;
			    this->entries[dest].nextNodePort = srcPort;
			    changed = true;
		    }
		}
	}
	
	if (changed) {
	    printChange(stateBefore, dv, src, srcPort);
	}
}

void RoutingTable::setUnreachable(char dest) {
    this->entries[dest].cost = std::numeric_limits<double>::infinity();
}

std::map<char, double> RoutingTable::getDV() {
	std::map<char, double> dv;
	for (std::map<char, Entry>::iterator it = entries.begin(); it != entries.end(); ++it) {
		dv[it->first] = it->second.cost;
	}
	return dv;
}

std::map<char, int> RoutingTable::getNeighbours() {
    std::map<char, int> neighbours;
    for (std::map<char, Entry>::iterator it = entries.begin(); it != entries.end(); ++it) {
		Entry entry = it->second;
		if (it->first == entry.nextNode && entry.cost < std::numeric_limits<double>::infinity() && entry.nextNodePort != -1) {
		    neighbours[it->first] = it->second.nextNodePort;
		}
	}
	return neighbours;
}

void RoutingTable::printChange(std::string stateBefore, std::map<char,double> dv, char src, int srcPort) {
    std::time_t now = std::time(0);
    std::string strdv = "Destination: Cost\n";
    for (std::map<char, double>::iterator it = dv.begin(); it != dv.end(); ++it) {
        strdv+= std::string("") + it->first + ":" + std::to_string(it->second) + "\n";
    }
    
    std::cout << std::ctime(&now) << ":\nRouting table before modification:\n" << stateBefore 
        << "\nDistance vector received from node: " << src << " via port " << std::to_string(srcPort) << ":\n"
        << strdv << "\nRouting table after modification:\n" << this->toString() << "\n";
}

std::string RoutingTable::toString() {
    std::string str = "Destination, Cost, Next Node, Next Node's Port Number\r\n";
    for (std::map<char, Entry>::iterator it = entries.begin(); it != entries.end(); ++it) {
		Entry entry = it->second;
		str += std::string("") + (it->first) + ", " + std::to_string(entry.cost) + ", " + entry.nextNode + ", " + std::to_string(entry.nextNodePort) + "\r\n";
	}
	return str;
}
