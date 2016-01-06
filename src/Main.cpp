/*
 * Main.cpp
 *
 *  Created on: Nov 19, 2015
 *      Author: hooman
 */


#include "UProxy.h"


#define FIRST_HOP_IP "127.0.0.1"
#define FIRST_HOP_PORT 5555

#define SECOND1_HOP_IP "127.0.0.1"
#define SECOND1_HOP_PORT 5556

#define SECOND2_HOP_IP "127.0.0.1"
#define SECOND2_HOP_PORT 5557

#define THIRD_HOP_IP "127.0.0.1"
#define THIRD_HOP_PORT 5558

#define FLOW_NUM 2

int main(int argc, char** argv)
{
    boost::asio::io_service io_service;

    //First Hop
    std::vector<std::string> secondHopIP;
    secondHopIP.push_back(SECOND1_HOP_IP);
    secondHopIP.push_back(SECOND2_HOP_IP);

    uint16_t secHopPort[] = {SECOND1_HOP_PORT,SECOND2_HOP_PORT};
    std::vector<uint16_t> secondHopPort(secHopPort, secHopPort+2); // definition

    //Second Hop
    std::vector<std::string> thirdHopIP;
    thirdHopIP.push_back(THIRD_HOP_IP);

    uint16_t thHopPort[] = {THIRD_HOP_PORT};
    std::vector<uint16_t> thirdHopPort(thHopPort, thHopPort+2); // definition

    //Third hop
    std::vector<std::string> fourHopIP;
    fourHopIP.push_back("");

    uint16_t fHopPort[] = {0};
    std::vector<uint16_t> fourHopPort(fHopPort, fHopPort+1); // definition


    if (argc <=1)
    {
        std::cerr << "Invalid input number" << std::endl;
    }
    else if (strncmp(argv[1],"1", 1) == 0)
    {
        std::cout << "Starting with 1" << std::endl;
        Proxy *proxy = new Proxy( io_service, FIRST_HOP_PORT, firstHop, &secondHopIP, &secondHopPort);
    }
    else if (strncmp(argv[1],"2", 1) == 0)
    {
        std::cout << "Starting with 2" << std::endl;
        Proxy *proxy = new Proxy( io_service, SECOND1_HOP_PORT, middleHop, &thirdHopIP, &thirdHopPort);
    }
    else if (strncmp(argv[1],"3", 1) == 0)
    {
        std::cout << "Starting with 3" << std::endl;
        Proxy *proxy = new Proxy( io_service, SECOND2_HOP_PORT, middleHop, &thirdHopIP, &thirdHopPort);
    }
    else if (strncmp(argv[1],"4", 1) == 0)
    {
        std::cout << "Starting with 4" << std::endl;
        Proxy *proxy = new Proxy( io_service, THIRD_HOP_PORT, lastHop, &fourHopIP, &fourHopPort);
    }

	io_service::work work(io_service);
		io_service.run();
}
