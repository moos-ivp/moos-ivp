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


// MOOSCommClient.cpp: implementation of the CMOOSCommClient class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #include <winsock2.h>
    #include "windows.h"
    #include "winbase.h"
    #include "winnt.h"
#else
    #include <pthread.h>
#endif

#include <cmath>
#include <string>
#include <set>
#include <limits>
#include <iostream>
#include <iomanip>
#include <cassert>

#include "MOOS/libMOOS/Utils/MOOSUtils.h"
#include "MOOS/libMOOS/Utils/MOOSException.h"
#include "MOOS/libMOOS/Utils/MOOSScopedLock.h"
#include "MOOS/libMOOS/Utils/MOOSScopedPtr.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/Utils/ThreadPriority.h"
#include "MOOS/libMOOS/Utils/IPV4Address.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"

#include "MOOS/libMOOS/Comms/XPCTcpSocket.h"
#include "MOOS/libMOOS/Comms/MOOSCommClient.h"
#include "MOOS/libMOOS/Comms/MOOSCommPkt.h"
#include "MOOS/libMOOS/Comms/MOOSSkewFilter.h"


#include "MOOS/libMOOS/Comms/MulticastNode.h"



using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAX_TIME_WARP_AGGLOMERATION_CONSTANT 10.0
#ifndef TIME_WARP_AGGLOMERATION_CONSTANT
#define TIME_WARP_AGGLOMERATION_CONSTANT 0.2
#endif



/*file scope function to redirect thread work to a particular instance of CMOOSCommClient */
bool ClientLoopProc( void * pParameter)
{
	CMOOSCommClient* pMe = 	(CMOOSCommClient*)pParameter;
	return pMe->ClientLoop();
}

CMOOSCommClient::CMOOSCommClient()
{

	m_pConnectCallBackParam = NULL;
	m_pfnConnectCallBack = NULL;

	m_pfnDisconnectCallBack = NULL;
	m_pDisconnectCallBackParam = NULL;

    m_pfnMailCallBack = NULL;
	m_pSocket = NULL;

	m_nOutPendingLimit = OUTBOX_PENDING_LIMIT;
	m_nInPendingLimit = INBOX_PENDING_LIMIT;
	m_bConnected = false;
        m_dfLastConnectionTime = -1;
	m_nFundamentalFreq = CLIENT_DEFAULT_FUNDAMENTAL_FREQ;
	m_nNextMsgID=0;
	m_bFakeSource = false;
    m_bQuiet= false;
    m_bMonitorClientCommsStatus = false;

    m_nMsgsReceived = 0;
    m_nMsgsSent = 0;
    m_nPktsReceived = 0;

    m_bPostNewestToFront = false;

    m_bExpectMailBoxOverFlow = false;


    //by default this client will adjust the local time skew
    //by using time information sent by the CommServer sitting
    //at the other end of this conenection.
    m_bDoLocalTimeCorrection = true;

	m_bMailPresent = false;

	//assume an old DB
	m_bDBIsAsynchronous = false;

	SetCommsControlTimeWarpScaleFactor(TIME_WARP_AGGLOMERATION_CONSTANT);

    SetVerboseDebug(false);

	SocketsInit();

#ifdef ENABLE_DETAILED_TIMING_AUDIT
    std::cerr<<"starting detailed timing audit\n";
    end_to_end_auditor_.Start();
#endif


}

CMOOSCommClient::~CMOOSCommClient()
{
    CMOOSCommClient::Close();
}

bool CMOOSCommClient::Run(const std::string & sServer, int Port, const std::string & sMyName, unsigned int nFundamentalFrequency)
{
	if(IsRunning())
	{
		std::cerr<<"error CMOOSCommClient::Run - client is already running\n";
		return false;
	}


	m_bQuit = false;

	//do advert
	DoBanner();

	//who are we going to be talking to?
	m_sDBHost = sServer;

	//and on what port are they listening
	m_lPort = Port;

	//and what are we called?
	m_sMyName = sMyName;

	if(m_pfnConnectCallBack==NULL)
	{
	    if(!m_bQuiet)
	        MOOSTrace("Warning no connect call back has been specified\n");
	}

	m_nFundamentalFreq=nFundamentalFrequency;

	if(m_nFundamentalFreq>CLIENT_MAX_FUNDAMENTAL_FREQ)
	{
		MOOSTrace("Setting Fundamental Freq to maximum value of %d Hz\n",CLIENT_MAX_FUNDAMENTAL_FREQ);
		m_nFundamentalFreq=CLIENT_MAX_FUNDAMENTAL_FREQ;
	}
	else
	{
		//MOOSTrace("Comms Running @ %d Hz\n",m_nFundamentalFreq);
	}

	StartThreads();

	return true;
}

std::string CMOOSCommClient::GetMOOSName()
{
	return m_sMyName;
}


bool CMOOSCommClient::SetCommsTick(int nCommTick)
{
    if(nCommTick>CLIENT_MAX_FUNDAMENTAL_FREQ)
	{
		MOOSTrace("Setting Fundamental Freq to maximum value of %d Hz\n",CLIENT_MAX_FUNDAMENTAL_FREQ);
		m_nFundamentalFreq=CLIENT_MAX_FUNDAMENTAL_FREQ;
        return false;
	}
    else
    {
        m_nFundamentalFreq = (int)nCommTick;
        if(m_nFundamentalFreq==0)//catch a stupid setting
            m_nFundamentalFreq = 1;
        return true;
    }

}


bool CMOOSCommClient::ExpectOutboxOverflow(unsigned int outbox_pending_size)
{
    m_OutLock.Lock();

    m_bExpectMailBoxOverFlow = true;

    m_nOutPendingLimit = outbox_pending_size;

    while(m_OutBox.size()>m_nOutPendingLimit)
    {
        if(m_bPostNewestToFront)
            m_OutBox.pop_back();
        else
            m_OutBox.pop_front();
    }

    m_OutLock.UnLock();

    return true;
}


//void CMOOSCommClient::SetOnMailtCallBack(bool (__cdecl *pfn)( void * pMailtParam), void * pMailParam)
void CMOOSCommClient::SetOnMailCallBack(bool (*pfn)( void * pMailParam), void * pMailParam)
{
	m_pfnMailCallBack = pfn;
	m_pMailCallBackParam = pMailParam;
}

bool CMOOSCommClient::HasMailCallBack()
{
    return m_pfnMailCallBack!=NULL;
}

bool CMOOSCommClient::IsAsynchronous()
{
	return false;
}

unsigned int CMOOSCommClient::GetNumberOfUnreadMessages()
{

	m_InLock.Lock();
	unsigned int n = m_InBox.size();
	m_InLock.UnLock();
	return n;

}

unsigned int CMOOSCommClient::GetNumberOfUnsentMessages()
{
	m_InLock.Lock();
	unsigned int n = m_OutBox.size();
	m_InLock.UnLock();
	return n;

}

