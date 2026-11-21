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
//   http://www.gnu.org/licenses/lgpl.txt  This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//



#ifndef _XPCUdpSocket
#define _XPCUdpSocket

#include "XPCSocket.h"
#include <map>
#include <string>


class XPCUdpSocket : public XPCSocket
    {
    public:
        
        // Constructor.  Used to create a new udp socket  - it will receive on _iPort
        XPCUdpSocket(long int _iPort) ;
        
        // Sends a message to a specified host on a specified port. The number of bytes sent is returned
        int iSendMessageTo(void *_vMessage, int _iMessageSize,long int nPort,const std::string & sHost);
        
        //send a telegram to all local network nodes on specified ports
        int iBroadCastMessage(void *_vMessage, int _iMessageSize,long int nPort);
        
        // Receives a UDP message 
        int iRecieveMessage(void *_vMessage, int _iMessageSize, int _iOption = 0);
        
        // Binds the socket to listen on a port number
        void vBindSocket();
                        
        // allows a read with a timeout to prevent from blocking indefinitely
        //int iReadMessageWithTimeOut(void *_vMessage, int _iMessageSize, double dfTimeOut,int _iOption=0);
        
    protected:
        bool GetAddress(long int nPort,const std::string & sHost,sockaddr_in & Address);

        std::map< std::pair< long int , std::string >, sockaddr_in >   m_KnownAdresses ;

        
    };

#endif
