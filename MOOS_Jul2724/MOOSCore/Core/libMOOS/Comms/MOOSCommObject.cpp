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
//   http://www.gnu.org/licenses/lgpl.txt distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/


#ifdef _WIN32
    #pragma warning(disable : 4786)
    #pragma warning(disable : 4503)
#endif

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Comms/MOOSCommPkt.h"
#include "MOOS/libMOOS/Comms/XPCTcpSocket.h"
#include "MOOS/libMOOS/Comms/MOOSCommObject.h"
#include "MOOS/libMOOS/Utils/MOOSException.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include <iostream>



#define DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE_KB 128
#define DEFAULT_SOCKET_SEND_BUFFER_SIZE_KB 128

const int kMaxBufferSizeKB = 2048;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSCommObject::CMOOSCommObject()
{
    m_bFakeDodgyComms = false;
    m_dfDodgeyCommsDelay = 1.0;
    m_dfDodgeyCommsProbability = 0.5;
    m_dfTerminateProbability = 0.0;
    m_bDisableNagle = false;
    m_bBoostIOThreads = false;


    SetReceiveBufferSizeInKB(DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE_KB);
    SetSendBufferSizeInKB(DEFAULT_SOCKET_SEND_BUFFER_SIZE_KB);


#ifdef DEFAULT_NO_NAGLE
    SetTCPNoDelay(true);
#else
    SetTCPNoDelay(false);
#endif



}

CMOOSCommObject::~CMOOSCommObject()
{

}

bool CMOOSCommObject::ConfigureCommsTesting(double dfDodgeyCommsProbability,
                                            double dfDodgeyCommsDelay,
                                            double dfTerminateProbability)
{
    m_bFakeDodgyComms = true;
    m_dfDodgeyCommsProbability = dfDodgeyCommsProbability;
    m_dfDodgeyCommsDelay = dfDodgeyCommsDelay;
    m_dfTerminateProbability = dfTerminateProbability;
    return true;
}


bool CMOOSCommObject::SetReceiveBufferSizeInKB(unsigned int KBytes)
{
    if(KBytes>0 && KBytes<kMaxBufferSizeKB)
	{
		m_nReceiveBufferSizeKB = KBytes;
		return true;
	}
	return false;
}


void CMOOSCommObject::SetTCPNoDelay(bool bTCPNoDelay)
{
#ifdef DEFAULT_NO_NAGLE
    if(!bTCPNoDelay){
            std::cerr<<"ignoring setting SetTCPNoDelay"
                       "to false because of compile flag \n";
    }
#endif
	m_bDisableNagle = bTCPNoDelay;
}



void CMOOSCommObject::BoostIOPriority(bool bBoost)
{
	m_bBoostIOThreads = bBoost;
}


/**	 set the size of the send  buffer of the underlying socket in MB.
* Its unlikely you need to change this from the default
* @param KBytes
* @return true on success
*/
bool CMOOSCommObject::SetSendBufferSizeInKB(unsigned int KBytes)
{
    if(KBytes>0 && KBytes<kMaxBufferSizeKB)
	{
		m_nSendBufferSizeKB = KBytes;
		return true;
	}
	return false;
}


void CMOOSCommObject::SimulateCommsError()
{
    if(MOOSUniformRandom(0.0,1.0)<m_dfDodgeyCommsProbability)
    {
        std::cout<<MOOS::ConsoleColours::Yellow();
        std::cout<<"faking slow connection..."<<m_dfDodgeyCommsDelay<<"s sleep\n";
        std::cout<<MOOS::ConsoleColours::reset();

        MOOSPause((int)(m_dfDodgeyCommsDelay*1000));


    }

    if(MOOSUniformRandom(0.0,1.0)<m_dfTerminateProbability)
    {
       std::cout<<MOOS::ConsoleColours::Red();
       std::cout<<"faking application-abort mid transaction\n";
       std::cout<<MOOS::ConsoleColours::reset();
       exit(-1);
    }
}

bool CMOOSCommObject::ReadPkt(XPCTcpSocket *pSocket, CMOOSCommPkt &PktRx, int nSecondsTimeout)
{
    #define CHUNK_READ 8192

    //now receive a message back..
    int nRqd=0;
    while((nRqd=PktRx.GetBytesRequired())!=0)
    {
        //std::cerr<<"I'm asking for "<<nRqd<<"\n";
        int nRxd = 0;

        try
        {
            if(nRqd<CHUNK_READ)
            {
                //read in in chunks of 1k
                if(nSecondsTimeout<0)
                {
                    nRxd  = pSocket->iRecieveMessage(PktRx.NextWrite(),nRqd);
                }
                else
                {
                    nRxd  = pSocket->iReadMessageWithTimeOut(PktRx.NextWrite(),nRqd,(double)nSecondsTimeout);
                }
            }
            else
            {
                if(nSecondsTimeout<0)
                {
                    nRxd  = pSocket->iRecieveMessage(PktRx.NextWrite(),CHUNK_READ);
                }
                else
                {
                    nRxd  = pSocket->iReadMessageWithTimeOut(PktRx.NextWrite(),CHUNK_READ,(double)nSecondsTimeout);
                }
            }
        }
        catch( XPCException & e)
        {
            MOOSTrace("Exception %s\n",e.sGetException());
            throw CMOOSException("CMOOSCommObject::ReadPkt() Failed Rx");
        }

        switch(nRxd)
        {
        case -1:
            throw CMOOSException("Gross error....");
            break;
        case 0:
            if(nSecondsTimeout>0)
                throw CMOOSException(MOOSFormat("remote side closed or lazy client ( waited more than %ds )",nSecondsTimeout));
            else
                throw CMOOSException("remote side closed....");
            break;
        default:
            if(!PktRx.OnBytesWritten(PktRx.NextWrite(),nRxd))
                throw CMOOSException("CMOOSCommObject::ReadPkt() Failed Rx - Packet rejects filling");
            break;
        }
    }

    return true;
}

