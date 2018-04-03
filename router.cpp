#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "RoutingTable.hpp"
#include "Datagram.hpp"

using boost::asio::ip::udp;

RoutingTable rt;
char name;
int srcPort;

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


void receiveThread() {
    boost::asio::io_service io_service;

    udp::socket socket(io_service, udp::endpoint(udp::v4(), srcPort));
    
    for (;;) {
          std::array<char, 128> recv_buf;
          udp::endpoint remote_endpoint;
          boost::system::error_code error;
          socket.receive_from(boost::asio::buffer(recv_buf),
              remote_endpoint, 0, error);
          
          std::vector<char> recvVec(128);
          std::copy_n(recv_buf.begin(), 128, recvVec.begin());
          
          Datagram dg;
          dg.consume(recvVec);
          
          //lock
          rt.update(dg.getSrc(), remote_endpoint.port(), dg.getDV());
          //unlock


          //if (error && error != boost::asio::error::message_size)
            //throw boost::system::system_error(error);
        }
}

void send(int destPort, Datagram dg) {
    boost::asio::io_service io_service;

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), "localhost", std::to_string(destPort));
    udp::endpoint receiver_endpoint = *resolver.resolve(query);

    udp::socket socket(io_service, udp::endpoint(udp::v4(), srcPort));
    
    std::vector<char> toSend = dg.encode();
    std::array<char, 128> send_buf;
    std::copy_n(toSend.begin(), 128, send_buf.begin());
    
    socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: router <routerName>(char) <port>" << std::endl;
		return 1;
	}

	//initialize
	name = argv[1][0];
	srcPort = std::stoi(argv[2]);

	std::string file = "initFile.txt";
	if (fileExists(file)) {
		rt.init(name, file);
	}

	//create the receiving thread
	std::thread t(receiveThread);
    t.detach();

	//infinite loop
	for (;;) {
		//sleep
		
		std::map<char, int> neighbours = rt.getNeighbours();
		for (std::map<char, int>::iterator it = neighbours.begin(); it != neighbours.end(); ++it) {
		    Datagram dg;
		    // MUTEX LOCK / cond var
		    dg.setDV(rt.getDV());
		    // unlock
            dg.setSrc(name);
            dg.setDest(it->first);
            send(it->second, dg);
		}
		
	}

	return 0;
}
