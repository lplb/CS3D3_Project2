#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "RoutingTable.hpp"
#include "Datagram.hpp"

using boost::asio::ip::udp;

RoutingTable rt;
char name;
int srcPort;
boost::asio::io_service io_service;
std::mutex m;
std::condition_variable cv;

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

void packetGenThread(boost::shared_ptr<udp::socket> socketPtr) {
    std::cout << "To generate and send a packet, enter the destination (single capitalized character)";
    char dest;
    std::cin >> dest;
    std::cout << "Enter the data to send (as a string)";
    std::string data;
    std::scin >> data;
    DG dg;
    dg.setSrc(name);
    dg.setDest(dest);
    dg.setData(data);
    
    std::unique_lock<std::mutex> lk(m);
    int destPort = rt.getPortTwrdsDest(dest);
    lk.unlock();
    
    send(destPort, dg, socketPtr);
    
    std::cout << "Packet sent";
}

void receiveThread(boost::shared_ptr<udp::socket> socketPtr) {

    //udp::socket socket(io_service, udp::endpoint(udp::v4(), srcPort));

    for (;;) {
        std::array<char, 128> recv_buf;
        udp::endpoint remote_endpoint;
        boost::system::error_code error;
        socketPtr.get()->receive_from(boost::asio::buffer(recv_buf),
        remote_endpoint, 0, error);
        int port = remote_endpoint.port();

        std::vector<char> recvVec(128);
        std::copy_n(recv_buf.begin(), 128, recvVec.begin());

        Datagram dg;
        dg.consume(recvVec);

        char dest = dg.getDest();

        if (dest != name) {
            std::unique_lock<std::mutex> lk(m);
            int destPort = rt.getPortTwrdsDest(dest);
            lk.unlock();
            send(destPort, dg, socketPtr);
        } else {
            if (dg.isDV()) {
                //lock
                std::unique_lock<std::mutex> lk(m);

                rt.update(dg.getSrc(), port, dg.getDV());
                //std::cout << rt.toString();

                //unlock
                lk.unlock();
            }
        }
    }
}

void send(int destPort, Datagram dg, boost::shared_ptr<udp::socket> socketPtr) {
    //boost::asio::io_service io_service;

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), "localhost", std::to_string(destPort));
    udp::endpoint receiver_endpoint = *resolver.resolve(query);

    //udp::socket socket(io_service, udp::endpoint(udp::v4(), srcPort));
    
    std::vector<char> toSend = dg.encode();
    std::array<char, 128> send_buf;
    std::copy_n(toSend.begin(), 128, send_buf.begin());
    
    socketPtr.get()->send_to(boost::asio::buffer(send_buf), receiver_endpoint);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: router <routerName>(char) <port>" << std::endl;
		return 1;
	}

	//initialize
	name = argv[1][0];
	srcPort = std::stoi(argv[2]);
	
	//udp::acceptor acceptor(io_service);
    //boost::asio::socket_base::reuse_address option(true);
    //acceptor.set_option(option);
    
	//udp::socket socket(io_service, udp::endpoint(udp::v4(), srcPort));
	
    boost::shared_ptr<udp::socket> socketPtr(new udp::socket(io_service, udp::endpoint(udp::v4(), srcPort)));
	
	
	//socketPtr = std::make_shared<udp::socket>(socket);
	
	

	std::string file = "initFile.txt";
	if (fileExists(file)) {
		rt.init(name, file);
	}

	//create the receiving thread
	std::thread t(receiveThread, socketPtr);
    t.detach();

	//infinite loop
	for (;;) {
		//sleep one second
		sleep(1);
		
		std::map<char, int> neighbours = rt.getNeighbours();
		for (std::map<char, int>::iterator it = neighbours.begin(); it != neighbours.end(); ++it) {
		    Datagram dg;
		    
		    // MUTEX LOCK / cond var
		    std::unique_lock<std::mutex> lk(m);
		    
		    dg.setDV(rt.getDV());
		    
		    // unlock
		    lk.unlock();
		    
            dg.setSrc(name);
            dg.setDest(it->first);
            send(it->second, dg, socketPtr);
		}
		
	}

	return 0;
}
