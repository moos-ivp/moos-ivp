/*
 *  MOOSUDPLink.cpp
 *  MOOS
 *
 *  Created by pnewman on 16/05/2009.
 *  Copyright 2009 Oxford University. All rights reserved.
 *
 */

#include "MOOSUDPLink.h"
#include <MOOSLIB/XPCUdpSocket.h>
#include <MOOSLIB/MOOSException.h>


#define MAX_UDP_PKT_SIZE 65536


bool _ListenCB(void * pParam)
{
    CMOOSUDPLink* pMe = (CMOOSUDPLink* )pParam;
    return pMe->ListenLoop();
    
}


CMOOSUDPLink::CMOOSUDPLink()
{
    m_pUDPSocket = NULL;
    m_nLocalPort = -1;
}

bool CMOOSUDPLink::Run(int nLocalPort)
{
    
    m_nLocalPort = nLocalPort;
    
    //input socket
    m_pUDPSocket = new XPCUdpSocket(m_nLocalPort);
    
    //its likely we want tp broadcast
    
    m_pUDPSocket->vSetBroadcast(true);
    
    if(nLocalPort!=-1)
    {
    
        try
        {
            m_pUDPSocket->vBindSocket();
        }
        catch (XPCException e)
        {
            MOOSTrace(e.sGetException());
            return false;
        }
        
        m_ListenThread.Initialise(_ListenCB,this);
        m_ListenThread.Start();
    }

    return true;

}

bool CMOOSUDPLink::Post(CMOOSMsg & M,const std::string & sHost, long int nPort)
{
    
    if(m_pUDPSocket==NULL)
        return MOOSFail("CMOOSUDPLink::Post No functioning UDP socket - has run been called\n");
    
    
    MOOSMSG_LIST OutBox;
    OutBox.push_back(M);

    CMOOSCommPkt P;
    
    P.Serialize(OutBox, true, false);
    
    try
    {
        if(P.GetStreamLength()>MAX_UDP_PKT_SIZE)
        {
            //too big
            throw XPCException("serialised packet is too big to send ....ask for an upgrade");    
        }
        
        int nSent = m_pUDPSocket->iSendMessageTo(P.m_pStream,P.GetStreamLength(),nPort,sHost);
        
        
        if(nSent!=P.GetStreamLength())
        {
            //something abd happened..
            throw XPCException("failed to send mailbox as datagram");
        }
        else
        {
            //MOOSTrace("sent %d bytes\n", nSent);
        }
    }
    catch(XPCException e)
    {
        MOOSTrace("CMOOSUdpLink::PostLoop Exception caught %s\n",e.sGetException());
    }
    
    
    return true;
    
}

bool CMOOSUDPLink::Fetch(MOOSMSG_LIST & M)
{
    m_InLock.Lock();
    	M.splice(M.begin(),m_InBox);
    m_InLock.UnLock();
    return true;
}




bool CMOOSUDPLink::ListenLoop()
{
    //MOOSTrace("listening for incoming data packets on port %d\n",m_nLocalPort);
    unsigned char Buff[MAX_UDP_PKT_SIZE];
    
    while(!m_ListenThread.IsQuitRequested())
    {
        int nRead = m_pUDPSocket->iRecieveMessage((unsigned char*)Buff,sizeof(Buff));
        CMOOSCommPkt P;
        
        try
        {
			ReadPktFromArray(Buff, nRead, P);

            m_InLock.Lock();
            {
            	if(P.Serialize(m_InBox,false))
                {
                	MOOSTrace("%d MOOSMsg's held\n",m_InBox.size());
                }
                else
                {
                    MOOSTrace("failed to serialise CommPacket to Mailbox");
                }
                
            }
            m_InLock.UnLock();
            
        }
        catch (CMOOSException e)
        {
            MOOSTrace("failed to serialise udp datagram to a CommPacket");
        }
            
        
    	
    }
    
	return true;
}



bool CMOOSUDPLink::ReadPktFromArray(unsigned char *pBuffer,int nBytes, CMOOSCommPkt& PktRx)
{
    
    int nRqd=0;
    int q = 0;
    while((nRqd=PktRx.GetBytesRequired())!=0)
    {
        if(q+nRqd<=nBytes)
        {
            PktRx.Fill(pBuffer+q,nRqd);
            q+=nRqd;
        }
        else
        {
            throw CMOOSException("UDP Packet was too small! Gross Error");            
        }
    }
    
    return true;
}


