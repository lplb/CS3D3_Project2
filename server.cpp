//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <array>
#include <boost/asio.hpp>
#include "RoutingTable.hpp"
#include "Datagram.hpp"

using boost::asio::ip::udp;

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

int main()
{
  try
  {
    boost::asio::io_service io_service;

    udp::socket socket(io_service, udp::endpoint(udp::v4(), 10001));
    RoutingTable rt;
    rt.init('B', "initFile.txt");
    std::cout << rt.toString();

    for (;;)
    {
      std::array<char, 128> recv_buf;
      udp::endpoint remote_endpoint;
      boost::system::error_code error;
      socket.receive_from(boost::asio::buffer(recv_buf),
          remote_endpoint, 0, error);
      
      std::vector<char> recvVec(128);
      std::copy_n(recv_buf.begin(), 128, recvVec.begin());
      
      Datagram dg;
      dg.consume(recvVec);
      
      rt.update(dg.getSrc(), remote_endpoint.port(), dg.getDV());
      std::cout << rt.toString();


      if (error && error != boost::asio::error::message_size)
        throw boost::system::system_error(error);

      dg.setDV(rt.getDV());
      dg.setSrc('B');
      std::vector<char> toSend = dg.encode();
      std::array<char, 128> send_buf;
      std::copy_n(toSend.begin(), 128, send_buf.begin());

      boost::system::error_code ignored_error;
      socket.send_to(boost::asio::buffer(send_buf),
          remote_endpoint, 0, ignored_error);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