std::string CMOOSCommClient::GetDBHostNameAsSeenByDB() const{
    return m_sDBHostAsSeenByDB;
}

std::string CMOOSCommClient::GetDBHostname()  {
    return m_sDBHost;
}

int CMOOSCommClient::GetDBHostPort(){
    return m_lPort;
}

std::string CMOOSCommClient::GetClientName(){
    return m_sMyName;
}

uint64_t CMOOSCommClient::GetNumBytesSent()
{
	return m_nBytesSent;
}
uint64_t CMOOSCommClient::GetNumBytesReceived()
{
	return m_nBytesReceived;
}

uint64_t CMOOSCommClient::GetNumPktsReceived()
{
    return m_nPktsReceived;
}

uint64_t CMOOSCommClient::GetNumMsgsReceived()
{
    return m_nMsgsReceived;
}

uint64_t CMOOSCommClient::GetNumMsgsSent()
{
    return m_nMsgsSent;
}


//void CMOOSCommClient::SetOnConnectCallBack(bool (__cdecl *pfn)( void * pConnectParam), void * pConnectParam)
void CMOOSCommClient::SetOnConnectCallBack(bool (*pfn)( void * pConnectParam), void * pConnectParam)
{
	m_pfnConnectCallBack = pfn;
	m_pConnectCallBackParam = pConnectParam;
}

//void CMOOSCommClient::SetOnDisconnectCallBack(bool (__cdecl *pfn)( void * pConnectParam), void * pParam)
void CMOOSCommClient::SetOnDisconnectCallBack(bool ( *pfn)( void * pConnectParam), void * pParam)
{
	m_pfnDisconnectCallBack = pfn;
	m_pDisconnectCallBackParam = pParam;
}

bool CMOOSCommClient::IsRunning()
{
	return m_ClientThread.IsThreadRunning();
}


bool CMOOSCommClient::WaitUntilConnected(const unsigned int nMilliseconds)
{
    unsigned int k=0;
    while(!IsConnected())
    {
        if(k>nMilliseconds)
        {
            return false;
        }
        else
        {
            MOOSPause(100);
            k+=100;
        }
    }

    return true;
}

bool CMOOSCommClient::ClientLoop()
{


    double dfTDebug = MOOSLocalTime();

    if(m_bBoostIOThreads)
    {
    	MOOS::BoostThisThread();
    }

	while(!m_ClientThread.IsQuitRequested())
	{
		m_nBytesReceived=0;
		m_nBytesSent=0;

		//this is the connect loop...
		m_pSocket = new XPCTcpSocket(m_lPort);


		try
		{
			m_pSocket->vSetRecieveBuf(m_nReceiveBufferSizeKB*1024);
			m_pSocket->vSetSendBuf(m_nSendBufferSizeKB*1024);
		}
		catch(  XPCException & e)
		{
			std::cerr<<"there was trouble configuring socket buffers: "<<e.sGetException()<<"\n";
		}

		if(ConnectToServer())
		{

	        ApplyRecurrentSubscriptions();

			while(!m_bQuit)
			{


                if(m_bVerboseDebug)
                {
					MOOSTrace("COMMSCLIENT DEBUG: Tick period %f ms (should be %d ms)\n",MOOSLocalTime()-dfTDebug,(int)(1000.0/m_nFundamentalFreq));
                    dfTDebug = MOOSLocalTime();
                }

				if(!DoClientWork())
				{
					break;
				}

                if(m_bVerboseDebug)
                    MOOSTrace("COMMSCLIENT DEBUG: DoClientWork takes %fs\n",MOOSLocalTime()-dfTDebug);

				//wait a while before contacting server again;
                if(m_nFundamentalFreq==0)
                    m_nFundamentalFreq=1;

				MOOSPause((int)(1000.0/m_nFundamentalFreq));

			}
		}
		//wait one second before try to connect again
		MOOSPause(1000);


	}

	//clean up on exit....
	if(m_pSocket!=NULL)
	{
		if(m_pSocket)
			delete m_pSocket;
		m_pSocket = NULL;
	}

    if(!m_bQuiet)
        MOOSTrace("CMOOSCommClient::ClientLoop() quits\n");

    m_bConnected = false;

	return true;
}


void CMOOSCommClient::PrintMessageToActiveQueueRouting()
{
	std::map<std::string,std::set<std::string > >::iterator q;

	std::cerr<<MOOS::ConsoleColours::Green()<<"--- Message Routing for client \""<<GetMOOSName()<<"\" ---\n";
	std::cerr<<MOOS::ConsoleColours::reset();


	for(q = Msg2ActiveQueueName_.begin();q!=Msg2ActiveQueueName_.end();++q)
	{
		//get a list of all queues which handle this message
		std::set<std::string> & rQL = q->second;
		std::set<std::string>::iterator p;
		std::cerr<<std::setw(10)<< q->first<<" -> queues{ ";
		for(p = rQL.begin();p!=rQL.end();++p)
		{
			if(WildcardQueuePatterns_.find(*p)!=WildcardQueuePatterns_.end())
			{
				std::cerr<<MOOS::ConsoleColours::Magenta()<<"*";
			}
			std::cerr<< "\""<<*p<<"\"";
			std::cerr<<MOOS::ConsoleColours::reset()<<" ";
		}
		std::cerr<<"}\n";
	}

	std::cerr<<MOOS::ConsoleColours::reset();


}

bool CMOOSCommClient::RemoveActiveQueue(const std::string & sQueueName)
{
	MOOS::ScopedLock L(ActiveQueuesLock_);

	//maps message name to a list of queues...
	std::map<std::string,std::set<std::string > >::iterator q;
	for(q = Msg2ActiveQueueName_.begin();q!=Msg2ActiveQueueName_.end();++q)
	{
		//get a list of all queues which handle this message
		std::set<std::string> & rQL = q->second;
		std::set<std::string>::iterator p = rQL.find(sQueueName);
		if(p!=rQL.end())
		{
            rQL.erase(p);
		}
	}

    std::map<std::string,MOOS::ActiveMailQueue*>::iterator w =  ActiveQueueMap_.find(sQueueName);
    if(w!=ActiveQueueMap_.end())
    {
    	delete w->second;
    	ActiveQueueMap_.erase(w);
    }
    else
    {
        return false;
    }
    //and remove from wildcard queue (if it is there)
    WildcardQueuePatterns_.erase(sQueueName);

	return true;
}


bool CMOOSCommClient::AddMessageRouteToActiveQueue(const std::string & sQueueName,
				const std::string & sMsgName,
				bool (*pfn)(CMOOSMsg &M, void * pYourParam),
				void * pYourParam )
{
	if(!HasActiveQueue(sQueueName))
		AddActiveQueue(sQueueName,pfn,pYourParam);

	return AddMessageRouteToActiveQueue(sQueueName,sMsgName);

}


