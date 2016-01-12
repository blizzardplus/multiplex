#ifndef PROXYSHUFFLER_H
#define PROXYSHUFFLER_H

/* 
 * This header defines the proxy shuffler class
 * which handles data transfer between SOCKS and
 * relay!
 */

#include "TunnelStream.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <cassert>
#include <iostream>
#include <string>

#include "Packet.h"

#define PROXY_BUF_SIZE MAX_RAW_MSG_LEN


class ProxyShuffler : public boost::enable_shared_from_this<ProxyShuffler> {

private:

	nodeType relayType;
  std::vector <boost::shared_ptr<TunnelStream> > socksList;
  std::vector <boost::shared_ptr<TunnelStream> > relayList;

  unsigned char socksReadBuffer[PROXY_BUF_SIZE];
  unsigned char relayReadBuffer[PROXY_BUF_SIZE];

  bool closed;
  boost::asio::io_service::work * work; 

  void readComplete(boost::shared_ptr<TunnelStream> thisStream,
                    boost::shared_ptr<TunnelStream> thatStream,
                    unsigned char* thisBuffer, bool inSocks, 
                    unsigned char* buf, std::size_t transferred);

  void writeComplete(boost::shared_ptr<TunnelStream> thisStream,
                     boost::shared_ptr<TunnelStream> thatStream,
                     unsigned char *buf, bool inSocks, 
                     const boost::system::error_code &err);

public:

	uint16_t relayCount, socksCount;
	int socks_pck_cnt, relay_pck_cnt;

	unsigned char* remaining_bytes_buf;
	int remaining_bytes;

 ProxyShuffler(std::vector <boost::shared_ptr<TunnelStream> > socks,
               std::vector <boost::shared_ptr<TunnelStream> > relay,
			   nodeType relay_type,
               boost::asio::io_service::work * wrk = 0);

 void addNewSocks(boost::shared_ptr<TunnelStream> socks,
         boost::asio::io_service::work * wrk = 0);

 void shuffle();

 void close(bool force = false);
  
};

#endif

