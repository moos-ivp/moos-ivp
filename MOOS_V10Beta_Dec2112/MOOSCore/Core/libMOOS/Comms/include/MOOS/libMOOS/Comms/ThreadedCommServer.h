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
         * @param SharedData a sae list of ClientThreadSharedData object
         * @return
         */
        ClientThread(const std::string & sName, XPCTcpSocket & ClientSocket,SHARED_PKT_LIST & SharedDataIncoming, bool bAsync,double dfConsolidationPeriodMS );


        /**
         * standard trick to get around c++ issue an threads
         * @param pParam (a cunningly disguised instance pointer
         * @return
         */
        static bool RunEntry(void * pParam) {  return  ( (ClientThread*)pParam) -> Run();}
        static bool WriteEntry(void * pParam) {  return  ( (ClientThread*)pParam) -> AsynchronousWriteLoop();}





        /**
         * here is the main business of the day - this does the reading and writing in turn
         * @return should not return unless socket closes..
         */
        bool Run();


        bool AsynchronousWriteLoop();

        /**
         * used to push data to client to send...
         * @param OutGoing an object which was orginally collected from _SharedDataIncoming
         * @return tru on success
         */
        bool SendToClient(ClientThreadSharedData & OutGoing);


        bool HandleClientWrite();

        bool OnClientDisconnect();

        XPCTcpSocket & GetSocket(){return _ClientSocket;};

        bool IsSynchronous(){return !_bAsynchronous;};
        bool IsAsynchronous(){return _bAsynchronous;};

        double GetConsolidationTime();

        bool Kill();

        bool Start();

    protected:


        //a thread object which will start the Run() method
        CMOOSThread _Worker;
        CMOOSThread _Writer;




        //what is the name of the client we are representing?
        std::string _sClientName;

        //we are simply given a reference to the socket via which the client is talking at constr
        XPCTcpSocket & _ClientSocket;

        //note that this a reference to a list given to us (we don't own it) at construction
        ThreadedCommServer::SHARED_PKT_LIST &  _SharedDataIncoming;

        //note that this one we own - its private to us
        ThreadedCommServer::SHARED_PKT_LIST _SharedDataOutgoing;

        //is the client asynchronous?
        bool _bAsynchronous;

        //what consolidation period are we asking clients to invoke?
        double _dfConsolidationPeriod;


    };


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



    protected:

		//all connected clients will push the received Pkts into this list....
		SafeList<ClientThreadSharedData> m_SharedDataListFromClient;

		std::map<std::string,ClientThread*> m_ClientThreads;




};

}

#endif /* THREADEDCOMMSERVER_H_ */
