#include "ProxyShuffler.h"
#include "Util.h"
#include "debug.h"
#include "Dispatcher.h"
#include "Mutex.h"
#include "Linked_list.h"
#include "Aggregator.h"


#include <boost/enable_shared_from_this.hpp>

#define MSS 1460 //maximum segment size

using namespace boost::asio;

Linked_list *linked_list = new Linked_list();

Mutex buf_mutex;


ProxyShuffler::ProxyShuffler(std::vector <boost::shared_ptr<TunnelStream> > socks,
                             std::vector <boost::shared_ptr<TunnelStream> > relay,
							 nodeType relay_type,
                             boost::asio::io_service::work * wrk)
  : socksList(socks), relayList(relay), closed(false), work(wrk), socksCount(0), relayCount(0),
    relay_pck_cnt(1), socks_pck_cnt(1), relayType(relay_type), remaining_bytes(0)
{


	 remaining_bytes_buf = (unsigned char*) malloc (5) ;

		linked_list->head=NULL;
		linked_list->head_pck=1;
}



void ProxyShuffler::shuffle() {
  std::cerr << "proxy relaying..." << std::endl;


  if (relayType==firstHop) // 1 input multiple outputs
  {
	  remaining_bytes = 0;

	  int next_rel = socks_pck_cnt % relayCount;
	  (socksList[0])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
		                          (socksList[0]), (relayList[next_rel]), socksReadBuffer, true, _1, _2));   //ADD HEADER TO BUFFER BEFORE SENDING/

	  for (int i=0 ; i< relayCount; i++)
	  {
		  (relayList[i])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
		                          (relayList[i]), (socksList[0]), relayReadBuffer, false, _1, _2));  // ADD relayReadBuffer to linked send what's needed
	  }
  }
  else if (relayType==lastHop)  // multiple inputs 1 output
  {
	  for (int i=0 ; i< socksCount; i++)
	  {
		  (socksList[i])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
		                          (socksList[i]), (relayList[0]), socksReadBuffer, true, _1, _2));  // ADD socksREADBUFFER to linked send what's needed
	  }

	  remaining_bytes = 0;

	  int next_socks = relay_pck_cnt % socksCount;

	  (relayList[0])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
		                          (relayList[0]), (socksList[next_socks]), relayReadBuffer, false, _1, _2));   //ADD HEADER TO BUFFER BEFORE SENDING
  }
  else if (relayType == middleHop) //1 input 1 output
  {
      std::cerr << " ======> " << socksList.size()  <<":" << relayList.size() << std::endl;
	  (socksList[0])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
	  		                          (socksList[0]), (relayList[0]), socksReadBuffer, true, _1, _2));
	  (relayList[0])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
	  		                          (relayList[0]), (socksList[0]), relayReadBuffer, false, _1, _2));
  }

}

void ProxyShuffler::addNewSocks(boost::shared_ptr<TunnelStream> socks,
         boost::asio::io_service::work * wrk)
{
	socksList.push_back(socks);
	socksList[socksCount]->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
	                                  (socksList[socksCount]), (relayList[0]), socksReadBuffer, true, _1, _2));
	socksCount++;
}


