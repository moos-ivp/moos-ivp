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
//   http://www.gnu.org/licenses/lgpl.txtgram is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
//   The XPC classes in MOOS are modified versions of the source provided
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga
//

#ifndef _XPCSocket
#define _XPCSocket

#include "MOOS/libMOOS/Comms/XPCGetProtocol.h"
#include "MOOS/libMOOS/Comms/XPCGetHostInfo.h"
#include <stdio.h>
//#include <string.h>

#ifdef UNIX
    #include <sys/socket.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    #include <iostream>
    #include <sys/types.h>
    #include <sys/ioctl.h>
	#include <netinet/tcp.h>
    #ifdef PLATFORM_LINUX
        #define FIONBIO 0x5421
    #endif
#elif _WIN32
    #include <winsock2.h>
    #include "windows.h"
    #include "winbase.h"
    #include "winnt.h"
    typedef int socklen_t;
#else
    #error "Looks like the build scripts didn't set the platform type"
#endif

class XPCSocket
{
protected:
    int iPort;        // Socket port number
    int iSocket;        // Socket file descriptor
    int iBlocking;        // Blocking flag
    char cBind;        // Binding flag
    double  m_dfLastRead;
    struct sockaddr_in clientAddress;    // Address of the client that sent data
public:
    void vSetRecieveTimeOut(int nTimeOut);
    void SetReadTime(double dfTime){m_dfLastRead = dfTime;};
    double GetReadTime(){return m_dfLastRead;};

    //returns integer number of last socket error
    static int iGetLastError();
    // Constructor.  Creates a socket given a protocol (UDP / TCP) and a port number
    XPCSocket(const char *_sProtocol, int _iPort);

    // Constructor.  Stores a socket file descriptor
    XPCSocket(int _iSocket) : iSocket(_iSocket) { };

    // Destructor.  Closes the socket
    virtual ~XPCSocket()
    {
        vCloseSocket();
    }

    // Closes the socket
    void vCloseSocket()
    {
        #ifdef    WINDOWS_NT
            closesocket(iSocket);
        #else
            close(iSocket);
        #endif
    }

    // The following member functions sets socket options on and off
    void vSetDebug(int _iToggle);
    void vSetBroadcast(int _iToggle);
    void vSetReuseAddr(int _iToggle);
    void vSetKeepAlive(int _iToggle);
    void vSetLinger(struct linger _lingerOption);
    void vSetSocketBlocking(int _iToggle);

    // Sets the size of the send and receive buffer
    void vSetSendBuf(int _iSendBufSize);
    void vSetRecieveBuf(int _iRecieveBufSize);

    // The following member functions retrieve socket option settings
    int iGetDebug();
    int  iGetBroadcast();
    int  iGetReuseAddr();
    int  iGetKeepAlive();
    void vGetLinger(struct linger &_lingerOption);
    int  iGetSendBuf();
    int  iGetRecieveBuf();
    int  iGetSocketBlocking() { return iBlocking; }

    // Returns the socket file descriptor
    int  iGetSocketFd() { return iSocket; }

    // Gets the system error
    char *sGetError()
    {
        #ifdef UNIX
            return strerror(errno);
        #elif _WIN32
            static char buf[10];
            sprintf(buf, "%d", WSAGetLastError());
            return buf;
        #endif
    }
};

#endif

