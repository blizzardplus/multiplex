#ifndef TUNNELSTREAM_H
#define TUNNELSTREAM_H

/* 
 * This header defines an interface class TunnelStream, which is the 
 * base class of incoming / outgoing stream connections. 
 */


#include <boost/asio.hpp>
#include <boost/function.hpp>

enum nodeType
{
    firstHop,
    middleHop,
    lastHop
};

typedef boost::function<void (const boost::system::error_code &error)> StreamWriteHandler;

typedef boost::function<void (unsigned char* buf, int read)> StreamReadHandler;

class TunnelStream {

 public:
  virtual void read(StreamReadHandler handler) = 0;
  virtual void write(unsigned char* buf, int length, StreamWriteHandler handler) = 0;
  virtual void close(bool force) = 0;
  uint16_t getStreamId()
  {
      return streamId;
  }
 protected:
  uint16_t streamId; //TODO:check for duplicate IDs
};

#endif


