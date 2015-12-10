/*
 * Main.cpp
 *
 *  Created on: Nov 19, 2015
 *      Author: hooman
 */


#include "UProxy.h"


#define FIRST_HOP_IP 127.0.0.1
#define FIRST_HOP_PORT 5555

#define SECOND_HOP_IP "127.0.0.1"
#define SECOND_HOP_PORT 5556

#define THIRD_HOP_IP "127.0.0.1"
#define THIRD_HOP_PORT 5557

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    if (argc <=1)
    {
        std::cerr << "Invalid input number" << std::endl;
    }
    else if (strncmp(argv[1],"1", 1) == 0)
    {
        std::cout << "Starting with 1" << std::endl;
        Proxy *proxy = new Proxy( io_service, FIRST_HOP_PORT, false, SECOND_HOP_IP, SECOND_HOP_PORT);
    }
    else if (strncmp(argv[1],"2", 1) == 0)
    {
        std::cout << "Starting with 2" << std::endl;
        Proxy *proxy = new Proxy( io_service, SECOND_HOP_PORT, false, THIRD_HOP_IP, THIRD_HOP_PORT);
    }
    else if (strncmp(argv[1],"3", 1) == 0)
    {
        std::cout << "Starting with 3" << std::endl;
        Proxy *proxy = new Proxy( io_service, THIRD_HOP_PORT, true, "", 0);
    }

	io_service::work work(io_service);
		io_service.run();
}