bool CMOOSCommClient::RemoveMessageRouteToActiveQueue(std::string const& sQueueName,
                                                      std::string const& sMsgName)
{
    if(!HasActiveQueue(sQueueName))
        return false;

    MOOS::ScopedLock L(ActiveQueuesLock_);

    std::map<std::string,std::set<std::string>  >::iterator w = Msg2ActiveQueueName_.find(sMsgName);

    if(w==Msg2ActiveQueueName_.end())
        return false;

    Msg2ActiveQueueName_.erase(w);

    return true;

}


//deprecated version
bool CMOOSCommClient::AddMessageCallBack(const std::string & sQueueName,
				const std::string & sMsgName,
				bool (*pfn)(CMOOSMsg &M, void * pYourParam),
				void * pYourParam )
{
	return AddMessageRouteToActiveQueue(sQueueName,sMsgName,pfn,pYourParam);
}

//add an active queue - don't forget that there is a templated version in hxx
//which uses a class member as a clallback
bool CMOOSCommClient::AddActiveQueue(const std::string & sQueueName,
				bool (*pfn)(CMOOSMsg &M, void * pYourParam),
				void * pYourParam )
{
	MOOS::ScopedLock L(ActiveQueuesLock_);

	std::map<std::string,MOOS::ActiveMailQueue*>::iterator w =  ActiveQueueMap_.find(sQueueName);
	if(w==ActiveQueueMap_.end())
	{
		//we need to create a new queue
		//std::cerr<<"making new active queue "<<sQueueName<<"\n";
		MOOS::ActiveMailQueue* pQ = new MOOS::ActiveMailQueue(sQueueName);
		ActiveQueueMap_[sQueueName] = pQ;

		pQ->SetCallback(pfn,pYourParam);
		pQ->Start();
		return true;
	}
	else
	{
		std::cerr<<"warning active queue "<<sQueueName<<" already exists\n";
		return false;
	}

}

bool CMOOSCommClient::AddWildcardActiveQueue(const std::string & sQueueName,
				const std::string & sPattern,
				bool (*pfn)(CMOOSMsg &M, void * pYourParam),
				void * pYourParam )
{
	if(!AddActiveQueue(sQueueName,pfn,pYourParam))
		return false;

	MOOS::ScopedLock L(ActiveQueuesLock_);

	WildcardQueuePatterns_[sQueueName]=sPattern;

	//now we had better see if this new wildcard queue is interested
	//in any messages we have already seen
	std::set< std::string>::iterator q;

	for(q=WildcardCheckSet_.begin();q!=WildcardCheckSet_.end();++q)
	{
		if(MOOSWildCmp(sPattern,*q))
		{
			Msg2ActiveQueueName_[*q].insert(sQueueName);
		}
	}


	return true;

}



bool CMOOSCommClient::AddMessageRouteToActiveQueue(const std::string & sQueueName,
		const std::string & sMsgName)
{
	if(HasActiveQueue(sQueueName))
	{
		//OK this queue exists
		MOOS::ScopedLock L(ActiveQueuesLock_);

		//now we can add the name of this Queue to list pointed
		//to by this message name
		Msg2ActiveQueueName_[sMsgName].insert(sQueueName);

		return true;
	}
	else
	{
		std::cerr<<"cannot add callback as queue "<<sQueueName<< " does not exist\n";
		return false;
	}
}


bool CMOOSCommClient::HasActiveQueue(const std::string & sQueueName)
{
	MOOS::ScopedLock L(ActiveQueuesLock_);
	return ActiveQueueMap_.find(sQueueName)!=ActiveQueueMap_.end();
}

bool CMOOSCommClient::DoClientWork()
{
	//this existence of this object makes this scope
	//a critical section - this allows ::flush to be called
	//safely
    MOOS::ScopedLock WL(m_WorkLock);

	//this is the IO Loop
	try
	{

		if(!IsConnected())
			return false;

		//note the symmetry here... a warm feeling
		CMOOSCommPkt PktTx,PktRx;

		m_OutLock.Lock();
		{
			//if nothing to send we send a NULL packet
			//just to tick things over..
			if(m_OutBox.empty())
			{
				//default msg is MOOS_NULL_MSG
				CMOOSMsg Msg;
				Msg.m_sSrc = m_sMyName;
				m_OutBox.push_front(Msg);
			}


			//convert our out box to a single packet
			try
			{
				PktTx.Serialize(m_OutBox,true);
				m_nMsgsSent+=PktTx.GetNumMessagesSerialised();
				m_nBytesSent+=PktTx.GetStreamLength();
			}
			catch (CMOOSException & e)
			{
				//clear the outbox
				m_OutBox.clear();
				throw CMOOSException("Serialisation Failed - this must be a lot of mail...");
			}

			//clear the outbox
			m_OutBox.clear();


		}
		m_OutLock.UnLock();

		double dfLocalPktTxTime = MOOSLocalTime();

        if(m_bVerboseDebug)
        {
            MOOSTrace("COMMSERVER DEBUG: instigated call in to DB at %f\n",dfLocalPktTxTime);
        }

        SendPkt(m_pSocket,PktTx);

        ReadPkt(m_pSocket,PktRx);

		m_nPktsReceived++;

#ifdef DEBUG_PROTOCOL_COMPRESSION
		MOOSTrace("Outgoing Compression = %.3f\n",PktTx.GetCompression());
		MOOSTrace("Incoming Compression = %.3f\n",PktRx.GetCompression());
#endif

		//quick! grab this time
		double dfLocalPktRxTime = MOOSLocalTime();

        if(m_bVerboseDebug)
        {
            MOOSTrace("COMMSERVER DEBUG: completed call to DB after %f s\n",dfLocalPktRxTime-dfLocalPktRxTime);
        }



		m_InLock.Lock();
		{
		    unsigned int num_pending = m_InBox.size();
			if(num_pending>m_nInPendingLimit)
			{
				MOOSTrace("Too many unread incoming messages [%d] : purging\n",num_pending);
				MOOSTrace("The user must read mail occasionally");
				m_InBox.clear();
			}

			//convert reply into a list of mesasges :-)
			//but no NULL messages
			//we ask serialise also to return the DB time
			//by looking in the first NULL_MSG in the packet
			double dfServerPktTxTime=numeric_limits<double>::quiet_NaN();


			m_nBytesReceived+=PktRx.GetStreamLength();

			//extract...
			PktRx.Serialize(m_InBox,false,true,&dfServerPktTxTime);

			m_nMsgsReceived+=m_InBox.size()-num_pending;

			//did you manage to grab the DB time while you were there?
            if(m_bDoLocalTimeCorrection && !MOOS::isnan(dfServerPktTxTime))
            {
				UpdateMOOSSkew(dfLocalPktTxTime, dfServerPktTxTime, dfLocalPktRxTime);
            }


			//here we dispatch to special call backs managed by threads
			DispatchInBoxToActiveThreads();


			m_bMailPresent = !m_InBox.empty();
		}
		m_InLock.UnLock();

        if(m_pfnMailCallBack!=NULL && m_bMailPresent)
        {
            bool bUserResult = (*m_pfnMailCallBack)(m_pMailCallBackParam);
            if(!bUserResult)
                MOOSTrace("user mail callback returned false..is all ok?\n");
        }


	}
	catch(CMOOSException & e)
	{
		MOOSTrace("Exception in ClientLoop() : %s\n",e.m_sReason);
		OnCloseConnection();
		return false;//jump out to connect loop....
	}

	return true;
}


