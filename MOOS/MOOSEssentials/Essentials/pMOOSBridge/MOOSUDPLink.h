/*
 *  MOOSUDPLink.h
 *  MOOS
 *
 *  Created by pnewman on 16/05/2009.
 *  Copyright 2009 Oxford University. All rights reserved.
 *
 */

#ifndef MOOSUDPLINKH
#define MOOSUDPLINKH

#include "MOOS/libMOOS/Comms/XPCUdpSocket.h"
#include "MOOS/libMOOS/Utils/MOOSThread.h"
#include "MOOS/libMOOS/Comms/MOOSMsg.h"
#include "MOOS/libMOOS/Comms/MOOSCommPkt.h"



class CMOOSUDPLink
    {
    public:
        CMOOSUDPLink();
        bool Post(CMOOSMsg & M,const std::string & sRemoteHost,long int nRemotePort);
        bool Fetch(MOOSMSG_LIST & MailIn);
        bool Run(int nLocalPort=-1);
        bool ListenLoop();
        
    protected:
        bool ReadPktFromArray(unsigned char *Buff,int nBytes, CMOOSCommPkt& PktRx);
        
        CMOOSThread m_ListenThread;        
  		CMOOSLock m_InLock;
        MOOSMSG_LIST m_InBox;

        XPCUdpSocket * m_pUDPSocket;

        
        long m_nLocalPort;
    };
#endif


