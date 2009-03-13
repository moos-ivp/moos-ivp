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
#include "XPCTcpSocket.h"
#ifdef _WIN32
#else
#include <sys/time.h>
#endif

#include <string>



void XPCTcpSocket::vConnect(const char *_sHost)
{
    struct sockaddr_in serverAddress;
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(iPort);
    
    // Resolve the IP address of the given host name
    std::string sHost(_sHost);
    hostType HostType;
    if(sHost.find_first_not_of("0123456789. ")!=std::string::npos)
    {
        HostType = NAME;
        XPCGetHostInfo getHostInfo(_sHost, HostType);
        
        // Store the IP address and socket port number
        serverAddress.sin_addr.s_addr =inet_addr(getHostInfo.sGetHostAddress());
        
    }
    else
    {
        HostType = ADDRESS;
        // Store the IP address and socket port number
        serverAddress.sin_addr.s_addr =inet_addr(_sHost);
    }        
    
    // Connect to the given address
    if (connect(iSocket, (struct sockaddr *)&serverAddress,sizeof(serverAddress)) == -1)
    {
        char sMsg[512];
        sprintf(sMsg, "Error Connecting To Socket. %s", sGetError());
        XPCException socketExcept(sMsg);
        throw socketExcept;
        return;
    }
}

XPCTcpSocket *XPCTcpSocket::Accept(char *_sHost)
{
    short int iNewSocket;   // Stores the new socket file descriptor
    
    struct sockaddr_in clientAddress;       // Stores the connected clients info
    
    // Gets the length of the client's address
    int iClientAddressLen = sizeof(clientAddress);
    
    // Accepts a new client connection and stores its socket file descriptor
    if ((iNewSocket = accept(iSocket, (struct sockaddr *)&clientAddress,(socklen_t*)&iClientAddressLen)) == -1)
    {
        char sMsg[512];
        
        sprintf(sMsg, "Error Accepting Socket. %s", sGetError());
        XPCException socketExcept(sMsg);
        throw socketExcept;
        return NULL;
    } 
    
    // If the host name is requested
    if (_sHost != NULL)
    {
        // Get the ascii representation of the address
        char *sAddress = inet_ntoa((struct in_addr)clientAddress.sin_addr);
        
        // Get the host name given the address
        try
        {
            XPCGetHostInfo getHostInfo(sAddress, ADDRESS);
            // Store the host name
            strcpy(_sHost, getHostInfo.sGetHostName());
        }
        catch(XPCException e)
        {
            strcpy(_sHost, sAddress);
            printf("INFO: %s using numeric address %s\n",e.sGetException(),_sHost);
        }
        
    }
    
    // Create and return the new XPCTcpSocket object
    XPCTcpSocket *newSocket = new XPCTcpSocket(iNewSocket);
    
    return newSocket;
}

void XPCTcpSocket::vListen(int _iNumPorts)
{
    // Incoming connections are listened for
    if (listen(iSocket, _iNumPorts) == -1)
    {
        char sMsg[512];       
        sprintf(sMsg, "Error Listening To Socket. %s", sGetError());
        throw XPCException(sMsg);;
        return;
    }
}       

int XPCTcpSocket::iSendMessage(void *_vMessage, int _iMessageSize)
{
    int iNumBytes;  // Stores the number of bytes sent
    
    // Sends the message to the connected host
    if ((iNumBytes = send(iSocket, (char *)_vMessage, _iMessageSize, 0)) ==
        -1)
    {
        char sMsg[512];        
        sprintf(sMsg, "Error sending socket message: %s", sGetError());
        throw XPCException(sMsg);
        return 0;
    }
    return iNumBytes;
}

