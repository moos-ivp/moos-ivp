/*
 * MulticastListener.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: pnewman
 */
#ifndef _WIN32
#include "unistd.h"
#endif

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include "Listener.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"


namespace MOOS {



Listener::Listener(SafeList<CMOOSMsg> & queue,
		const MOOS::IPV4Address & address,
		bool multicast, const std::vector<std::string> & white_list):
			queue_(queue), address_(address),multicast_(multicast),white_list_(white_list) {	
}

Listener::~Listener() {
}


bool Listener::Run()
{
	thread_.Initialise(dispatch, this);
	return thread_.Start();
}
bool Listener::ListenLoop()
{
	try
	{
		//set up socket....
		int socket_fd;
		socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if(socket_fd<0)
			throw std::runtime_error("Listener::ListenLoop()::socket()");

		//we want to be able to resuse it (multiple folk are interested)
		int reuse = 1;
		if (setsockopt(socket_fd, SOL_SOCKET,SO_REUSEADDR/* SO_REUSEPORT*/, &reuse, sizeof(reuse)) == -1)
			throw std::runtime_error("Listener::ListenLoop::setsockopt::reuse");

/*		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT,
			&reuse, sizeof(reuse)) == -1)
		throw std::runtime_error("Listener::ListenLoop()::failed to set resuse port option");*/


		//give ourselves plenty of receive space
		//set aside some space for receiving - just a few multiples of 64K
		int rx_buffer_size = 64*1024*28;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &rx_buffer_size, sizeof(rx_buffer_size)) == -1)
			throw std::runtime_error("Listener::ListenLoop()::setsockopt::rcvbuf");


		/* construct a datagram address structure */
		struct sockaddr_in dg_addr;
		memset(&dg_addr, 0, sizeof(dg_addr));
		dg_addr.sin_family = AF_INET;
		if(multicast_)
		{
			dg_addr.sin_addr.s_addr = inet_addr(address_.host().c_str());
		}
		else
		{
			dg_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		dg_addr.sin_port = htons(address_.port());

		if (bind(socket_fd, (struct sockaddr*) &dg_addr, sizeof(dg_addr)) == -1)
			throw std::runtime_error("Listener::ListenLoop()::bind");

		if(multicast_)
		{

			//join the multicast group
			struct ip_mreq mreq;
			mreq.imr_multiaddr.s_addr = inet_addr(address_.host().c_str());
			mreq.imr_interface.s_addr = INADDR_ANY;
			if(setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))==-1)
				throw std::runtime_error("Listener::ListenLoop()::setsockopt::ADD_MEMBERSHIP");
		}


		//make a receive buffer
		std::vector<unsigned char > incoming_buffer(2*64*1024);

		while(!thread_.IsQuitRequested())
		{
			//read socket blocking
			int num_bytes_read = read(socket_fd,
					incoming_buffer.data(),
					incoming_buffer.size());

			if(num_bytes_read>0)
			{

				//deserialise
				CMOOSMsg msg;
				msg.Serialize(incoming_buffer.data(), incoming_buffer.size(), false);

				if(IsWanted(msg)){
					//push onto queue
					queue_.Push(msg);
				}
			}

		}
	}
	catch(const std::exception & e)
	{
		std::cerr<<MOOS::ConsoleColours::Red();
		std::cerr<<"Caught exception in a listening thread:\n";
		std::cerr<<"    "<<e.what()<<std::endl;
		std::cerr<<"    "<<std::strerror(errno)<<std::endl;
		std::cerr<<"    input route abandoned ("<<address_.to_string()<<")"<<std::endl;
		std::cerr<<MOOS::ConsoleColours::reset();

		exit(-1);

		return false;
	}

	return true;

}

struct  WhiteListMatcher{
	WhiteListMatcher(const CMOOSMsg & msg ): m_(msg){}
	bool operator()(const std::string & s){
		return MOOSWildCmp(s,m_.GetName());
	}
	const CMOOSMsg & m_;
};

bool Listener::IsWanted(const CMOOSMsg & msg){
	if(white_list_.empty()){
		return true;
	}else{
		WhiteListMatcher matcher(msg);
		return std::any_of(white_list_.begin(),white_list_.end(),matcher);
	}

}



}
