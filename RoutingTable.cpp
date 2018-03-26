#include "RoutingTable.hpp"

void RoutingTable::init(char name, std::string intFilePath) {
	std::ifstream ifs(intFilePath);
	std::string line;
	while (std::getline(ifs, line)) {
		if (line[0] == name) {
			Entry entry;
			entry.dest = line[2];
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
		if (newCost < entries[dest].cost ) {
			this->entries[dest].cost = newCost;
			this->entries[dest].nextNode = src;
			this->entries[dest].nextNodePort = srcPort;
		}
	}
}

std::map<char, int> RoutingTable::getDV() {
	std::map<char, int> dv;
	for (std::map<char, Entry>::iterator it = entries.begin(); it != entries.end(); ++it) {
		dv[it->first] = it->second.cost;
	}
	return dv;
}
