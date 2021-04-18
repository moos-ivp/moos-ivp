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
/*
 * ThreadedCommServer.h
 *
 *  Created on: Aug 29, 2011
 *      Author: pnewman
 */

#ifndef THREADEDCOMMSERVER_H_
#define THREADEDCOMMSERVER_H_


#include "MOOS/libMOOS/Comms/MOOSCommServer.h"
#include "MOOS/libMOOS/Utils/SafeList.h"
#include "MOOS/libMOOS/Thirdparty/PocoBits/SharedPtr.h"

namespace MOOS
{

//this is a small container structure used to hold data which is shared
//between classes the in concert support the threadedcommserver
struct ClientThreadSharedData
{

	std::string _sClientName;

	//payload
	Poco::SharedPtr<CMOOSCommPkt> _pPkt;

	//little bit of status
	enum Status
	{
	  NOT_INITIALISED,
	  PKT_READ,
	  CONNECTION_CLOSED,
	  PKT_WRITE,
	  STOP_THREAD,
	} _Status;


	ClientThreadSharedData(const std::string & sN,Status eStatus =NOT_INITIALISED):
	_sClientName(sN),_Status(eStatus)
	{
		_pPkt = new CMOOSCommPkt;
	};

	ClientThreadSharedData(){_Status =NOT_INITIALISED; };

};


class ServerAudit;

class ThreadedCommServer : public CMOOSCommServer
{
public:
    ThreadedCommServer();
    virtual ~ThreadedCommServer();

private:
    typedef CMOOSCommServer BASE;


protected:
    typedef SafeList<ClientThreadSharedData> SHARED_PKT_LIST;


    /**
     * This is a class which handles the Reading and Writing of fully formed MOOSCommPkts
     * It ceaselessly looks to read from a socket - when a Pkt is formed it stuff it on a shared thread safe
     * list. It then waits for a reply pkt to be placed in _SharedDataOutgoing via a call to
     * ::SendToClient()
     */
    class ClientThread : public CMOOSCommObject
    {
    public:
        virtual ~ClientThread();
        /**
         *
         * @param sName name of client this object is encapsulating
         * @param ClientSocket reference to a socket at the other end of which is the client
         * @param SharedData a safe list of ClientThreadSharedData objects
         * @param bAsync is this an asynchronous client
         * @param dfConsolidationPeriodMS how long should this client be told to pause between talking to DB
         * @param dfClientTimeout how long will we tolerate silence before dumping this sorry soul
         * @param are we wishing to boost the priority of this IO bound thread
         * @return
         */
        ClientThread(const std::string & sName,
        		XPCTcpSocket & ClientSocket,
        		SHARED_PKT_LIST & SharedDataIncoming,
        		bool bAsync,
        		double dfConsolidationPeriodMS,
        		double dfClientTimeout,
        		bool bBoostThread);


        /**
         * standard trick to get around c++ issue an threads
         * @param pParam (a cunningly disguised instance pointer
         * @return
         */
        static bool ReadEntry(void * pParam) {  return  ( (ClientThread*)pParam) -> AsynchronousReadLoop();}
        static bool WriteEntry(void * pParam) {  return  ( (ClientThread*)pParam) -> AsynchronousWriteLoop();}

        /**
         * here is the main business of the day - this does the reading and writing in turn
         * @return should not return unless socket closes..
         */
        bool AsynchronousReadLoop();

        bool AsynchronousWriteLoop();

        /**
         * used to push data to client to send...
         * @param OutGoing an object which was orginally collected from _SharedDataIncoming
         * @return tru on success
         */
        bool SendToClient(ClientThreadSharedData & OutGoing);

        bool SelectWrite(ClientThreadSharedData & SDOutGoing);


        bool HandleClientWrite();

        bool OnClientDisconnect();

        XPCTcpSocket & GetSocket(){return m_ClientSocket;};

        bool IsSynchronous(){return !m_bAsynchronous;};
        bool IsAsynchronous(){return m_bAsynchronous;};

        double GetConsolidationTime();

        const std::string & GetClientName(){ return m_sClientName;};

        bool Start();

    protected:


        //a thread object which will start the Run() method
        CMOOSThread m_Reader;
        CMOOSThread m_Writer;

        //terminates this client and constituent threads
        bool Kill();

        //what is the name of the client we are representing?
        std::string m_sClientName;

        //we are simply given a reference to the socket via which the client is talking at constr
        XPCTcpSocket & m_ClientSocket;

        //note that this a reference to a list given to us (we don't own it) at construction
        ThreadedCommServer::SHARED_PKT_LIST &  m_SharedDataIncoming;

        //note that this one we own - its private to us
        ThreadedCommServer::SHARED_PKT_LIST m_SharedDataOutgoing;

        //is the client asynchronous?
        bool m_bAsynchronous;

        //what consolidation period are we asking clients to invoke?
        double m_dfConsolidationPeriod;

        //how long can we tolerate silence
        double m_dfClientTimeout;

        //are we asked to boost prioirty
        bool m_bBoostThread;

        std::vector<unsigned char  > m_IncomingStorage;
        std::vector<unsigned char  > m_OutgoingStorage;
    };

    typedef Poco::SharedPtr<ClientThread> SharedClientThread;



    /** Called when a new client connects. Performs handshaking and adds new socket to m_ClientSocketList
    @param pNewClient pointer to the new socket created in ListenLoop;
    @see ListenLoop*/
    virtual bool OnNewClient(XPCTcpSocket * pNewClient,char * sName);

    virtual bool OnClientDisconnect(ClientThreadSharedData &SD);
    virtual bool OnClientDisconnect();

    /** return true if Aynschronous Clients are supported */
    virtual bool SupportsAsynchronousClients();

    virtual bool ServerLoop();

    virtual bool TimerLoop();

    virtual bool AddAndStartClientThread(XPCTcpSocket & NewClientSocket,const std::string & sName);

    virtual bool ProcessClient(ClientThreadSharedData &SD, MOOS::ServerAudit & Auditor);

    virtual bool ProcessClient();

    bool StopAndCleanUpClientThread(std::string sName);

    virtual bool Stop();

    protected:

		//all connected clients will push the received Pkts into this list....
		SafeList<ClientThreadSharedData> m_SharedDataListFromClient;
        typedef std::map<std::string,SharedClientThread> ClientThreadsMap;
        ClientThreadsMap m_ClientThreads;

        typedef SafeList<SharedClientThread> SafeClientThreadsList;
        SafeClientThreadsList m_OldClientThreadsToDestroy;

        //do not decelare m_WasteDisposal before m_OldClientThreadsToDestroy
        //as we have to have the thread being destroyed before the resource it
        //is working on.
        CMOOSThread m_WasteDisposal;
        static bool WasteDisposalEntry(void * pParam) {
            return   static_cast<ThreadedCommServer*>(pParam) -> WasteDisposalLoop();}
        virtual bool WasteDisposalLoop();

};

}

#endif /* THREADEDCOMMSERVER_H_ */