bool CMOOSCommClient::DispatchInBoxToActiveThreads()
{


	//here we dispatch to special callbacks managed by threads

	MOOS::ScopedLock L(ActiveQueuesLock_);

#ifdef ENABLE_DETAILED_TIMING_AUDIT
    double time_now = MOOSLocalTime();
#endif
	//before we start we can see if we have a default queue installed...
	std::map<std::string, std::set<std::string> >::iterator q;

	MOOSMSG_LIST::iterator t = m_InBox.begin();

	//iterate over all pending messages.
	while(t!=m_InBox.end())
	{

#ifdef ENABLE_DETAILED_TIMING_AUDIT
    end_to_end_auditor_.AddForAudit(*t,m_sMyName,time_now);
#endif

//	    std::cerr<<"Inbox size:"<<m_InBox.size()<<"\n";
//	    t->Trace();

		//does this message have a active queue mapping?
		q= Msg2ActiveQueueName_.find(t->GetKey());

		//have we ever checked this message against the wildcard queues?
		std::set<std::string>::iterator u  =  WildcardCheckSet_.find(t->GetKey());

		if(q==Msg2ActiveQueueName_.end() || u==WildcardCheckSet_.end() )
		{
			//maybe the wildcard queues are interested?
			//or maybe this is a new message whihc has not been seen by wildcard queues

			//each element is a <nickname,pattern> string pair;
			std::map<std::string, std::string  >::iterator w;

			bool bFoundWCMatch = false;
			for(w = WildcardQueuePatterns_.begin();w!=WildcardQueuePatterns_.end();++w)
			{
				std::string sPattern = w->second;
				//build a list of all wc queues that match this message
				//add these queues to the list of queues pointed to by this message
				if(MOOSWildCmp(sPattern,t->GetKey()))
				{
//				    std::cerr<<"found wildcard match adding queue  "<<w->first
//				            <<" to routing for "<<t->GetKey()<<"\n";

					Msg2ActiveQueueName_[t->GetKey()].insert(w->first);
					bFoundWCMatch = true;
				}
			}

			//remember all messages that have been received...
			//we do it here because at this point wild card queue have been given
			//the option to register their interest....
			//but what to do if a wc queue in installed at run time...?
			WildcardCheckSet_.insert(t->GetKey());

			//std::cerr<<"added key"<<t->GetKey()<<" to wildcard chacek set\n";

			//if we found a least one mapping simply go again without
			//incrementing t...smart
			if(bFoundWCMatch)
			{
			    continue;
			}
			else
			{
			    if(q==Msg2ActiveQueueName_.end())
			    {
			        //wildcard queues are not interested
			        //no standard queue is interested
			        //nothing to do....
			        return true;
			    }
			}
		}

		//now we know which queue(s) are relevant for us.
		//there namaes are in a string list.
		std::set<std::string>::iterator r;

		bool bPickedUpByActiveQueue = false;
		for(r = q->second.begin();r!=q->second.end();++r)
		{

//		    std::cerr<<"found queue that is relevent "<<*r<<"\n";

		    //for each named queue find a pointer to
			//the actual active queue
			std::map<std::string,MOOS::ActiveMailQueue*>::iterator v;
			v = ActiveQueueMap_.find(*r);
			if(v!=ActiveQueueMap_.end())
			{
				//and now we have checked it exists push this message to that
				//queue
                MOOS::ActiveMailQueue* pQ = v->second;
//                std::cerr<<"pushing to queue: "<<(void*)pQ<<"\n";
                bPickedUpByActiveQueue = true;
				pQ->Push(*t);
			}
			else
			{
				//this is bad news - we have be told to use a queue
				//which does not exist.
			    //std::cerr<<"WTF\n";
				throw std::runtime_error("active queue "+*r+" not found");
			}
		}


		if(bPickedUpByActiveQueue)
		{
	        //we have now handled this message remove it from the Inbox.
		    MOOSMSG_LIST::iterator to_erase = t;
		    ++t;
		    m_InBox.erase(to_erase);
		}
		else
		{
		    ++t;
		}
	}

	return true;
}


bool CMOOSCommClient::StartThreads()
{
	m_bQuit = false;
    if(!m_ClientThread.Initialise(ClientLoopProc,this))
        return false;
    if(!m_ClientThread.Start())
        return false;


    return true;
}

bool CMOOSCommClient::ConnectToServer()
{
	if(IsConnected())
	{
		MOOSTrace("attempt to connect to server whilst already connected...\n");
		return true;
	}

	int nAttempt=0;



    if(!m_bQuiet)
	    MOOSTrace("\n-------------- moos connect ----------------------\n");

	while(!m_bQuit)
	{
        if(!m_bQuiet)
		    MOOSTrace("  contacting a MOOS server %s:%ld -  try %.5d ",m_sDBHost.c_str(),m_lPort,++nAttempt);

        if(m_bDisableNagle)
        	m_pSocket->vSetNoDelay(1);

        try
		{
			m_pSocket->vConnect(m_sDBHost.c_str());
			break;
		}
		catch(const XPCException & e)
		{
			//connect failed....
		    UNUSED_PARAMETER(e);

			if(m_pSocket)
				delete m_pSocket;

			m_pSocket = new XPCTcpSocket(m_lPort);

			MOOSPause(1000);
			MOOSTrace("\r");
		}
	}

	if(m_bQuit)
	{
		MOOSTrace("ConnectToServer returns early\n");
		return false;
	}


	if(HandShake())
	{
	    if(!m_bQuiet)
	        MOOSTrace("--------------------------------------------------\n\n");

		//suggestion to move this to here accpted by PMN on 21st Jan 2009
        //we must be connected for user callback to work..
        m_bConnected = true;
        m_dfLastConnectionTime = MOOSLocalTime();


        if(m_pfnConnectCallBack!=NULL)
		{
			//invoke user defined callback
			bool bUserResult = (*m_pfnConnectCallBack)(m_pConnectCallBackParam);
			if(!bUserResult)
			{
	            if(!m_bQuiet)
	                MOOSTrace("  Invoking User OnConnect() callback...FAIL");
			}

		}

        //look to turn on status monitoring
        ControlClientCommsStatusMonitoring(m_bMonitorClientCommsStatus);

	}
	else
	{
	    if(!m_bQuiet)
	        MOOSTrace("--------------------------------------------------\n\n");

        m_bQuit = true;

		if(m_pSocket)
			delete m_pSocket;

		m_pSocket = new XPCTcpSocket(m_lPort);
		return false;
	}

	return true;
}

