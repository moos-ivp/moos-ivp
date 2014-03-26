/*
 * FullDuplexUDPChannel.cpp
 *
 *  Created on: Mar 1, 2014
 *      Author: pnewman
 */


#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <stdexcept>



#include "MOOS/libMOOS/Utils/CommsTools.h"
#include "MOOS/libMOOS/Comms/MulticastNode.h"


namespace MOOS {

MulticastNode::MulticastNode() {
    // TODO Auto-generated constructor stub
    hops_=1;
    unread_limit_=20;
}

MulticastNode::~MulticastNode() {
    // TODO Auto-generated destructor stub
    write_thread_.Stop();
    read_thread_.Stop();
}

bool rd_dispatch(void * pParam)
{
    MulticastNode * pMe = (MulticastNode*)pParam;
    return pMe->ReadLoop();
}
bool wt_dispatch(void * pParam)
{
    MulticastNode * pMe = (MulticastNode*)pParam;
    return pMe->WriteLoop();
}



bool MulticastNode::Configure(const std::string & address, int port, int hops)
{
    ipv4_address_=MOOS::IPV4Address(address,port);
    hops_=hops;

    read_thread_.Initialise(rd_dispatch,this);
    write_thread_.Initialise(wt_dispatch,this);


    return true;
}
bool MulticastNode::SetUnreadLimit(unsigned int limit)
{
    unread_limit_=limit;

    return true;
}
bool MulticastNode::Run(bool run_write,bool run_read)
{
    bool bSuccess=true;
    if(run_read)
        bSuccess&=read_thread_.Start();
    if(run_write)
        bSuccess&=write_thread_.Start();

    return bSuccess;
}

bool MulticastNode::Read(std::string &data,int  timeout_ms)
{
    std::vector<unsigned char> v;
    if(!Read(v,timeout_ms))
        return false;

    data = std::string(v.begin(),v.end());
    return true;
}
bool MulticastNode::Write(const std::string & data)
{
    //std::vector<unsigned char> v(data.size());
    //std::copy(data.begin(),data.end(),v.begin());
    std::vector<unsigned char> v(data.begin(),data.end());
    return Write(v);

}


bool MulticastNode::Read(std::vector<unsigned char > & data,int timeout_ms)
{
    if(!inbox_.IsEmpty())
    {
        return inbox_.Pull(data);
    }

    if(inbox_.WaitForPush(timeout_ms) )
    {
        return inbox_.Pull(data);
    }
    else
    {
        return false;
    }
}

bool MulticastNode::Write( std::vector<unsigned char > & data)
{
    return outbox_.Push(data);
}

bool MulticastNode::ReadLoop()
{

    try
    {
        //set up a simply DGRAM socket
        int socket_rx_ = socket(AF_INET, SOCK_DGRAM, 0);
        if(socket_rx_<0)
        {
            throw std::runtime_error("FullDuplexUDPChannel::WriteLoop()::socket()");
        }

        //we want to be able to resuse it (multiple folk are interested)
        int reuse = 1;
        if (setsockopt(socket_rx_, SOL_SOCKET,SO_REUSEADDR/* SO_REUSEPORT*/, &reuse, sizeof(reuse)) == -1)
        {
            throw std::runtime_error("FullDuplexUDPChannel::WriteLoop()::setsockopt::reuse");
        }

        /* construct a multicast address structure */
        struct sockaddr_in mc_addr;
        memset(&mc_addr, 0, sizeof(mc_addr));
        mc_addr.sin_family = AF_INET;
        mc_addr.sin_port = htons(ipv4_address_.port());
        mc_addr.sin_addr.s_addr = inet_addr(ipv4_address_.host().c_str());

        if (bind(socket_rx_, (struct sockaddr*) &mc_addr, sizeof(mc_addr)) == -1)
        {
            throw std::runtime_error("SetupAndJoinMulticast()::setsockopt::bind");
        }

        //join the multicast group
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = inet_addr(ipv4_address_.host().c_str());
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        if(setsockopt(socket_rx_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))==-1)
        {
            throw std::runtime_error("FullDuplexUDPChannel::WriteLoop()::setsockopt::ADD_MEMBERSHIP");
        }


        while(!read_thread_.IsQuitRequested())
        {
            //assume max 64K packet....
            unsigned char t[65536];
            if(MOOS::WaitForSocket(socket_rx_,1))
            {
                //do the read from the socket...
                struct sockaddr_storage sender;
                socklen_t sendsize = sizeof(sender);
                memset(&sender,0, sizeof(sender));

                int n = recvfrom(socket_rx_, t, sizeof(t),
                                 0,
                                 (struct sockaddr*)&sender,
                                 &sendsize);
                if(n>0)
                {
                    std::vector<unsigned char> v(t,t+n);
                    inbox_.Push( v);
                }

                while(inbox_.Size()>unread_limit_)
                {
                    inbox_.Pop();
                }

            }

        }

    }
    catch(std::exception & e)
    {
        std::cerr<<"issue with socket creation :"<<e.what()<<"\n";
        return false;
    }



    return true;
}

bool MulticastNode::WriteLoop()
{

    try
    {
        //set up a simply DGRAM socket
        int socket_tx = socket(AF_INET, SOCK_DGRAM, 0);
        if(socket_tx<0)
        {
            throw std::runtime_error("FullDuplexUDPChannel::WriteLoop()::socket()");
        }

        //we want to be able to resuse it (multiple folk are interested)
        int reuse = 1;
        if (setsockopt(socket_tx, SOL_SOCKET,SO_REUSEADDR/* SO_REUSEPORT*/, &reuse, sizeof(reuse)) == -1)
        {
            throw std::runtime_error("FullDuplexUDPChannel::WriteLoop()::setsockopt::reuse");
        }

        if(setsockopt(socket_tx, IPPROTO_IP, IP_MULTICAST_TTL, &hops_, sizeof(hops_))==-1)
        {
            throw std::runtime_error("FullDuplexUDPChannel::WriteLoop()::setsockopt::reuse");
        }

        /* construct a multicast address structure */
        struct sockaddr_in mc_addr;
        memset(&mc_addr, 0, sizeof(mc_addr));
        mc_addr.sin_family = AF_INET;
        mc_addr.sin_port = htons(ipv4_address_.port());
        mc_addr.sin_addr.s_addr = inet_addr(ipv4_address_.host().c_str());



        while(!write_thread_.IsQuitRequested())
        {
            std::vector<unsigned char> v;
            if(!outbox_.IsEmpty() || outbox_.WaitForPush(100))
            {
                outbox_.Pull(v);
                int nSent = sendto(socket_tx,
                           v.data(),
                           v.size(),
                           0,
                           (struct sockaddr *)&mc_addr,
                           sizeof(mc_addr));
                if(nSent!=(int)v.size())
                {
                    std::cerr<<"FullDuplexUDPChannel::WriteLoop() failed to send complete telegram\n";
                }
                else
                {
//                    std::cerr<<"wrote "<<nSent<<"\n";
                }
            }
        }


    }
    catch(std::exception & e)
    {
        std::cerr<<"issue with socket creation :"<<e.what()<<"\n";
        return false;
    }

    return true;
}





}//namespace
