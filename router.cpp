#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "RoutingTable.hpp"

using boost::asio::ip::udp;

RoutingTable rt;
char name;
int port;

/**
* Source for this method: https://techoverflow.net/2013/08/21/how-to-check-if-file-exists-using-stat/?q=/blog/2013/08/21/how-to-check-if-file-exists-using-stat/
*
* Check if a file exists
* @return true if and only if the file exists, false else
*/
bool fileExists(const std::string& file) {
	struct stat buf;
	return (stat(file.c_str(), &buf) == 0);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: router <routerName>(char) <port>" << std::endl;
		return 1;
	}

	//initialize
	name = argv[1][0];
	port = std::stoi(argv[2]);

	std::string file = "initFile.txt";
	if (fileExists(file)) {
		rt.init(name, file);
	}

	//create the threads for sending and receiving
	//...

	//infinite loop
	for (;;) {
		//do we need something here?
	}

	return 0;
}
