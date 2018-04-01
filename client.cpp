//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <vector>
#include "RoutingTable.hpp"
#include "Datagram.hpp"

using boost::asio::ip::udp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 4)
    {
      std::cerr << "Usage: client <host> <nodeName> <port>" << std::endl;
      return 1;
    }
    
    RoutingTable rt;
    rt.init(argv[2][0], "initFile.txt");
    
    std::cout << rt.toString();

    boost::asio::io_service io_service;

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), argv[1], "10001");
    udp::endpoint receiver_endpoint = *resolver.resolve(query);

    udp::socket socket(io_service, udp::endpoint(udp::v4(), std::stoi(argv[3])));
    
    Datagram dg;
    dg.setDV(rt.getDV());
    dg.setSrc(argv[2][0]);
    std::vector<char> toSend = dg.encode();
    std::array<char, 128> send_buf;
    std::copy_n(toSend.begin(), 128, send_buf.begin());
    
    socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

    std::cout << "sent\n";

    std::array<char, 128> recv_buf;
    udp::endpoint sender_endpoint;
    socket.receive_from(
        boost::asio::buffer(recv_buf), sender_endpoint);

    std::vector<char> recvVec(128);
    std::cout << "afasf\r\n";
    std::copy_n(recv_buf.begin(), 128, recvVec.begin());
      
    Datagram dg2;
    dg2.consume(recvVec);
      
    rt.update(dg2.getSrc(), sender_endpoint.port(), dg2.getDV());
    std::cout << rt.toString();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
