///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS CORE Component. 
//        
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; either version 2 of the 
//   License, or (at your option) any later version. 
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//   General Public License for more details. 
//            
//   You should have received a copy of the GNU General Public License 
//   along with this program; if not, write to the Free Software 
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
//   02111-1307, USA. 
//
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//
//////////////////////////    END_GPL    //////////////////////////////////


#ifndef _XPCTcpSocket
#define _XPCTcpSocket

#include "XPCSocket.h"

#ifdef WINDOWS_NT
#ifndef MSG_WAITALL
#define MSG_WAITALL 4
#endif
#endif

class XPCTcpSocket : public XPCSocket
{
private:
#ifdef WINDOWS_NT
    // Windows NT version of the MSG_WAITALL option
    int iRecieveMessageAll(void *_vMessage, int _iMessageSize);
#endif
public:

    // Constructor.  Used to create a new TCP socket given a port
    XPCTcpSocket(long int _iPort) : XPCSocket("tcp", _iPort) { };

    // Constructor.  Called when a client connects and a new file descriptor has
    // be created as a result.    
    XPCTcpSocket(short int _iSocketFd) : XPCSocket(_iSocketFd) { };

    // Sends a message to a connected host. The number of bytes sent is returned
    int iSendMessage(void *_vMessage, int _iMessageSize);

    // Receives a TCP message 
    int iRecieveMessage(void *_vMessage, int _iMessageSize, int _iOption = 0);

    // Binds the socket to an address and port number
    void vBindSocket();

    // Accepts a connecting client.  The address of the connected client is stored in the
    // parameter
    XPCTcpSocket *Accept(char *_sHost = NULL);

    // Listens to connecting clients
    void vListen(int _iNumPorts = 5);

    // Connects to a client specified by a supplied host name
    virtual void vConnect(const char *_sHost);

    // allows a read with a timeout to prevent from blocking indefinitely
    int iReadMessageWithTimeOut(void *_vMessage, int _iMessageSize, double dfTimeOut,int _iOption=0);

};

#endif
