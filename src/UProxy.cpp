#include "UProxy.h"

using namespace boost::asio;
using boost::asio::ip::tcp;


Proxy::Proxy(/*UDPRelay *relay,*/ io_service &io_service, int listenPort/*, Morpher * morpher, SkypeClient * skc*/
        , nodeType relay_type, std::vector<std::string> *next_host, std::vector<uint16_t> *next_port)
  : acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), listenPort)), io_srv (&io_service)/*, relay(relay), _morpher(morpher), skypeclient(skc)*/
    , relayType(relay_type), nextHostList(next_host), nextPortList(next_port)
{
    std::cerr << (int)relayType << std::endl;
  acceptIncomingConnection();
}

void Proxy::acceptIncomingConnection() {
  boost::shared_ptr<ip::tcp::socket> socket(new ip::tcp::socket(acceptor.get_io_service()));
  acceptor.async_accept(*socket, boost::bind(&Proxy::handleIncomingConnection,
					     this, socket, placeholders::error));
}


void Proxy::handleIncomingConnection(boost::shared_ptr<ip::tcp::socket> socket,
					const boost::system::error_code &err) 
{
  if (err) {
    std::cerr << "[proxy] " << "Error accepting incoming connection: " << err << std::endl;
    acceptIncomingConnection();
    return;
  }

  std::cerr << "[proxy] " << "Got SOCKS Connection..." << std::endl;

  boost::shared_ptr<SocksStream> connection(new SocksStream(*io_srv, socket, Util::getRandomId()));
  connection->getRequest(boost::bind(&Proxy::handleSocksRequest, this,
				     connection, _1, _2, _3));

  acceptIncomingConnection();
}

void relayError(const boost::system::error_code &err) {
	std::cerr << "[server] " << "Error with relay, exiting..." << std::endl;
	exit(0);
};


static boost::shared_ptr<ProxyShuffler> liveShuffler;
void Proxy::handleSocksRequest(boost::shared_ptr<SocksStream> connection,
				  std::string &endHost,
				  uint16_t endPort,
				  const boost::system::error_code &err)
{
  std::cerr << "[proxy] " << "Got SOCKS Request: " << endHost << ":" << endPort << std::endl;

  if (err) {
    connection->close();
  }
  else     {
      //boost::shared_ptr<Codec> codec(new Codec(false));
      //skypeclient->runOnce(&host, &port, codec);

      std::cerr << "[proxy] " << "Skype server at " << endHost << ":" << endPort << std::endl;

      //boost::bind(&Proxy::handleStreamOpen, this, connection, _1, _2);

      boost::shared_ptr<Relay> relay(new Relay(*io_srv,
              boost::bind(relayError,
                      placeholders::error)));

      boost::system::error_code error;
      try {

          std::string next_host;
          uint16_t next_port;
          std::vector < boost::shared_ptr<TunnelStream> > rstreamList;
          if (relayType == lastHop )
          {
              if(liveShuffler == NULL)
              {
                  next_host = endHost;
                  next_port = endPort;
                  boost::shared_ptr<TunnelStream> rstream(relay->openStream(next_host, next_port, relayType, endHost, endPort, &error));
                  if (!error)
                  {
                      rstreamList.push_back(rstream);
                  }
              }
          }
          else
          {
              for (int i=0; i<nextHostList->size(); i++)
              {
                  next_host = (*nextHostList)[i];
                  next_port = (*nextPortList)[i];
                  boost::shared_ptr<TunnelStream> rstream(relay->openStream(next_host, next_port, relayType, endHost, endPort, &error));

                  if (!error)
                  {
                      rstreamList.push_back(rstream);
                  }
              }

          }
          connection->respondConnected();

          if(relayType != lastHop || liveShuffler == NULL)
          {

              std::vector <boost::shared_ptr<TunnelStream> > connectionList ;
              connectionList.push_back(connection);
              boost::shared_ptr<ProxyShuffler> proxyShuffler(new ProxyShuffler(connectionList, rstreamList, relayType));
              proxyShuffler->relayCount = rstreamList.size();
              proxyShuffler->socksCount = 1;
              if(relayType == lastHop)
                  liveShuffler = proxyShuffler;
              proxyShuffler->shuffle();
          }
          else
          {
              (liveShuffler)->addNewSocks(connection);
              //(liveShuffler)->shuffle();
          }

      }
      catch (...) {
          std::cerr << "[server] " << " unexpected error. exit now" << std::endl;
      }
  }
}

void Proxy::handleStreamOpen(boost::shared_ptr<SocksStream> socks,
                             boost::shared_ptr<Packetizer> stream,
                             const boost::system::error_code &err)
{
  /*if (err) {
    std::cerr << "[proxy] " << "Error opening stream: " << err << std::endl;
    socks->respondConnectError();
    socks->close();
    return;
  }
  std::cerr << "[proxy] " << "Successfully opened relay..." << std::endl;

  socks->respondConnected();

  boost::shared_ptr<ProxyShuffler> proxyShuffler(new ProxyShuffler(socks, stream));
  //stream->run();
  proxyShuffler->shuffle();
*/}