/** this is called by user of a CommClient object
to send a Msg to MOOS */
bool CMOOSCommClient::Post(CMOOSMsg &Msg, bool bKeepMsgSourceName)
{
	if(!IsConnected())
		return false;

	m_OutLock.Lock();

	//stuff our name in here  - prevent client from having to worry about
	//it...
	if(!m_bFakeSource && !bKeepMsgSourceName )
	{
		Msg.m_sSrc = m_sMyName;
	}
	else
	{
		if(!Msg.IsType(MOOS_NOTIFY))
		{
			Msg.m_sSrc = m_sMyName;
		}
	}


	if(Msg.IsType(MOOS_SERVER_REQUEST))
	{
		Msg.m_nID=MOOS_SERVER_REQUEST_ID;
	}
	else
	{
		//set up Message ID;
		Msg.m_nID=m_nNextMsgID++;
	}


	if(m_bPostNewestToFront)
		m_OutBox.push_front(Msg);
	else
		m_OutBox.push_back(Msg);

	if(m_OutBox.size()>m_nOutPendingLimit)
	{
        if(!m_bExpectMailBoxOverFlow)
        {
            MOOSTrace("\nThe outbox is very full. This is suspicious and dangerous.\n");
            MOOSTrace("\nRemoving old unsent messages as new ones are added\n");
        }
		//remove oldest message...

		if(m_bPostNewestToFront)
			m_OutBox.pop_back();
		else
			m_OutBox.pop_front();
	}

	m_OutLock.UnLock();

	return true;

}

bool IsNullMsg(const CMOOSMsg& msg)
{
	return msg.IsType(MOOS_NULL_MSG);
}
/** this is called by a user of a CommClient object
to retrieve mail */
bool CMOOSCommClient::Fetch(MOOSMSG_LIST &MsgList)
{

	if(!m_bMailPresent)
		return false;

	MsgList.clear();

	m_InLock.Lock();

	MOOSMSG_LIST::iterator p;

	m_InBox.remove_if(IsNullMsg);


	MsgList.splice(MsgList.begin(),m_InBox,m_InBox.begin(),m_InBox.end());

	//remove all elements
	m_InBox.clear();

	m_bMailPresent = false;

	m_InLock.UnLock();

	return !MsgList.empty();
}

std::string CMOOSCommClient::HandShakeKey()
{
	//old MOOS Clients return empty string
	return "";
}

bool CMOOSCommClient::HandShake()
{
	try
	{
        if(m_bDoLocalTimeCorrection)
            SetMOOSSkew(0);

        if(!m_bQuiet)
        {
            std::cout<<"\n";
            std::cout<<std::left<<std::setw(40)<<("  Handshaking as "+m_sMyName);
        }

		//announce the protocl we will be talking...
		// We use strncpy to explicitly pad the destination buffer with
		// nulls, so the handshake message will not contain random bytes.
		char buff[MOOS_PROTOCOL_STRING_BUFFER_SIZE];
		strncpy(buff, MOOS_PROTOCOL_STRING, MOOS_PROTOCOL_STRING_BUFFER_SIZE);
		m_pSocket->iSendMessage(buff, MOOS_PROTOCOL_STRING_BUFFER_SIZE);

		//a little bit of handshaking..we need to say who we are
		CMOOSMsg Msg(MOOS_DATA,HandShakeKey(),(char *)m_sMyName.c_str());

		SendMsg(m_pSocket,Msg);

		CMOOSMsg WelcomeMsg;

		ReadMsg(m_pSocket,WelcomeMsg);

		if(WelcomeMsg.IsType(MOOS_POISON))
		{
            if(!m_bQuiet)
            {
            	std::cerr<<MOOS::ConsoleColours::Red()<<"[fail]\n";
            	std::cerr<<"    \""<<WelcomeMsg.m_sVal<<"\"\n";
            	std::cerr<<MOOS::ConsoleColours::reset();
            }
			return false;
		}
		else
		{


			m_sCommunityName = WelcomeMsg.GetCommunity();


            m_bDBIsAsynchronous = MOOSStrCmp(WelcomeMsg.GetString(),"asynchronous");
            MOOSValFromString(m_sDBHostAsSeenByDB,WelcomeMsg.m_sSrcAux,"hostname",true);

			if(!m_bQuiet)
			{
				std::cout<<MOOS::ConsoleColours::Green()<<"[ok]\n";
	            std::cout<<MOOS::ConsoleColours::reset();

			}

            if(!m_bQuiet)
            {
                std::cout<<std::left<<std::setw(40);
            	std::cout<<"  DB reports async support is  ";
            	if(m_bDBIsAsynchronous)
            	{
                    std::cout<<MOOS::ConsoleColours::Green()<<"[on]\n";
            	}
            	else
            	{
                    std::cout<<MOOS::ConsoleColours::Red()<<"[off]\n";
            	}

                std::cout<<MOOS::ConsoleColours::reset();


            	if(!WelcomeMsg.m_sSrcAux.empty())
            	{

                    std::cout<<std::left<<std::setw(40);
                    std::cout<<"  DB is running on ";
                    std::cout<<MOOS::ConsoleColours::Green()<<m_sDBHostAsSeenByDB<<"\n";
                    std::cout<<MOOS::ConsoleColours::reset();

                    std::cout<<std::left<<std::setw(40);

                    std::cout<<"  Timing skew estimation is ";
                    if( GetLocalIPAddress()!=m_sDBHostAsSeenByDB)
                    {
                        std::cout<<MOOS::ConsoleColours::Green()<<"[on]\n";
                        std::cout<<MOOS::ConsoleColours::reset();
                        DoLocalTimeCorrection(true);
                    }
                    else
                    {
                        std::cout<<MOOS::ConsoleColours::yellow();
                        std::cout<<"[off] (not needed)\n";
                        DoLocalTimeCorrection(false);
                    }
            	}
                std::cout<<MOOS::ConsoleColours::reset();
            }

            //read our skew
            double dfSkew = WelcomeMsg.m_dfVal;
            if(m_bDoLocalTimeCorrection)
                SetMOOSSkew(dfSkew);


		}
	}
	catch(CMOOSException & e)
	{
		MOOSTrace("Exception in hand shaking : %s",e.m_sReason);
		return false;
	}
	return true;

}

bool CMOOSCommClient::IsConnected()
{
	return m_bConnected;
}

std::string CMOOSCommClient::GetCommunityName()
{
	if(IsConnected())
		return m_sCommunityName;
	else
		return "";
}