#ifdef WINDOWS_NT
int XPCTcpSocket::iRecieveMessageAll(void *_vMessage, int _iMessageSize)
{
    int iNumBytes = 0;      // The number of bytes received
    
    int iCurrentSize = _iMessageSize;       // The number of bytes wanted toreceive
    int iOffsetSize = 0;                    // The number of bytes currentlyrecieved
    
    // While the number of bytes received is less than the number requestedcontinue to
    // retrieve more data
    while (iNumBytes < iCurrentSize)
    {
        // The socket message is recieved and stored within the mesageoffset by the 
        // offset number of bytes
        iNumBytes = recv(iSocket, (char *)_vMessage + iOffsetSize,iCurrentSize, 0);
        if (iNumBytes == -1)
        {
            // If the reason for failure is a client disconnect, an exception isnot thrown.
            // The number of bytes returned is 0
            if (WSAGetLastError() == WSAECONNRESET)
                return 0;
            
            char sMsg[512];
            
            sprintf(sMsg, "Error receiving on socket: %s", sGetError());
            XPCException socketExcept(sMsg);
            throw socketExcept;
            return iNumBytes;
        }
        else if (iNumBytes == 0)
            return iNumBytes;
        
        // If the total number of bytes requested are not returned, theoffset is adjusted
        // and the number of bytes left to receive is also adjusted
        else if (iNumBytes < iCurrentSize)
        {
            iOffsetSize += iNumBytes;
            iCurrentSize = iCurrentSize - iNumBytes;
            iNumBytes = 0;
        }
    }
    
    return _iMessageSize;   
}
#endif

int XPCTcpSocket::iRecieveMessage(void *_vMessage, int _iMessageSize, int _iOption)
{
    int iNumBytes;  // The number of bytes recieved
    
#ifdef WINDOWS_NT
    if (_iOption == MSG_WAITALL)
        // If the option is MSG_WAITALL and this is a WINDOW_NT machinecall iReceiveMessageAll
        // to process it
        return iRecieveMessageAll(_vMessage, _iMessageSize);
#endif
    
    // Recieves a TCP socket message.  The number of bytes received isreturned
    iNumBytes = recv(iSocket, (char *)_vMessage, _iMessageSize, _iOption);
    if (iNumBytes == -1)
    {
        // If the reason for failure is a client disconnect, anexception is not thrown.
        // The number of bytes returned is 0    
#ifdef UNIX
        if (errno == ECONNRESET)
            return 0;
#else
        if (WSAGetLastError() == WSAECONNRESET)
            return 0;
#endif
        char sMsg[512];
        
        sprintf(sMsg, "Error receiving on socket: %s", sGetError());
        printf("%s\n",sMsg);
        throw XPCException(sMsg);
        
    }
    
    return iNumBytes;
}

void XPCTcpSocket::vBindSocket()
{
    // Bind the socket to the given address and port number
    if (bind(iSocket, (struct sockaddr *)&clientAddress,sizeof(clientAddress)) == -1)
    {
        char sMsg[512];
        sprintf(sMsg, "Error binding to socket: %s", sGetError());
        XPCException socketExcept(sMsg);
        throw socketExcept;
        return;
    }
}

int XPCTcpSocket::iReadMessageWithTimeOut(void *_vMessage, int _iMessageSize, double dfTimeOut,int _iOption)
{
    int iNumBytes = 0;
    
    struct timeval timeout;        // The timeout value for the select system call
    fd_set fdset;                // Set of "watched" file descriptors
    
    
    // The socket file descriptor set is cleared and the socket file 
    // descriptor contained within tcpSocket is added to the file
    // descriptor set.
    FD_ZERO(&fdset);
    
    FD_SET((unsigned int)iGetSocketFd(), &fdset);
    
    
    // The select system call is set to timeout after 1 seconds with no data existing
    // on the socket. This has to be here, within the loop as Linux actually writes over
    // the timeout structure on completion of select (now that was a hard bug to find)
    
    if(dfTimeOut<1)
    {
        dfTimeOut=1.0;
    }
    
    timeout.tv_sec    = (int)dfTimeOut;
    timeout.tv_usec = 0;
    
    
    // A select is setup to return when data is available on the socket
    // for reading.  If data is not available after timeout seconds, select
    // returns with a value of 0.  If data is available on the socket,
    // the select returns and data can be retrieved off the socket.
    int iSelectRet = select(iGetSocketFd() + 1,
        &fdset,
        NULL,
        NULL,
        &timeout);
    
    // If select returns a -1, then it failed and the thread exits.
    switch(iSelectRet)
    {
    case -1:
        //                Trace("Select failed ");
        iNumBytes=-1;
        break;
        
    case 0:
        //timeout...nothing to read
        iNumBytes = 0;
        break;
        
    default:
        
        if (FD_ISSET(iGetSocketFd(), &fdset) != 0)
        {
            //something to do read:
            return iRecieveMessage(_vMessage,_iMessageSize,_iOption);
        }
        break;
    }
    
    //zero socket set..
    FD_ZERO(&fdset);
    
    return iNumBytes;
}