void ProxyShuffler::readComplete(boost::shared_ptr<TunnelStream> thisStream,
                                 boost::shared_ptr<TunnelStream> thatStream,
                                 unsigned char* thisBuffer, bool inSocks, 
                                 unsigned char* buf, std::size_t transferred) 
{
  if (closed) return;

  if (transferred == -1) {
    close(!inSocks);
#ifdef DEBUG_STREAMID
    std::cerr << " Closing Relay ID ======> " << thisStream->getStreamId() << "\t" << std::endl;
#endif
    return;
  }

  assert(transferred <= PROXY_BUF_SIZE);
  memcpy(thisBuffer, buf, transferred);


#ifdef DEBUG_SHUFFLER
  if(inSocks) {
    std::cerr << " ======> " << transferred << "\tbytes" << std::endl;
  }
  else {
    std::cerr << " <====== " << transferred << "\tbytes" << std::endl;
  }
#endif
  Util::hexDump(thisBuffer,transferred);



  if ((relayType==firstHop && inSocks==true) || (relayType==lastHop && inSocks==false)) // ADD HEADER TO BUFFER BEFORE SENDING
  {
	  int transferred_with_h = transferred + 8;


	  int transferred_with_previous = transferred_with_h + remaining_bytes;


	  unsigned char *mod_buf;

	  int new_remaining_bytes;

	  if (transferred_with_previous>MSS)
	  {
		  mod_buf = (unsigned char*) malloc (MSS-8);
		  new_remaining_bytes = transferred_with_previous - MSS;
		  if (remaining_bytes>0)
		  {
			  memcpy(mod_buf, remaining_bytes_buf, remaining_bytes);
			  free(remaining_bytes_buf);
		  }
		  memcpy(mod_buf + remaining_bytes, thisBuffer, MSS - remaining_bytes - 8);
	  }
	  else
	  {
		  mod_buf = (unsigned char*) malloc (transferred_with_previous-8);
		  new_remaining_bytes = 0;
		  if (remaining_bytes>0)
		  {
			  memcpy(mod_buf, remaining_bytes_buf, remaining_bytes);
			  free(remaining_bytes_buf);
		  }
		  memcpy(mod_buf + remaining_bytes, thisBuffer, transferred);
	  }

	  if (new_remaining_bytes>0)
	  {
		  remaining_bytes_buf = (unsigned char*) realloc (remaining_bytes_buf,new_remaining_bytes);
		  memcpy(remaining_bytes_buf, thisBuffer + MSS - remaining_bytes - 8, new_remaining_bytes);
	  }

	  remaining_bytes = new_remaining_bytes;




	  if (transferred_with_previous>MSS)
	  {
		  unsigned char *new_mod_buf = (unsigned char*) malloc (MSS);

		  if(inSocks == true)
		  {
			  Dispatcher::add_header(mod_buf, MSS-8, socks_pck_cnt++ , new_mod_buf);
		  }
		  else
		  {
			  Dispatcher::add_header(mod_buf, MSS-8, relay_pck_cnt++ , new_mod_buf);
		  }

		  thatStream->write(new_mod_buf, MSS,
		                    boost::bind(&ProxyShuffler::writeComplete, shared_from_this(),
		                                thisStream, thatStream, new_mod_buf, inSocks,
		                                boost::asio::placeholders::error));
	  }
	  else
	  {
		  unsigned char *new_mod_buf = (unsigned char*) malloc (transferred_with_previous);

		  if(inSocks == true)
		  {
			  Dispatcher::add_header(mod_buf, transferred_with_previous-8, socks_pck_cnt++ , new_mod_buf);
		  }
		  else
		  {
			  Dispatcher::add_header(mod_buf, transferred_with_previous-8, relay_pck_cnt++ , new_mod_buf);
		  }


		  thatStream->write(new_mod_buf, transferred_with_previous,
		                    boost::bind(&ProxyShuffler::writeComplete, shared_from_this(),
		                                thisStream, thatStream, new_mod_buf, inSocks,
		                                boost::asio::placeholders::error));
	  }


  }
  else if ((relayType==firstHop && inSocks==false) || (relayType==lastHop && inSocks==true))  // add to linked list
  {

		unsigned int buf_len;
		unsigned int pck_num;
		unsigned char* buf;

		Aggregator::removeHeader(thisBuffer, &buf_len, &pck_num);

		buf = (unsigned char *) malloc (buf_len);
		Aggregator::appendPayload(thisBuffer, buf, &buf_len);


		buf_mutex.lock();


		linked_list->insertNode(buf,buf_len,pck_num);


		while (linked_list->check_head())
		{
			int buf_len = linked_list->getBufLen();
			unsigned char* sentBuf = (unsigned char*) malloc (buf_len);
			unsigned char* tmp =  linked_list->getBuf();
			memcpy(sentBuf, tmp, buf_len);

			//SEND PACKET

			thatStream->write(sentBuf, buf_len,
				                    boost::bind(&ProxyShuffler::writeComplete, shared_from_this(),
				                                thisStream, thatStream, thisBuffer, inSocks,
				                                boost::asio::placeholders::error));
			linked_list->removeNode();
		}

		buf_mutex.unlock();
  }
  else if (relayType == middleHop) //1 input 1 output
  {

	  std::cout << std::endl;
      std::cout << " ======> relaying " << transferred  <<" bytes " << std::endl;

	  thatStream->write(thisBuffer, transferred,
	                    boost::bind(&ProxyShuffler::writeComplete, shared_from_this(),
	                                thisStream, thatStream, thisBuffer, inSocks,
	                                boost::asio::placeholders::error));
  }


}

void ProxyShuffler::writeComplete(boost::shared_ptr<TunnelStream> thisStream,
                                  boost::shared_ptr<TunnelStream> thatStream,
                                  unsigned char *buf, bool inSocks, 
                                  const boost::system::error_code &err)
{
  if (closed) return;

  if (err) {
    std::cerr << "ProxyShuffler::writeComplete error: " << err << std::endl;
    close();
    return;
  }

  if (relayType==firstHop && inSocks==true)   //send next packet to another relay
  {
	  int next_rel = socks_pck_cnt % relayCount;
	  thisStream->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
			  	  	  	  	  	   thisStream, (relayList[next_rel]), buf, inSocks, _1, _2));
  }
  else if (relayType==lastHop && inSocks==false)	//send next packet to another socks (if available)
  {
	  int next_socks = (relay_pck_cnt+1) % socksCount;
	  thisStream->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
			  	  	  	  	  	   thisStream, (socksList[next_socks]), buf, inSocks, _1, _2));

  }
  else
  {
	  thisStream->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
	                               thisStream, thatStream, buf, inSocks, _1, _2));
  }

}

void ProxyShuffler::close(bool force) {
  if (!closed) {
	  for (int i=0 ; i < socksCount ; i++)
		  (relayList[i])->close(force);
	  for (int i=0 ; i < socksCount ; i++)
		  (socksList[i])->close(force);
	  closed = true;
    if (work) {
      delete work;
      work = 0; 
    }
  }
}