bool CMOOSCommClient::OnCloseConnection()
{
	m_pSocket->vCloseSocket();

	if(m_pSocket)
		delete m_pSocket;

	m_pSocket= NULL;
	m_bConnected = false;

	ClearResources();

	bool bUserResult = true;
	if(m_pfnDisconnectCallBack!=NULL)
	{
		//invoke user defined callback
		bUserResult = (*m_pfnDisconnectCallBack)(m_pDisconnectCallBackParam);
	}


	return bUserResult;
}

void CMOOSCommClient::DoBanner()
{
//    if(m_bQuiet)
//        return ;

    return;
//	MOOSTrace("****************************************************\n");
//	MOOSTrace("*       This is MOOS Client                        *\n");
//	MOOSTrace("*       c. P Newman 2001-2012                      *\n");
//	MOOSTrace("****************************************************\n");

}

bool CMOOSCommClient::UnRegister(const string &sVar)
{
	if(!IsConnected())
		return false;

	if(m_Registered.find(sVar)==m_Registered.end() || m_Registered.empty())
	{
		return true;
	}

	CMOOSMsg MsgUR(MOOS_UNREGISTER,sVar.c_str(),0.0);
	if(Post(MsgUR))
	{
		m_Registered.erase(sVar);
		return true;
	}
	else
	{
		return false;
	}

}

bool CMOOSCommClient::UnRegister(const string &sVarPattern, const string & sAppPattern)
{
	if(!IsConnected())
		return false;

	if(m_Registered.empty())
	{
		return true;
	}

	std::string sMsg;

	MOOSAddValToString(sMsg,"AppPattern",sAppPattern);
	MOOSAddValToString(sMsg,"VarPattern",sVarPattern);
	MOOSAddValToString(sMsg,"Interval",0.0);

	CMOOSMsg MsgUR(MOOS_WILDCARD_UNREGISTER,m_sMyName,sMsg);

	return Post(MsgUR);

}




bool CMOOSCommClient::Register(const string &sVar, double dfInterval)
{
	if(!IsConnected())
		return false;

	if(sVar.empty())
		return MOOSFail("\n ** WARNING ** Cannot register for \"\" (empty string)\n");

	//if(m_Registered.find(sVar)==m_Registered.end() || m_Registered.empty())
	if(1)
	{
		CMOOSMsg MsgR(MOOS_REGISTER,sVar.c_str(),dfInterval);
		bool bSuccess =  Post(MsgR);
		if(bSuccess)
		{
			m_Registered.insert(sVar);
		}
		return bSuccess;
	}
	else
	{
		return false;
	}
}

bool CMOOSCommClient::Register(const std::string & sVarPattern,const std::string & sAppPattern, double dfInterval)
{
	std::string sMsg;

	if(sVarPattern.empty())
	    return MOOSFail("empty variable pattern in CMOOSCommClient::Register");

    if(sAppPattern.empty())
        return MOOSFail("empty source pattern in CMOOSCommClient::Register");


    MOOSAddValToString(sMsg,"AppPattern",sAppPattern);
    MOOSAddValToString(sMsg,"VarPattern",sVarPattern);
    MOOSAddValToString(sMsg,"Interval",dfInterval);

    CMOOSMsg MsgR(MOOS_WILDCARD_REGISTER,m_sMyName,sMsg);


    return Post(MsgR);
}



bool CMOOSCommClient::IsRegisteredFor(const std::string & sVariable)
{
    return !m_Registered.empty() && m_Registered.find(sVariable)!=m_Registered.end();
}

bool CMOOSCommClient::Notify(const string &sVar, double dfVal, double dfTime)
{
	CMOOSMsg Msg(MOOS_NOTIFY,sVar.c_str(),dfVal,dfTime);

	m_Published.insert(sVar);

	return Post(Msg);

}



bool CMOOSCommClient::Notify(const std::string & sVar,double dfVal, const std::string & sSrcAux,double dfTime)
{
	CMOOSMsg Msg(MOOS_NOTIFY,sVar.c_str(),dfVal,dfTime);

	Msg.SetSourceAux(sSrcAux);

	m_Published.insert(sVar);

	return Post(Msg);
}



bool CMOOSCommClient::Notify(const string &sVar, const string & sVal, double dfTime)
{
	CMOOSMsg Msg(MOOS_NOTIFY,sVar.c_str(),sVal.c_str(),dfTime);

	m_Published.insert(sVar);

	return Post(Msg);
}



bool CMOOSCommClient::Notify(const std::string &sVar, const std::string & sVal, const std::string & sSrcAux, double dfTime)
{
	CMOOSMsg Msg(MOOS_NOTIFY,sVar.c_str(),sVal.c_str(),dfTime);

	Msg.SetSourceAux(sSrcAux);

	m_Published.insert(sVar);

	return Post(Msg);
}

bool CMOOSCommClient::Notify(const std::string &sVar, const char * sVal,double dfTime)
{
	return Notify(sVar,std::string(sVal),dfTime);
}

bool CMOOSCommClient::Notify(const std::string &sVar, const char * sVal,const std::string & sSrcAux, double dfTime)
{
	return Notify(sVar,std::string(sVal),sSrcAux,dfTime);
}



bool CMOOSCommClient::Notify(const string &sVar, void * pData,unsigned int nSize, double dfTime)
{
	std::string BinaryPayload((char*)pData,nSize);

	CMOOSMsg Msg(MOOS_NOTIFY,sVar,BinaryPayload,dfTime);

	Msg.MarkAsBinary();

	m_Published.insert(sVar);

	return Post(Msg);

}


bool CMOOSCommClient::Notify(const string &sVar, void * pData,unsigned int nSize, const std::string & sSrcAux,double dfTime)
{

    CMOOSMsg Msg(MOOS_NOTIFY,sVar,nSize,pData,dfTime);

	Msg.SetSourceAux(sSrcAux);
    Msg.MarkAsBinary();

	m_Published.insert(sVar);

	return Post(Msg);
}


bool CMOOSCommClient::Notify(const std::string & sVar,const std::vector<unsigned char>& vData,double dfTime)
{
	if(vData.empty())
		return false;

	return Notify(sVar,(void*) (&vData[0]),vData.size(), dfTime);
}

bool CMOOSCommClient::Notify(const std::string & sVar,const std::vector<unsigned char>& vData, const std::string & sSrcAux,double dfTime)
{
	if(vData.empty())
		return false;

	return Notify(sVar,(void*) (&vData[0]),vData.size(),sSrcAux,dfTime);
}




bool CMOOSCommClient::ServerRequest(const string &sWhat,MOOSMSG_LIST  & MsgList, double dfTimeOut, bool bClear)
{
    if(!IsConnected())
        return false;

    CMOOSMsg Msg(MOOS_SERVER_REQUEST,sWhat.c_str(),"");

	if(!Post(Msg))
		return false;

	if(!Flush())
		return false;

	if(Msg.m_nID != MOOS_SERVER_REQUEST_ID)
	{
		return MOOSFail("Logical Error in ::ServerRequest");
	}

	int nSleep = 100;

	double dfWaited = 0.0;

	while(dfWaited<dfTimeOut)
	{
		if (Peek(MsgList, MOOS_SERVER_REQUEST_ID, bClear))
		{
			//OK we have our reply...
			return true;
		}
		else
		{
			MOOSPause(nSleep);
			dfWaited+=((double)nSleep)/1000.0;

		}
	}

	return false;
}

