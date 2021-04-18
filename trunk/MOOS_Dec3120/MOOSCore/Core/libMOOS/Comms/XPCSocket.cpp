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
//   http://www.gnu.org/licenses/lgpl.txt  This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/


//   The XPC classes in MOOS are modified versions of the source provided
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga
//
//////////////////////////    END_GPL    //////////////////////////////////
#include "MOOS/libMOOS/Comms/XPCSocket.h"

XPCSocket::XPCSocket(const char *_sProtocol, int _iPort)
{
    iPort = _iPort;
    iBlocking = 0;

    try
    {
        // Retrieve the socket protocol
        XPCGetProtocol socketProtocol(_sProtocol);

        // If the protocol is UDP a UDP socket is created
        if (strcmp(socketProtocol.sGetProtocolName(), "udp") == 0)
        {
            if ((iSocket = socket(AF_INET, SOCK_DGRAM, socketProtocol.iGetProtocolNumber())) == -1)
            {
                char sMsg[512];

                sprintf(sMsg, "Error opening socket: %s", sGetError());
                XPCException socketExcept(sMsg);
                throw socketExcept;
            }
        }

        // If the protocol is TCP a TCP socket is created
        else if (strcmp(socketProtocol.sGetProtocolName(), "tcp") == 0)
        {
            if ((iSocket = socket(AF_INET, SOCK_STREAM, socketProtocol.iGetProtocolNumber())) == -1)
            {
                char sMsg[512];

                sprintf(sMsg, "Error opening socket: %s", sGetError());
                XPCException socketExcept(sMsg);
                throw socketExcept;
            }
        }
    }
    catch(XPCException &exceptObject)
    {
        char sMsg[512];

        sprintf(sMsg, "Protocol Error Definition: %s", exceptObject.sGetException());
        XPCException socketExcept(sMsg);
        throw socketExcept;
    }

    // The client address is initialized to all addresses at the specified port
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddress.sin_port = htons((short)iPort);
}

void XPCSocket::vSetDebug(int _iToggle)
{
    if (setsockopt(iSocket, SOL_SOCKET, SO_DEBUG, (char *)&_iToggle, sizeof(_iToggle)) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Setting Debug Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
}

void XPCSocket::vSetBroadcast(int _iToggle)
{
    if (setsockopt(iSocket, SOL_SOCKET, SO_BROADCAST, (char *)&_iToggle, sizeof(_iToggle)) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Setting Broadcast Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
}



void XPCSocket::vSetReuseAddr(int _iToggle)
{
    if (setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&_iToggle, sizeof(_iToggle)) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Setting Reuseaddr Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
}

void XPCSocket::vSetKeepAlive(int _iToggle)
{
    if (setsockopt(iSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&_iToggle, sizeof(_iToggle)) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Setting Keepalive Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
}

void XPCSocket::vSetLinger(struct linger _lingerOption)
{
    if (setsockopt(iSocket, SOL_SOCKET, SO_LINGER, (char *)&_lingerOption, sizeof(struct linger)) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Setting Linger Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
}

void XPCSocket::vSetSendBuf(int _iSendBufSize)
{
    if (setsockopt(iSocket, SOL_SOCKET, SO_SNDBUF, (char *)&_iSendBufSize, sizeof(_iSendBufSize)) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Setting SendBufSize Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
}

void XPCSocket::vSetRecieveBuf(int _iRecieveBufSize)
{
    if (setsockopt(iSocket, SOL_SOCKET, SO_SNDBUF, (char *)&_iRecieveBufSize, sizeof(_iRecieveBufSize)) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Setting RecieveBufSize Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
}

void XPCSocket::vSetSocketBlocking(int _iToggle)
{
    char sMsg[512];

    if (_iToggle)
    {
        if (iGetSocketBlocking())
            return;
        else
        {
            iBlocking = 1;
            // Socket blocking is turned ON
#ifdef WINDOWS_NT
            if (ioctlsocket(iSocket, FIONBIO, (unsigned long *)&iBlocking) == -1)
#else
                if (ioctl(iSocket, FIONBIO, (char *)&iBlocking) == -1)
#endif
                {
                    sprintf(sMsg, "Error Turning ON Socket Blocking Status: %s", sGetError());
                    XPCException sockOptExcept(sMsg);
                    throw sockOptExcept;
                }

        }
    }
    else
    {
        if (!iGetSocketBlocking())
            return;
        else
        {
            iBlocking = 0;
            // Socket blocking is turned off
#ifdef WINDOWS_NT
            if (ioctlsocket(iSocket, FIONBIO, (unsigned long *)&iBlocking) == -1)
#else
                if (ioctl(iSocket, FIONBIO, (char *)&iBlocking) == -1)
#endif
                {
                    sprintf(sMsg, "Error Turning OFF Socket Blocking Status: %s", sGetError());
                    XPCException sockOptExcept(sMsg);
                    throw sockOptExcept;
                }

        }
    }
}

int XPCSocket::iGetDebug()
{
    int iGetOption;

    int iOptionLen = sizeof(iGetOption);

    if (getsockopt(iSocket, SOL_SOCKET, SO_DEBUG, (char *)&iGetOption,(socklen_t*) &iOptionLen) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Recieving Debug Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
    return iGetOption;
}

int XPCSocket::iGetBroadcast()
{
    int iGetOption;

    int iOptionLen = sizeof(iGetOption);

    if (getsockopt(iSocket, SOL_SOCKET, SO_BROADCAST, (char *)&iGetOption,(socklen_t*) &iOptionLen) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Extracting Broadcast Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
    return iGetOption;
}

int XPCSocket::iGetReuseAddr()
{
    int iGetOption;

    int iOptionLen = sizeof(iGetOption);

    if (getsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&iGetOption,(socklen_t*) &iOptionLen) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Extracting Resuseaddr Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
    return iGetOption;
}

int XPCSocket::iGetKeepAlive()
{
    int iGetOption;

    int iOptionLen = sizeof(iGetOption);

    if (getsockopt(iSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&iGetOption,(socklen_t*) &iOptionLen) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Extracting Keepalive Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
    return iGetOption;
}

void XPCSocket::vGetLinger(struct linger &_lingerOption)
{
    int iOptionLen = sizeof(struct linger);

    if (getsockopt(iSocket, SOL_SOCKET, SO_LINGER, (char *)&_lingerOption,(socklen_t*) &iOptionLen) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Extracting Linger Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
    return;
}

int XPCSocket::iGetSendBuf()
{
    int iSendBuf;

    int iOptionLen = sizeof(iSendBuf);

    if (getsockopt(iSocket, SOL_SOCKET, SO_SNDBUF, (char *)&iSendBuf,(socklen_t*) &iOptionLen) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Extracting SendBuf Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
    return iSendBuf;
}

int XPCSocket::iGetRecieveBuf()
{
    int iRcvBuf;

    int iOptionLen = sizeof(iRcvBuf);

    if (getsockopt(iSocket, SOL_SOCKET, SO_RCVBUF, (char *)&iRcvBuf,(socklen_t*) &iOptionLen) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Extracting RcvBuf Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
    return iRcvBuf;
}

int XPCSocket::iGetLastError()
{
#ifdef UNIX
    return errno;
#else
    return  WSAGetLastError();
#endif
}

void XPCSocket::vSetRecieveTimeOut(int nTimeOut)
{
    if (setsockopt(iSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nTimeOut, sizeof(nTimeOut)) == -1)
    {
        char sMsg[512];

        sprintf(sMsg, "Error Setting ReciveTimeOut Option: %s", sGetError());
        XPCException sockOptExcept(sMsg);
        throw sockOptExcept;
    }
}
