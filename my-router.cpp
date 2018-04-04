#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <ctime>
#include <cmath>
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

std::map<char, long> lastTimeReceived;
#define check_time 10

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

void sendDVsToNeighbours(boost::shared_ptr<udp::socket> socketPtr) {
    std::map<char, int> neighbours = rt.getNeighbours();
	for (std::map<char, int>::iterator it = neighbours.begin(); it != neighbours.end(); ++it) {
	    Datagram dg;
	    
	    // lock
	    std::unique_lock<std::mutex> lk(m);
	    //cv.wait(lk);
	    
	    dg.setDV(rt.getDV());
	    
	    // unlock
	    lk.unlock();
	    //cv.notify_all();
	    
        dg.setSrc(name);
        dg.setDest(it->first);
        send(it->second, dg, socketPtr);
	}
}

void genPacket(boost::shared_ptr<udp::socket> socketPtr) {
    std::cout << "To generate and send a packet, first enter the source(capitalized char)\n";
    char src;
    std::cin >> src;
    std::cout << "Now, enter the destination(capitalized character)\n";
    char dest;
    std::cin >> dest;
    std::cout << "Finally, enter the data to send (as a string)\n";
    std::string data;
    std::cin.ignore();
    std::getline(std::cin, data);
    Datagram dg;
    dg.setSrc(src);
    dg.setDest(dest);
    dg.setData(data);
    
    int srcPort = 10000 + src - 'A';
    
    send(srcPort, dg, socketPtr);
    
    std::cout << "Packet sent";
    
    std::ofstream outfile;
    outfile.open(std::string("routing-output") + name + ".txt");
    outfile << "Packet sent from node " << src << " to node " << dest << "containing the following data:\n" << dg.getData();
    
    
}

void checkAliveThread(boost::shared_ptr<udp::socket> socketPtr) {
    for(;;) {
        sleep(1);
        bool newUnreachable = false;
        for (std::map<char, long>::iterator it = lastTimeReceived.begin(); it != lastTimeReceived.end(); ++it) {
            time_t curTime = time(0);
            if (abs(curTime - it->second) > check_time && it->first != name) {
                std::unique_lock<std::mutex> lk(m);
                //cv.wait(lk);
                rt.setUnreachable(it->first);
                lk.unlock();
                //cv.notify_all();
                lastTimeReceived.erase(it);
                newUnreachable = true;
                std::cout << "\n" << it->first << " is unreachable\n";
            }
        }
        if (newUnreachable) {
            sendDVsToNeighbours(socketPtr);
        }
    }
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
        lastTimeReceived[dg.getSrc()] = time(0);

        if (dest != name) {
            std::unique_lock<std::mutex> lk(m);
            //cv.wait(lk);
            int destPort = rt.getPortTwrdsDest(dest);
            lk.unlock();
            //cv.notify_all();
            send(destPort, dg, socketPtr);
            std::cout << "Received packet destined for " << dest << ", resending it through port " << destPort << "\n";
            std::ofstream outfile;
            outfile.open(std::string("routing-output") + name + ".txt", std::ofstream::app);
            outfile << "Received packet destined for " << dest << ", resending it through port " << destPort << "\n";
        } else {
            if (dg.isDV()) {
                //lock
                std::unique_lock<std::mutex> lk(m);
                //cv.wait(lk);

                rt.update(dg.getSrc(), port, dg.getDV());
                //std::cout << rt.toString();

                //unlock
                lk.unlock();
                //cv.notify_all();
            } else {
                std::cout << "Received packet destined for this node (" << dest << ") containing the following data:" << dg.getData() << "\n";
                std::ofstream outfile;
                outfile.open(std::string("routing-output") + name + ".txt", std::ofstream::app);
                outfile << "Received packet destined for this node (" << dest << ") containing the following data:" << dg.getData() << "\n";
            }
        }
    }
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: router <routerName>(char) <port>" << std::endl;
		return 1;
	}

	//initialize
	name = argv[1][0];
	srcPort = std::stoi(argv[2]);
	
	std::cout << "Opening socket for node " << name << " on port " << srcPort << std::endl;
	
    boost::shared_ptr<udp::socket> socketPtr(new udp::socket(io_service, udp::endpoint(udp::v4(), srcPort)));
	
	// if name==G: packet generation. No routing.
	if (name == 'G') {
	    genPacket(socketPtr);
	    return 0;
	}

	std::string file = "initFile.txt";
	if (fileExists(file)) {
		rt.init(name, file);
	}
	
	std::map<char, int> neighbours = rt.getNeighbours();
	for (std::map<char, int>::iterator it = neighbours.begin(); it != neighbours.end(); ++it) {
	    lastTimeReceived[it->first] = time(0);
	}

	//create the receiving thread
	std::thread tr(receiveThread, socketPtr);
    tr.detach();
    
    std::thread tc(checkAliveThread, socketPtr);
    tc.detach();

	//infinite loop
	for (;;) {
		//sleep five seconds
		sleep(5);
		
		sendDVsToNeighbours(socketPtr);
		
	}

	return 0;
}