bool CMOOSCommClient::Peek(MOOSMSG_LIST & MsgList, int nIDRequired,bool bClear)
{
	MsgList.clear();

	m_InLock.Lock();

	MOOSMSG_LIST::iterator p,q;

	p=m_InBox.begin();
	while(p!=m_InBox.end())
	{
		if(!p->IsType(MOOS_NULL_MSG))
		{
			//only give client non NULL Msgs
			if(p->m_nID==nIDRequired)
			{
				//this is the correct ID!
				MsgList.push_front(*p);
				q=p++;
				m_InBox.erase(q);
				continue;
			}
		}
		++p;
	}

	//conditionally (ex MIT suggestion 2006) remove all elements
	if(bClear)
		m_InBox.clear();


	m_InLock.UnLock();

	return !MsgList.empty();
}

//a static helper function
bool CMOOSCommClient::PeekMail(MOOSMSG_LIST &Mail,
							   const string &sKey,
							   CMOOSMsg &Msg,
							   bool bRemove,
							   bool bFindYoungest )
{
	MOOSMSG_LIST::iterator p;
	MOOSMSG_LIST::iterator q =Mail.end();

	double dfYoungest = -1;

	for(p = Mail.begin();p!=Mail.end();++p)
	{
		if(p->m_sKey==sKey)
		{
			//might want to consider more than one msg....

			if(bFindYoungest)
			{
				if(p->m_dfTime>dfYoungest)
				{
					dfYoungest=p->m_dfTime;
					q = p;
				}
			}
			else
			{
				//simply take first
				q=p;
				break;
			}

		}
	}

	if(q!=Mail.end())
	{
		Msg=*q;

		if(bRemove)
		{
			//Mail.erase(p);
			Mail.erase(q);
		}
		return true;

	}

	return false;
}



bool CMOOSCommClient::PeekAndCheckMail(MOOSMSG_LIST &Mail, const std::string &sKey, CMOOSMsg &Msg,bool bErase , bool bFindYoungest)
{
    if(PeekMail(Mail,sKey,Msg,bErase,bFindYoungest))
        return(!Msg.IsSkewed(MOOSTime()-5.0));
    else
        return false;
}

bool CMOOSCommClient::Close(bool  )
{

	m_bQuit = true;

	if(m_ClientThread.IsThreadRunning())
		m_ClientThread.Stop();

	ClearResources();

	MOOS::ScopedLock L(ActiveQueuesLock_);

	std::map<std::string,MOOS::ActiveMailQueue*  >::iterator q;

	for(q = ActiveQueueMap_.begin();q!=ActiveQueueMap_.end();++q)
	{
		MOOS::ActiveMailQueue* pQueue = q->second;
		pQueue->Stop();
		delete pQueue;
	}

	ActiveQueueMap_.clear();
	Msg2ActiveQueueName_.clear();
	WildcardCheckSet_.clear();


	return true;
}

bool CMOOSCommClient::FakeSource(bool bFake)
{
	m_bFakeSource = bFake;
	return true;
}

bool CMOOSCommClient::ClearResources()
{
	m_OutLock.Lock();
		m_OutBox.clear();
	m_OutLock.UnLock();

	m_InLock.Lock();
		m_InBox.clear();
	m_InLock.UnLock();


	m_Registered.clear();

    m_sDBHostAsSeenByDB.clear();

	return true;

}

string CMOOSCommClient::GetDescription()
{
    //pmn makes this more of a standard thing in May 2009  - use of : instead of @
	return MOOSFormat("%s:%ld",m_sDBHost.c_str(),m_lPort);
}

bool CMOOSCommClient::Flush()
{
	return DoClientWork();
}


//std::auto_ptr<std::ofstream> SkewLog(NULL);

bool CMOOSCommClient::UpdateMOOSSkew(double dfRqTime, double dfTxTime, double dfRxTime)
{


	double dfOldSkew = GetMOOSSkew();

	// This function needs to be provided MOOSLocal time stamps!

	//back out correction which has already been made..
	//dfRqTime-=dfOldSkew;
	//dfRxTime-=dfOldSkew;
//#define MOOS_DETECT_CLOCK_DRIFT
#ifdef MOOS_DETECT_CLOCK_DRIFT

	// This is an experimental and unfinished feature.  It tracks the drift between
	// clocks on client and server, in order to provide highly accurate time stamps
	// at the client, which are within about 0.1ms of the server time.
	//
	// In its current implementation, the filter begins to suffer from numerical
	// issues after around 3 or 4 hours of use.  This is known and expected behaviour
	// and will be fixed soon.
	//
	// arh 30/03/2009

	if (!m_pSkewFilter.get())
	{
		// Make a fresh skew filter
		//m_pSkewFilter = std::auto_ptr<MOOS::CMOOSSkewFilter>(new MOOS::CMOOSConditionedSkewFilter);
		m_pSkewFilter = MOOS::ScopedPtr<MOOS::CMOOSSkewFilter>(new MOOS::CMOOSSkewFilter);
		if (!m_pSkewFilter.get())
			return false;
	}

	MOOS::CMOOSSkewFilter::tSkewInfo skewinfo;
	double dfNewSkew = m_pSkewFilter->Update(dfRqTime, dfTxTime, dfRxTime, &skewinfo);

#else // MOOS_DETECT_CLOCK_DRIFT

	double dfMeasuredSkewA = dfTxTime-dfRxTime;
	double dfMeasuredSkewB = dfTxTime-dfRqTime;
	double dfMeasuredSkew  = 0.5*(dfMeasuredSkewA+dfMeasuredSkewB);

	double dfNewSkew;
	if(dfOldSkew!=0.0)
	{
		dfNewSkew = 0.9*dfOldSkew+0.1*dfMeasuredSkew;
	}
	else
	{
		dfNewSkew = dfMeasuredSkew;
	}




#endif // MOOS_DETECT_CLOCK_DRIFT

//	MOOSTrace("\n%s\nTx Time = %.4f \nDB time = %.4f\nreply = %.4f\nskew = %.5f\n",
//			m_sMyName.c_str(),
//			dfRqTime,
//			dfTxTime,
//			dfRxTime,
//			dfNewSkew);
//
//	MOOSTrace("local = %.4f\nMOOS = %.4f\n ", MOOSLocalTime(false), MOOS::Time());
//
//
//
//	std::cerr<<GetLocalIPAddress()<<"\n";

/*
	if (SkewLog.get())
	{
	    SkewLog->setf(std::ios::fixed);
	    (*SkewLog) <<
		"RQ=" << setprecision(9) << dfRqTime << "," <<
		"TX=" << setprecision(9) << dfTxTime << "," <<
		"RX=" << setprecision(9) << dfRxTime << "," <<
#ifdef MOOS_DETECT_CLOCK_DRIFT
		"m=" << setprecision(9) << skewinfo.m << "," <<
		"c=" << setprecision(9) << skewinfo.c << "," <<
		"LB=" << setprecision(9) << skewinfo.LB << "," <<
		"UB=" << setprecision(9) << skewinfo.UB << "," <<
		"envLB=" << setprecision(9) << skewinfo.envLB << "," <<
		"envUB=" << setprecision(9) << skewinfo.envUB << "," <<
		"envEst=" << setprecision(9) << skewinfo.envEst << "," <<
		"filtEst=" << setprecision(9) << skewinfo.filtEst <<
#endif // MOOS_DETECT_CLOCK_DRIFT
		std::endl;
	}
*/

	SetMOOSSkew(dfNewSkew);

	return true;
}

