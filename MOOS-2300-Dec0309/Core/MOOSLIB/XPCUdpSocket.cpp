/*
 *  XPCUdpSocket.cpp
 *  MOOS
 *
 *  Created by pnewman on 13/05/2009.
 *  Copyright 2009 Oxford University. All rights reserved.
 *
 */

#include "XPCUdpSocket.h"
#include <map>
#include <string>



XPCUdpSocket::XPCUdpSocket(long int iPort): XPCSocket("udp", iPort)
{
}    


bool XPCUdpSocket::GetAddress(long int nPort,const std::string & sHost,sockaddr_in & Address)
{

    std::map< std::pair< long int , std::string  >, sockaddr_in >::iterator q;
    
    std::pair< long int , std::string  > PP(nPort,sHost);
    
    q = m_KnownAdresses.find(PP);
    
    if(q!=m_KnownAdresses.end())
    {
        Address = q->second;
        return true;
    }
    else
    {
        
		Address.sin_family = AF_INET;
    	Address.sin_port = htons(nPort);
        
        hostType HostType;
        if(sHost.find_first_not_of("0123456789. ")!=std::string::npos)
        {
            HostType = NAME;
            XPCGetHostInfo getHostInfo(sHost.c_str(), HostType);
            
            // Store the IP address and socket port number
            Address.sin_addr.s_addr =inet_addr(getHostInfo.sGetHostAddress());
            
        }
        else
        {
            HostType = ADDRESS;
            // Store the IP address and socket port number
            Address.sin_addr.s_addr =inet_addr(sHost.c_str());
        }        
        
        //save it
        m_KnownAdresses[PP] = Address;
        
        return true;
    }
    
}

int XPCUdpSocket::iBroadCastMessage(void *_vMessage, int _iMessageSize,long int nPort)
{
	return iSendMessageTo(_vMessage, _iMessageSize,nPort,"255.255.255.255");
}

// Sends a message to a connected host. The number of bytes sent is returned
int XPCUdpSocket::iSendMessageTo(void *_vMessage, int _iMessageSize,long int nPort,const std::string & sHost)
{
    
    sockaddr_in  Address;
    
    if(!GetAddress(nPort,sHost,Address))
    {
        throw XPCException("::iSendMessageTo failed to get destination address\n");
    }
    
    int iNumBytes=sendto(iSocket, (const char*)_vMessage, _iMessageSize,0,
             (struct sockaddr *) &Address, sizeof(Address) );
    
    
    // Sends the message to the connected host
    if (iNumBytes == -1)
    {
        char sMsg[512];        
        sprintf(sMsg, "Error sending socket message: %s", sGetError());
        throw XPCException(sMsg);
        return 0;
    }
    return iNumBytes;
    
}

// Receives a UDP message 
int XPCUdpSocket::iRecieveMessage(void *_vMessage, int _iMessageSize, int _iOption)
{
    
    int iNumBytes;  // The number of bytes recieved
    
    
    // Recieves a UDP socket message.  The number of bytes received isreturned
    struct sockaddr SenderInfo;

    socklen_t SenderInfoLen = sizeof (SenderInfo);
    
    iNumBytes = recvfrom(iSocket, (char *)_vMessage,
                         _iMessageSize,
                         _iOption,(struct sockaddr *) &SenderInfo, &SenderInfoLen);
    if (iNumBytes <=0)
    {
        char sMsg[512];
        
        sprintf(sMsg, "Error receiving on socket: %s", sGetError());
        printf("%s\n",sMsg);
        throw XPCException(sMsg);
        
    }
    
    return iNumBytes;
    
}

// Binds the socket to an address and port number
void XPCUdpSocket::vBindSocket()
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

/*
// allows a read with a timeout to prevent from blocking indefinitely
int XPCUdpSocket::iReadMessageWithTimeOut(void *_vMessage, int _iMessageSize, double dfTimeOut,int _iOption)
{
    return -1;
}
*/


