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
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/




/*
 * ThreadedCommServer.cpp
 *
 *  Created on: Aug 29, 2011
 *      Author: pnewman
 */


#include "MOOS/libMOOS/Utils/MOOSException.h"
#include "MOOS/libMOOS/Comms/XPCTcpSocket.h"
#include "MOOS/libMOOS/Comms/ThreadedCommServer.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"
#include "MOOS/libMOOS/Utils/ThreadPriority.h"
#include <iomanip>
#include <iterator>
#include <algorithm>

#ifndef _WIN32
    #include <sys/signal.h>
#endif

#ifdef _WIN32
#define INVALID_SOCKET_SELECT WSAEINVAL
#pragma warning(disable:4018) // signed/unsigned comparison
#pragma warning(disable:4389) // signed/unsigned comparison
#pragma warning(disable:4127) // conditional expression is constant
#else
#define INVALID_SOCKET_SELECT EBADF
#endif


namespace MOOS
{

ThreadPrint gPrinter(std::cout);


ThreadedCommServer::ThreadedCommServer()
{
    // TODO Auto-generated constructor stub
}

ThreadedCommServer::~ThreadedCommServer()
{
    // TODO Auto-generated destructor stub
    Stop();

}
bool ThreadedCommServer::Stop()
{

    //kill this first because we have to prevent client from reconnecting
    //because we are going to pull the plug on them
   if(m_ListenThread.IsThreadRunning())
       m_ListenThread.Stop();

   if(m_pListenSocket!=NULL)
   {
       m_pListenSocket->vCloseSocket();
       delete m_pListenSocket;
       m_pListenSocket = NULL;
   }


   //then pull the plug on the loop which will spot clients disconnecting
   if(m_ServerThread.IsThreadRunning())
       m_ServerThread.Stop();

   //now shut down each of the client threads in turn
    std::map<std::string,ClientThread*>::iterator q;
    for(q=m_ClientThreads.begin();q!=m_ClientThreads.end();q++)
    {
        q->second->Kill();
        delete q->second;
    }
    m_ClientThreads.clear();

    //maybe the base class has other business
    return BASE::Stop();
}


/**
 * called when a new client connects. Our job is to make a new thread which will handle
 * all communications with the client. This will stop slow comms slowing everything up
 * @param pNewClient
 * @param sName
 * @return
 */
bool ThreadedCommServer::OnNewClient(XPCTcpSocket * pNewClient,char * sName)
{
    if(!BASE::OnNewClient(pNewClient,sName))
    {
        return false;
    }

    //after handshaking we will know what this client is called
    std::string sClientName = GetClientName(pNewClient);


    //here we need to start a new thread to handle our comms...
    return AddAndStartClientThread(*pNewClient,sClientName);

}

/**
 * The mechanics of starting a new thread to handle client communications
 * @param pNewClient
 * @param sName
 * @return
 */
bool ThreadedCommServer::AddAndStartClientThread(XPCTcpSocket & NewClientSocket,const std::string & sName)
{

    std::map<std::string,ClientThread*>::iterator q = m_ClientThreads.find(sName);


    if(q != m_ClientThreads.end() )
    {
        ClientThread* pExistingClientThread = q->second;

        if(pExistingClientThread)
        {
            MOOSTrace("worrying condition ::AddAndStartClientThread() killing an existing client thread");

            //stop the thread
            pExistingClientThread->Kill();

            //free up the heap (old skool - good grief)
            delete pExistingClientThread;

            //remove from map
            m_ClientThreads.erase(q);
        }
        else
        {
            return MOOSFail("logical error ::AddAndStartClientThread() NULL thread pointer");
        }
    }

    //make a new client - and show it where to put data and how to talk to a client
    bool bAsync = m_AsynchronousClientSet.find(sName)!=m_AsynchronousClientSet.end();

    //we need to look up timing information
    double dfConsolidationTime = 0.0;
    std::list< std::pair< std::string, double >  >::iterator v;
    for(v = m_ClientTimingVector.begin();v!=m_ClientTimingVector.end();v++)
    {
    	if(MOOSWildCmp(v->first,sName))
    	{
    		dfConsolidationTime=v->second;
    		break;
    	}
    }


    ClientThread* pNewClientThread =  new  ClientThread(sName,
    		NewClientSocket,
    		m_SharedDataListFromClient,
    		bAsync,
    		dfConsolidationTime,
    		m_dfClientTimeout,
    		m_bBoostIOThreads);

    //add to map
    m_ClientThreads[sName] = pNewClientThread;


    return pNewClientThread->Start();


}

/**
 * This is the main loop - it looks for complete Pkt being placed in the incoming list
 * and invokes a handler
 * @return true on exit
 */
bool ThreadedCommServer::ServerLoop()
{



	m_Auditor.SetQuiet(m_bQuiet);
    m_Auditor.Run("localhost",m_nAuditPort);

    if(m_bBoostIOThreads)
    {
    	MOOS::BoostThisThread();
    }

	//eternally look at our incoming work list....
	while(!m_ServerThread.IsQuitRequested())
    {
        ClientThreadSharedData SDFromClient;

       
        if(m_SharedDataListFromClient.IsEmpty())
        {
            if(!m_SharedDataListFromClient.WaitForPush(1000))
                continue;
        }

        m_SharedDataListFromClient.Pull(SDFromClient);

        switch(SDFromClient._Status)
        {
        case ClientThreadSharedData::PKT_READ:
        {
            ProcessClient(SDFromClient,m_Auditor);
            break;
        }

        case ClientThreadSharedData::CONNECTION_CLOSED:
            OnClientDisconnect(SDFromClient);
            m_Auditor.Remove(SDFromClient._sClientName);
            break;

        default:
            break;
        }

    }

    return true;

}



/**
 * the main handler  - a Pkt has been fetch off the work list (and is in SD)
 * we now invoke a callback and then place the return packet in the
 * same basket
 * @param SD
 * @return
 */
bool ThreadedCommServer::ProcessClient(ClientThreadSharedData &SDFromClient,MOOS::ServerAudit & Auditor)
{
    bool bResult = true;

    try
    {

        //now we act on that packet
        //by way of the user supplied called back
        if(m_pfnRxCallBack!=NULL)
        {

            std::string sWho = SDFromClient._sClientName;

            //first find the client object
			std::map<std::string,ClientThread*>::iterator q = m_ClientThreads.find(sWho);
			if(q == m_ClientThreads.end())
			{
			   return MOOSFail("logical error - FIX ME!");
			}

			ClientThread* pClient = q->second;

            if(m_bQuiet)
                InhibitMOOSTraceInThisThread(false);

            double dfTNow = MOOS::Time();

            MOOSMSG_LIST MsgLstRx,MsgLstTx;

            //convert to list of messages
            SDFromClient._pPkt->Serialize(MsgLstRx,false);

            Auditor.AddStatistic(sWho,SDFromClient._pPkt->GetStreamLength(),MsgLstRx.size(),dfTNow,true);

			if(MsgLstRx.empty())
			{
				std::cerr<<"very strange there is no content in the Pkt\n";
				return false;
			}

            //is there any sort of notification going on here?
            bool bIsNotification = false;
            double dfLargeDelay = m_dfCommsLatencyConcern*GetMOOSTimeWarp();
            for(MOOSMSG_LIST::iterator q = MsgLstRx.begin();q!=MsgLstRx.end();q++)
            {
            	if(q->IsType(MOOS_NOTIFY))
            	{
            		if(dfTNow-q->GetTime()>dfLargeDelay)
            		{
            			std::cout<<"WARNING : Message "<<q->GetKey()<<" from "<<q->GetSource()<<" is "<<(dfTNow-q->GetTime())*1000<<" ms delayed\n";
            		}
            		bIsNotification= true;
            		break;
            	}
            }


            //is this a timing message from V10 client?
            bool bTimingPresent = false;
            CMOOSMsg TimingMsg;
            if(MsgLstRx.front().IsType(MOOS_TIMING))
            {
            	bTimingPresent = true;
            	TimingMsg =MsgLstRx.front();

            	MsgLstRx.pop_front();


            	TimingMsg.SetDouble( MOOSLocalTime());

                Auditor.AddTimingStatistic(sWho,
                                           TimingMsg.GetTime(),
                                           TimingMsg.GetDouble());

            	//and here we control the speed of this clienttxt
            	TimingMsg.SetDoubleAux(pClient->GetConsolidationTime());
            }

            //let owner figure out what to do !
			//this is a user supplied call back
			if(!(*m_pfnRxCallBack)(sWho,MsgLstRx,MsgLstTx,m_pRxCallBackParam))
			{
				//client call back failed!!
				MOOSTrace(" CMOOSCommServer::ProcessClient()  pfnCallback failed\n");
			}
			else
			{
				//std::cerr<<"picked up "<<MsgLstTx.size()<<" messages for "<<sWho<<"\n";
			}


            if(pClient->IsSynchronous())
            {
				//every packet will no begin with a NULL message the double val

				//add a default packet so client doesn't block
				CMOOSMsg NullMsg;
				//and this is the timing payload
				NullMsg.m_dfVal = MOOSLocalTime();
				MsgLstTx.push_front(NullMsg);

            }
            else if(bTimingPresent)
            {
            	MsgLstTx.push_front(TimingMsg);
            }

            //send packet back to client...
            ClientThreadSharedData SDDownStream(sWho,ClientThreadSharedData::PKT_WRITE);

            if(!MsgLstTx.empty())
            {
            	unsigned int nMessages = MsgLstTx.size();
				//stuff reply message into a packet
				SDDownStream._pPkt->Serialize(MsgLstTx,true);

				Auditor.AddStatistic(sWho,
									SDDownStream._pPkt->GetStreamLength(),
									nMessages,
									MOOS::Time(),
									false);

				//add it to the work load
				pClient->SendToClient(SDDownStream);
            }

            //was there ever a notification? If not just continue
            if(bIsNotification==false)
            	return true;

            //and here if we have any new fancy asynchronous clients
            //w can send them mail as well...
            for(q=m_ClientThreads.begin();q!=m_ClientThreads.end();q++)
            {
            	ClientThread* pClient = q->second;
            	if(m_pfnFetchAllMailCallBack!=NULL && pClient->IsAsynchronous())
            	{
            		//OK this client can handle unsolicited pushes of data
            		MsgLstTx.clear();
            		if((*m_pfnFetchAllMailCallBack)(q->first,MsgLstTx,m_pFetchAllMailCallBackParam))
                    {
                    	//any pending mail?
                    	if(MsgLstTx.size()==0)
                    		continue;

                    	//gPrinter.Print("sending message to "+q->first);

        				//std::cerr<<"*pushing*  "<<MsgLstTx.size()<<" messages for "<<sWho<<"\n";


                    	ClientThreadSharedData SDAdditionalDownStream(sWho,
                    			ClientThreadSharedData::PKT_WRITE);

                    	//stuff all notifications into a packet
                    	unsigned int nMessages = MsgLstTx.size();
                    	SDAdditionalDownStream._pPkt->Serialize(MsgLstTx,true);


                        Auditor.AddStatistic(q->first,
                        		SDAdditionalDownStream._pPkt->GetStreamLength(),
                        		nMessages,
                        		MOOS::Time(),
                        		false);

                        //add it to the work load of this client
                        pClient->SendToClient(SDAdditionalDownStream);

                    }
            	}
            }


        }
    }
    catch(CMOOSException e)
    {
        MOOSTrace("ProcessClient() Exception: %s\n", e.m_sReason);
        bResult = false;
    }

    return bResult;

}

bool ThreadedCommServer::ProcessClient()
{
	return BASE::ProcessClient();
}


bool ThreadedCommServer::OnClientDisconnect(ClientThreadSharedData &SD)
{


    //lock the base socket list
    m_SocketListLock.Lock();


    //we need to get the socket this thread is working on
    std::map<std::string,ClientThread*>::iterator q = m_ClientThreads.find(SD._sClientName);

    //we need to point the base class focus socket at this
    m_pFocusSocket = &(q->second->GetSocket());

    //now we can stop and clean up the thread
    StopAndCleanUpClientThread(SD._sClientName);


    SOCKETLIST::iterator p = std::find(m_ClientSocketList.begin(),m_ClientSocketList.end(),m_pFocusSocket);


    //now call the base class operation - this cleans down the socket from the
    //base class select
    BASE::OnClientDisconnect();

    if(p!=m_ClientSocketList.end())
        m_ClientSocketList.erase(p);

    m_SocketListLock.UnLock();

    return true;
}

bool ThreadedCommServer::OnClientDisconnect()
{

	return BASE::OnClientDisconnect();
}



bool ThreadedCommServer::StopAndCleanUpClientThread(std::string sName)
{



	//use this name to get the thread which is doing our work
    std::map<std::string,ClientThread*>::iterator q = m_ClientThreads.find(sName);


    if(q==m_ClientThreads.end())
        return MOOSFail("runtime error ThreadedCommServer::StopAndCleanUpClientThread - cannot figure out worker thread");

    //stop the thread and wait for it to return
    ClientThread* pWorker = q->second;
    if(!pWorker->Kill())
    {
    	std::cerr<<"failed to kill a client - serious problem\n";
    	throw std::runtime_error("failed to kill worker");
    }

    //remove any reference to this worker thread
    m_ClientThreads.erase(q);
    delete pWorker;
    return true;
}

bool ThreadedCommServer::SupportsAsynchronousClients()
{
	return true;
}

bool ThreadedCommServer::TimerLoop()
{
    //we don't run absent client checks in the threaded version
    //simply quit this thread..
    return true;
}



ThreadedCommServer::ClientThread::~ClientThread()
{
	Kill();
}


ThreadedCommServer::ClientThread::ClientThread(const std::string & sName, XPCTcpSocket & ClientSocket,SHARED_PKT_LIST & SharedDataIncoming, bool bAsync, double dfConsolidationPeriodMS,double dfClientTimeout, bool bBoost ):
            _sClientName(sName),
            _ClientSocket(ClientSocket),
            _SharedDataIncoming(SharedDataIncoming),
			_bAsynchronous(bAsync),
			_dfConsolidationPeriod(dfConsolidationPeriodMS/1000.0),
			_dfClientTimeout(dfClientTimeout),
			_bBoostThread(bBoost)
{
    _Worker.Initialise(RunEntry,this);
    _Worker.Name("ThreadedCommServer::ClientThread::Worker::"+sName);

    if(IsAsynchronous())
    {
    	_Writer.Initialise(WriteEntry,this);
    	_Writer.Name("ThreadedCommServer::ClientThread::Writer::"+sName);

    }
}



bool ThreadedCommServer::ClientThread::Run()
{


    //ignore broken pipes as is standard for network apps
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif


    struct timeval timeout;        // The timeout value for the select system call
    fd_set fdset;                // Set of "watched" file descriptors

    double dfLastGoodComms = MOOSLocalTime();

    //this is an io-bound important thread...
    if(_bBoostThread)
    {
    	MOOS::BoostThisThread();
    }

    while(!_Worker.IsQuitRequested())
    {

        // The socket file descriptor set is cleared and the socket file
        // descriptor contained within tcpSocket is added to the file
        // descriptor set.
        FD_ZERO(&fdset);
        FD_SET(_ClientSocket.iGetSocketFd(), &fdset);

        // The select system call is set to timeout after 1 seconds with no data existing
        // on the socket. This reinitialisation has to be here, within the loop as Linux actually writes over
        // the timeout structure on completion of select (now that was a hard bug to find)
        timeout.tv_sec    = 1;
        timeout.tv_usec = 0;



        // A select is setup to return when data is available on the socket
        // for reading.  If data is not available after 1000 useconds, select
        // returns with a value of 0.  If data is available on the socket,
        // the select returns and data can be retrieved off the socket.
        int iSelectRet = select(_ClientSocket.iGetSocketFd() + 1,
            &fdset,
            NULL,
            NULL,
            &timeout);

        // If select returns a -1, then it failed and the thread exits.
        switch(iSelectRet)
        {
        case -1:
            if(XPCSocket::iGetLastError()==INVALID_SOCKET_SELECT)
            {
                return false;
            }
            else
            {
                return false;
            }

        case 0:
            //timeout...nothing to read - spin
        	if(MOOSLocalTime()-dfLastGoodComms>_dfClientTimeout)
        	{
        		std::cout<<MOOS::ConsoleColours::Red();
        		std::cout<<"Disconnecting \""<<_sClientName<<"\" after "<<_dfClientTimeout<<" seconds of silence\n";
        		std::cout<<MOOS::ConsoleColours::reset();
        		OnClientDisconnect();
        		return true;
        	}
            break;

        default:
            //something to read (somewhere)
            if (FD_ISSET(_ClientSocket.iGetSocketFd(), &fdset) != 0)
            {
                if(!HandleClientWrite())
                {
                    //client disconnected!
                    OnClientDisconnect();
                    //MOOSTrace("socket thread for %s quits after disconnect",_sClientName.c_str());
                    return true;
                }

                //something good happened so record our success
        		dfLastGoodComms = MOOSLocalTime();
            }
            else
            {
                //this is strange and unexpected.....
                MOOSTrace("unexpected logical condition");
            }
            break;

        }

        //zero socket set..
        FD_ZERO(&fdset);

    }
    //MOOSTrace("socket thread for %s quits",_sClientName.c_str());
    return 0;
}

bool ThreadedCommServer::ClientThread::OnClientDisconnect()
{

    //prepare to send it up the chain
    CMOOSCommPkt PktRx,PktTx;
    ClientThreadSharedData SD(_sClientName,ClientThreadSharedData::CONNECTION_CLOSED);

    //push this data back to the central thread
    _SharedDataIncoming.Push(SD);

    if(IsAsynchronous())
    {
	    _SharedDataOutgoing.Push(SD);
    }

    return true;
}


bool ThreadedCommServer::ClientThread::Kill()
{

    ClientThreadSharedData QuitInstruction("",ClientThreadSharedData::STOP_THREAD);

	if(IsAsynchronous())
	{
	    //wait for it to stop..
	    _SharedDataOutgoing.Push(QuitInstruction);

		if(!_Writer.Stop())
			return false;
	}

    if(!_Worker.Stop())
    	return false;

    return true;
}

bool ThreadedCommServer::ClientThread::Start()
{
    if(! _Worker.Start())
    	return false;

    if(IsAsynchronous())
    	if(! _Writer.Start())
    		return false;

    return true;
}

double ThreadedCommServer::ClientThread::GetConsolidationTime()
{
	return _dfConsolidationPeriod;
}

bool ThreadedCommServer::ClientThread::SendToClient(ClientThreadSharedData & OutGoing)
{
    _SharedDataOutgoing.Push(OutGoing);
    return true;
}

bool ThreadedCommServer::ClientThread::AsynchronousWriteLoop()
{

    bool bResult = true;

    try
    {
        if(_bBoostThread)
        {
        	MOOS::BoostThisThread();
        }


		while(!_Writer.IsQuitRequested())
		{
			ClientThreadSharedData SDDownChain;

			if(_SharedDataOutgoing.Size()==0)
			{
				_SharedDataOutgoing.WaitForPush();
			}

			_SharedDataOutgoing.Pull(SDDownChain);

			switch(SDDownChain._Status)
			{
				//we are being asked to quit
				case ClientThreadSharedData::CONNECTION_CLOSED:
				{
					return true;
				}

				case ClientThreadSharedData::STOP_THREAD:
				{
				    return true;
				}

				//do normal writing
				case ClientThreadSharedData::PKT_WRITE:
				{

					if(SDDownChain._pPkt.isNull())
					{
						std::cerr<<"logical error"<< MOOSHERE;
						return false;
					}
					//send packet to client
					SendPkt(&_ClientSocket,*SDDownChain._pPkt);
					break;
				}
				default:
				{
					//we have no expectation of this signal
					MOOSTrace("logical error %s", MOOSHERE);
					return false;
				}
			}
		}
    }
    catch (const CMOOSException & e)
	{
	   MOOSTrace("CMOOSCommServer::ClientThread::AsynchronousWriteLoop() Exception: %s\n", e.m_sReason);
	   bResult = false;
	}

	//std::cout<<"Async writer "<<_sClientName<<" quits after thread quit requested\n";

    return bResult;
}

bool ThreadedCommServer::ClientThread::HandleClientWrite()
{
    bool bResult = true;

    try
    {

        //prepare to send it up the chain
        ClientThreadSharedData SDUpChain(_sClientName);
        SDUpChain._Status = ClientThreadSharedData::PKT_READ;

        //read input

        if(!ReadPkt(&_ClientSocket,*SDUpChain._pPkt))
        {
        	throw std::runtime_error("failed packet read and no exception handled");
        }


		_ClientSocket.SetReadTime(MOOS::Time());


        //push this data back to the central thread
        _SharedDataIncoming.Push(SDUpChain);

        if(IsSynchronous())
        {
			//wait for data to be returned...
            ClientThreadSharedData SDDownChain(_sClientName);

            if(_SharedDataOutgoing.Size()==0)
            	_SharedDataOutgoing.WaitForPush();

			_SharedDataOutgoing.Pull(SDDownChain);

			if(SDDownChain._Status!=ClientThreadSharedData::PKT_WRITE)
			{
				MOOSTrace("logical error %s", MOOSHERE);
				return false;
			}

			//send packet to client
			SendPkt(&_ClientSocket,*SDDownChain._pPkt);

			if(_SharedDataOutgoing.Size()!=0)
			{
				std::cerr<<"logical error ThreadedCommServer::ClientThread::HandleClientWrite\n";
			}
        }
        else
        {
        }

    }
    catch (const CMOOSException & e)
    {
		MOOS::DeliberatelyNotUsed(e);
        bResult = false;
    }

    return bResult;


}




}