bool CMOOSCommClient::SetCommsControlTimeWarpScaleFactor(double dfSF)
{

    if(dfSF<0.0|| dfSF>MAX_TIME_WARP_AGGLOMERATION_CONSTANT)
    {
        std::cerr<<MOOS::ConsoleColours::Red();
        std::cerr<<"Warning: Comms Scale factor out of range (0:10.0\n";
        std::cerr<<MOOS::ConsoleColours::reset();
        return false;
    }

    m_dfOutGoingDelayTimeWarpScaleFactor = dfSF;
    return true;
}


double CMOOSCommClient::GetCommsControlTimeWarpScaleFactor()
{
    return m_dfOutGoingDelayTimeWarpScaleFactor;
}


bool CMOOSCommClient::ControlClientCommsStatusMonitoring(bool bEnable)
{
    if(bEnable)
    {

        if(!AddRecurrentSubscription("DB_QOS",0.0))
            return false;

        if(!AddRecurrentSubscription("DB_RWSUMMARY",0.0))
            return false;

        if(HasActiveQueue("_ClientSummaries"))
            return true;

        if(!AddActiveQueue("_ClientSummaries",this,&CMOOSCommClient::ProcessClientCommsStatusSummary))
            return false;

        if(!AddMessageRouteToActiveQueue("_ClientSummaries","DB_QOS"))
            return false;

        if(!AddMessageRouteToActiveQueue("_ClientSummaries","DB_RWSUMMARY"))
            return false;

        ApplyRecurrentSubscriptions();
    }
    else
    {
        if(HasActiveQueue("_ClientSummaries"))
            return RemoveActiveQueue("_ClientSummaries");
    }

    return true;
}

bool CMOOSCommClient::GetClientCommsStatus(const std::string & sClient, MOOS::ClientCommsStatus & TheStatus)
{
    MOOS::ScopedLock L(m_ClientStatusLock);
    std::map<std::string , MOOS::ClientCommsStatus>::iterator q = m_ClientStatuses.find(sClient);

    if (q==m_ClientStatuses.end())
        return false;

    TheStatus = q->second;

    return true;
}

void CMOOSCommClient::GetClientCommsStatuses(std::list<MOOS::ClientCommsStatus> & Statuses)
{
    MOOS::ScopedLock L(m_ClientStatusLock);
    std::map<std::string , MOOS::ClientCommsStatus>::iterator q;

    for(q=m_ClientStatuses.begin();q!=m_ClientStatuses.end();++q)
        Statuses.push_back(q->second);

}


void CMOOSCommClient::EnableCommsStatusMonitoring(bool bEnable)
{
    m_bMonitorClientCommsStatus = bEnable;
    //ControlClientCommsStatusMonitoring(bEnable);
}


bool CMOOSCommClient::ProcessClientCommsStatusSummary(CMOOSMsg & M)
{
    MOOS::ScopedLock L(m_ClientStatusLock);
    if(M.GetName()=="DB_QOS")
    {
        while(!M.m_sVal.empty())
        {
            std::string sT = MOOSChomp(M.m_sVal);
            if(sT.empty())
                break;
            std::string sC  = MOOSChomp(sT,"=");

            if(sC.empty())
                return MOOSFail("CMOOSCommClient::ProcessClientSummary empty client name");

            MOOS::ClientCommsStatus & rS = m_ClientStatuses[sC];

            rS.name_=sC;
            rS.recent_latency_ =MOOS::StringToDouble(MOOSChomp(sT,":"));
            rS.max_latency_ =   MOOS::StringToDouble(MOOSChomp(sT,":"));
            rS.min_latency_ =   MOOS::StringToDouble(MOOSChomp(sT,":"));
            rS.avg_latency_ =   MOOS::StringToDouble(MOOSChomp(sT,":"));

        }
    }
    else if(M.GetName()=="DB_RWSUMMARY")
    {
        while(!M.m_sVal.empty())
        {
            std::string sT = MOOSChomp(M.m_sVal);
            if(sT.empty())
                break;
            std::string sC  = MOOSChomp(sT,"=");
            std::string sSub  = MOOSChomp(sT,"&");
            std::string sPub  = MOOSChomp(sT,"&");


            if(sC.empty())
                return MOOSFail("CMOOSCommClient::ProcessClientSummary empty client name");

            MOOS::ClientCommsStatus & rS = m_ClientStatuses[sC];

            rS.subscribes_.clear();
            rS.publishes_.clear();

//            std::cerr<<"sSubs="<<sSub<<"\n";
//            std::cerr<<"sPub="<<sPub<<"\n";

            while (!sSub.empty())
            {
                rS.subscribes_.push_back(MOOSChomp(sSub,":"));
            }

            while (!sPub.empty())
            {
                rS.publishes_.push_back(MOOSChomp(sPub,":"));
            }


            //rS.Write(std::cerr);

        }
    }
    return true;
}


bool CMOOSCommClient::ApplyRecurrentSubscriptions()
{

    MOOS::ScopedLock L(RecurrentSubscriptionLock);
    std::map< std::string, double >::iterator q;
    for(q = m_RecurrentSubscriptions.begin();q!=m_RecurrentSubscriptions.end();++q)
    {
        if(!Register(q->first,q->second))
            return false;
    }

    return true;
}


bool CMOOSCommClient::AddRecurrentSubscription(const std::string &sVar, double dfPeriod)
{
    if(sVar.empty())
        return false;

    MOOS::ScopedLock L(RecurrentSubscriptionLock);

    m_RecurrentSubscriptions[sVar] = dfPeriod;

    return true;

}
bool CMOOSCommClient::RemoveRecurrentSubscription(const std::string & sVar)
{

    if(sVar.empty())
        return false;

    MOOS::ScopedLock L(RecurrentSubscriptionLock);

    if(m_RecurrentSubscriptions.find(sVar)==m_RecurrentSubscriptions.end())
        return false;
    else
        m_RecurrentSubscriptions.erase(sVar);

    return true;
}



