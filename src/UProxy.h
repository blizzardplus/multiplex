#ifndef UPROXY_H
#define UPROXY_H

/* 
 * This header defines the socks proxy wrapper class
 */

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cassert>
#include <iostream>
#include <string>

#include "Relay.h"
#include "TunnelStream.h"
#include "SocksStream.h"
#include "Packetizer.h"
#include "ProxyShuffler.h"
//#include "Morpher.h"
//#include "Crypto.h"
#include "Util.h"
#include "debug.h"


using namespace boost::asio;

class Proxy {
private:
	int id = 0;
 public:
  ip::tcp::acceptor acceptor;
  io_service *io_srv;
  nodeType relayType;
  std::vector<std::string> *nextHostList;
  std::vector<uint16_t> *nextPortList;
  //Morpher  * _morpher;
  //SkypeClient * skypeclient;

  void acceptIncomingConnection();
  void handleIncomingConnection(boost::shared_ptr<ip::tcp::socket> socket,
                                const boost::system::error_code &err);
  
  void handleSocksRequest(boost::shared_ptr<SocksStream> connection,
                          std::string &host,
                          uint16_t port,
                          const boost::system::error_code &err);

  void handleStreamOpen(boost::shared_ptr<SocksStream> socks,
                        boost::shared_ptr<Packetizer> stream,
                        const boost::system::error_code &err);


 public:
  Proxy(/*UDPRelay *relay, */ boost::asio::io_service &io_service,
        int listenPort/*, Morpher * morpher, SkypeClient * skc*/, nodeType relay_type, std::vector<std::string> *nextHost, std::vector<uint16_t> *nextPort);

    
};

#endif

