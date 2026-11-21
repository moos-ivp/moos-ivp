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
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//



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
    int iSendMessage(const void *_vMessage, int _iMessageSize);

    // Receives a TCP message 
    int iRecieveMessage(void *_vMessage, int _iMessageSize, int _iOption = 0);

    // Binds the socket to an address and port number
    void vBindSocket();

    // Accepts a connecting client.  The address of the connected client is stored in the
    // parameter
    XPCTcpSocket *Accept(char *_sHost = NULL);

    // Listens to connecting clients
    void vListen(int _iNumPorts = -1);

    // Connects to a client specified by a supplied host name
    virtual void vConnect(const char *_sHost);

    // allows a read with a timeout to prevent from blocking indefinitely
    int iReadMessageWithTimeOut(void *_vMessage, int _iMessageSize, double dfTimeOut,int _iOption=0);

    void vSetNoDelay(int _iToggle);

};

#endif
