#include "ProxyShuffler.h"
#include "Util.h"
#include "debug.h"
#include "Dispatcher.h"
#include "Mutex.h"
#include "Linked_list.h"
#include "Thread_pool.h"
#include "Aggregator.h"


#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;

Linked_list *linked_list = new Linked_list();  // need to be changed!! do not use globals
//Mutex *buf_mutex_ptr = new Mutex();

Mutex buf_mutex; //= *buf_mutex_ptr;


ProxyShuffler::ProxyShuffler(std::vector <boost::shared_ptr<TunnelStream> > socks,
                             std::vector <boost::shared_ptr<TunnelStream> > relay,
							 nodeType relay_type,
                             boost::asio::io_service::work * wrk)
  : socksList(socks), relayList(relay), closed(false), work(wrk), socksCount(0), relayCount(0),
    relay_pck_cnt(1), socks_pck_cnt(1), relayType(relay_type)
{
	  /*Thread_pool tp(4);
	  int ret = tp.initialize_threadpool();
	  if (ret == -1) {
	    cerr << "Failed to initialize thread pool!" << endl;
	  } */
		linked_list->head=NULL;
		linked_list->head_pck=1;  // could be added to the constructor
}


/*
void process(void* arg)
{
	unsigned char* in_buf = (unsigned char*) arg;
	unsigned int buf_len;
	unsigned int pck_num;
	unsigned char* buf;

	Aggregator::removeHeader(in_buf, &buf_len, &pck_num);

	buf = (unsigned char *) malloc (buf_len);
	Aggregator::appendPayload(in_buf, buf, &buf_len);


	buf_mutex.lock();

	//std::cout << "REMOVING HEADERS" << buf_len <<  pck_num  << std::endl;
	//std::cout << buf[0] <<  buf[1]  << std::endl;

	linked_list->insertNode(buf,buf_len,pck_num);


	//std::cout <<  linked_list->head_pck << std::endl;


	std::cout <<  linked_list->head->pck_num << std::endl;
	if (linked_list->head->next!=NULL)
	{
		std::cout <<  linked_list->head->next->pck_num << std::endl;
		if (linked_list->head->next->next!=NULL)
		{
			std::cout <<  linked_list->head->next->next->pck_num << std::endl;
		}
	}


	buf_mutex.unlock();


}
*/


void ProxyShuffler::shuffle() {
  std::cerr << "proxy relaying..." << std::endl;


 // int k = pck_cnt % relayCount;

  //addHeader(socksReadBuffer, socksReadBuffer_with_header)

 /*
  (socksList[0])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
		  (socksList[0]), (relayList)[k], socksReadBuffer, true,  _1, _2));



  for (int i=0 ; i< relayCount; i++)
  {
	  (relayList[i])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
	                          (relayList[i]), (socksList[0]), relayReadBuffer, false, _1, _2));
  }
*/

  if (relayType==firstHop) // 1 input multiple outputs
  {

	  int next_rel = socks_pck_cnt % relayCount;
	  (socksList[0])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
		                          (socksList[0]), (relayList[next_rel]), socksReadBuffer, true, _1, _2));   //ADD HEADER TO BUFFER BEFORE SENDING/

	  for (int i=0 ; i< relayCount; i++)
	  {
		  (relayList[i])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
		                          (relayList[i]), (socksList[0]), relayReadBuffer, false, _1, _2));  // ADD socksREADBUFFER ro linked send whats needed
	  }
  }
  else if (relayType==lastHop)  // multiple inputs 1 output
  {
	  for (int i=0 ; i< socksCount; i++)
	  {
		  (socksList[i])->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
		                          (socksList[i]), (relayList[0]), socksReadBuffer, true, _1, _2));  // ADD socksREADBUFFER ro linked send whats needed
	  }

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
	                                  (socksList[socksCount]), (relayList[0]), socksReadBuffer, true, _1, _2));  // ADD socksREADBUFFER ro linked send whats needed
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
  memcpy(thisBuffer, buf, transferred);  // instead of this BUFFER copy to the linked list
  //Decide whether you have data to write to the other side



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
	  unsigned char *mod_buf = (unsigned char*) malloc (transferred_with_h);


	  if(inSocks == true)
	      Dispatcher::add_header(thisBuffer, transferred, socks_pck_cnt++ , mod_buf);
	  else
	      Dispatcher::add_header(thisBuffer, transferred, relay_pck_cnt++ , mod_buf);

	  thatStream->write(mod_buf, transferred_with_h,
	                    boost::bind(&ProxyShuffler::writeComplete, shared_from_this(),
	                                thisStream, thatStream, mod_buf, inSocks,
	                                boost::asio::placeholders::error));
  }
  else if ((relayType==firstHop && inSocks==false) || (relayType==lastHop && inSocks==true))  // multiple inputs 1 output
  {
	  //Task* t1 = new Task(&process, (void*) thisBuffer);
	  //tp.add_task(t1);


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
	  thatStream->write(thisBuffer, transferred,
	                    boost::bind(&ProxyShuffler::writeComplete, shared_from_this(),
	                                thisStream, thatStream, thisBuffer, inSocks,
	                                boost::asio::placeholders::error));
  }

  /*
  thatStream->write(thisBuffer, transferred,
                    boost::bind(&ProxyShuffler::writeComplete, shared_from_this(),
                                thisStream, thatStream, thisBuffer, inSocks, 
                                placeholders::error));
                                */

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

  thisStream->read(boost::bind(&ProxyShuffler::readComplete, shared_from_this(),
                               thisStream, thatStream, buf, inSocks, _1, _2));
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
