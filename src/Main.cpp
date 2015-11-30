/*
 * Main.cpp
 *
 *  Created on: Nov 19, 2015
 *      Author: hooman
 */


#include "UProxy.h"

int main(int argc, char** argv)
{
	boost::asio::io_service io_service;
	Proxy *proxy = new Proxy( io_service, 5555);


	io_service::work work(io_service);
		io_service.run();
}