//bool CMOOSCommObject::ReadPktV2(XPCTcpSocket *pSocket, CMOOSCommPkt &PktRx, int nSecondsTimeout)
//{
//    #define CHUNK_READ 8192
//    unsigned char Buffer[CHUNK_READ];
//    unsigned char *pBuffer = Buffer;

//    //now receive a message back..
//    int nRqd=0;
//    while((nRqd=PktRx.GetBytesRequired())!=0)
//    {
//        int nRxd = 0;

//        try
//        {
//            if(nRqd<CHUNK_READ)
//            {
//                //read in in chunks of 1k
//                if(nSecondsTimeout<0)
//                {
//                    nRxd  = pSocket->iRecieveMessage(pBuffer,nRqd);
//                }
//                else
//                {
//                    nRxd  = pSocket->iReadMessageWithTimeOut(pBuffer,nRqd,(double)nSecondsTimeout);
//                }
//            }
//            else
//            {
//                //read in in chunks of 1k
//                if(nSecondsTimeout<0)
//                {
//                    nRxd  = pSocket->iRecieveMessage(pBuffer,CHUNK_READ);
//                }
//                else
//                {
//                    nRxd  = pSocket->iReadMessageWithTimeOut(pBuffer,CHUNK_READ,(double)nSecondsTimeout);
//                }
//            }
//        }
//        catch(XPCException e)
//        {
//            MOOSTrace("Exception %s\n",e.sGetException());
//            throw CMOOSException("CMOOSCommObject::ReadPkt() Failed Rx");
//        }

//        switch(nRxd)
//        {
//        case -1:
//            throw CMOOSException("Gross error....");
//            break;
//        case 0:
//            if(nSecondsTimeout>0)
//                throw CMOOSException(MOOSFormat("remote side closed or lazy client ( waited more than %ds )",nSecondsTimeout));
//            else
//                throw CMOOSException("remote side closed....");
//            break;
//        default:
//            PktRx.Fill(pBuffer,nRxd);
//            break;
//        }
//    }

//    return true;
//}

bool CMOOSCommObject::SendPkt(XPCTcpSocket *pSocket, CMOOSCommPkt &PktTx)
{
    int nSent = 0;

    try
    {

        if(m_bFakeDodgyComms)
        {
            //this is some very low level cruft that is only hear to provide
        	//some gruesome testing - normal programmers should ignore this
        	//block of code
        	nSent+=pSocket->iSendMessage(PktTx.Stream(),sizeof(int));
        	SimulateCommsError();
        	nSent+=pSocket->iSendMessage(PktTx.Stream()+sizeof(int),PktTx.GetStreamLength()-sizeof(int));
        }
        else
        {
        	nSent = pSocket->iSendMessage(PktTx.Stream(),PktTx.GetStreamLength());
        }
    }
    catch(XPCException & e)
    {
        MOOSTrace("MOOSCommObject::SendPkt Exception caught %s\n",e.sGetException());
        throw CMOOSException("CMOOSCommObject::SendPkt() Failed Tx");
    }
    
    if(nSent!=PktTx.GetStreamLength())
    {
        throw CMOOSException("CMOOSCommObject::SendPkt() Failed Tx");
    }

    return true;
}

bool CMOOSCommObject::SendMsg(XPCTcpSocket *pSocket,CMOOSMsg &Msg)
{
    MOOSMSG_LIST MsgList;
    
    MsgList.push_front(Msg);

    CMOOSCommPkt Pkt;

    Pkt.Serialize(MsgList,true);
	

    return SendPkt(pSocket,Pkt);
}


bool CMOOSCommObject::ReadMsg(XPCTcpSocket *pSocket,CMOOSMsg &Msg, int nSecondsTimeout)
{
    MOOSMSG_LIST MsgList;
    
    CMOOSCommPkt Pkt;

    if(ReadPkt(pSocket,Pkt,nSecondsTimeout))
    {

        Pkt.Serialize(MsgList,false);

        if(!MsgList.empty())
        {
            Msg = MsgList.front();
        }
        
    }


    return !MsgList.empty();
}



std::string CMOOSCommObject::GetLocalIPAddress()
{
    char Name[255];
    if(gethostname(Name,sizeof(Name))!=0)
    {
        MOOSTrace("Error getting host name\n");
        return "unknown";
    }
    return std::string(Name);
}


bool CMOOSCommObject::SocketsInit()
{

#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 2, 2 );
 
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */

        printf("failed socket init\n");
        return false;
    }
 
    /* Confirm that the WinSock DLL supports 2.2.*/
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */
 
    if ( LOBYTE( wsaData.wVersion ) != 2 ||
            HIBYTE( wsaData.wVersion ) != 2 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                     
        */

        printf("failed socket init\n");

        WSACleanup( );
        return false; 
    }
#endif
    return true;
}

