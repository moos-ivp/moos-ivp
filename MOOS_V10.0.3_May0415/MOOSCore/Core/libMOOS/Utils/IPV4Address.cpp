/**
///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of 
//   Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and 
//   the University of Oxford 2003-2013 
//   
//   email: pnewman@robots.ox.ac.uk. 
//              
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/




/*
 * IPV4Address.cpp
 *
 *  Created on: Sep 1, 2012
 *      Author: pnewman
 */

#ifdef UNIX
	#include <ifaddrs.h>
	#include <arpa/inet.h>
	#include <netdb.h> 


#elif _WIN32
    #include <winsock2.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include "MOOS/libMOOS/Utils/IPV4Address.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"


namespace MOOS {

IPV4Address::IPV4Address() {
	// TODO Auto-generated constructor stub

}

IPV4Address::~IPV4Address() {
	// TODO Auto-generated destructor stub
}

IPV4Address::IPV4Address(const std::string & host, uint16_t p):host_(host),port_(p)
{

};
IPV4Address::IPV4Address(const std::string & host_and_port)
{
	std::string tmp(host_and_port);
	if(tmp.find(':')==std::string::npos)
		throw std::runtime_error("IPV4Address::IPV4Address "+tmp+" is not of host:port format");

	host_ = MOOS::Chomp(tmp,":");

	if(host_.empty() || tmp.empty() )
		throw std::runtime_error("IPV4Address::IPV4Address "+tmp+" is not of host:port format");

	if(!MOOSIsNumeric(tmp))
		throw std::runtime_error("IPV4Address::IPV4Address "+tmp+" is not of host:port format");

	port_ = atoi(tmp.c_str());

}

bool IPV4Address::operator==(const IPV4Address & a) const
{
	return host_==a.host() && port_==a.port();
}


void IPV4Address::set_host(const std::string & host)
{
	host_=host;
}

void IPV4Address::set_port(uint16_t port)
{
	port_=port;
}

std::string IPV4Address::host() const
{
	return host_;
}

uint16_t IPV4Address::port() const
{
	return port_;
}

bool IPV4Address::operator<(const IPV4Address & P) const
{
	if (host_<P.host_)
		return true;
	if(host_==P.host_)
	{
		if(port_<P.port_)
			return true;
	}

	return false;
}
std::string IPV4Address::to_string() const
{
	std::stringstream ss;
	ss<<host_<<":"<<port_;
	return ss.str();
}

std::string IPV4Address::GetNumericAddress(const std::string & address)
{

	if(address.find_first_not_of("0123456789. ")==std::string::npos)
		return address;

	struct hostent *hp  = gethostbyname(address.c_str());

	if(hp==NULL)
		throw std::runtime_error("failed name lookup on "+address);

	if(hp->h_addr_list[0]==NULL)
		throw std::runtime_error("no address returned for  "+address);

	return std::string(inet_ntoa( *(struct in_addr *) hp->h_addr_list[0]));


}

std::string IPV4Address::GetIPAddress()
{
#ifdef _MSC_VER
    return "127.0.0.1";
#else
    struct ifaddrs *ifaddr, *ifa;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
      return "127.0.0.1";
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        int family = ifa->ifa_addr->sa_family;

        if (family == AF_INET)
        {

            if(getnameinfo(
                    ifa->ifa_addr,
                    sizeof(struct sockaddr_in),
                    host,
                    NI_MAXHOST,
                    NULL,
                    0,
                    NI_NUMERICHOST)==0)
            {
                std::string sHost(host);
                if(sHost.find("127.0.0.1")==std::string::npos)
                {
                    return  sHost;
                }
            }

        }
    }

    freeifaddrs(ifaddr);

    return "127.0.0.1";
#endif
}

bool IPV4Address::ConvertHostToNumeric()
{
	try
	{
		host_ = GetNumericAddress(host_);
	}
	catch(const std::runtime_error & e)
	{
		std::cerr<<"IPV4Address::ConvertHostToNumeric issue"<<e.what()<<std::endl;
		return false;
	}
	return true;
}

}
