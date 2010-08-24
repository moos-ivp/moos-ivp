#include "MOOSGenLib/MOOSGenLibGlobalHelper.h"
#include "MOOSLIB/XPCTcpSocket.h"
#include "MOOSLIB/MOOSCommClient.h"
#include "MOOSDBHTTPServer.h"
#include "HTTPConnection.h"

#ifndef _WIN32
    #include "signal.h"
#endif

#define WEBSERVER_PORT 9080L
#define WEBSERVER_COMMCLIENT_TICK 5

bool _DUMMYMOOSCB(void *){return true;}

CMOOSDBHTTPServer::CMOOSDBHTTPServer(long lPort)
{
    //make a new comms client to reach the DB   
    m_pMOOSComms = new CMOOSCommClient;
    m_pMOOSComms->SetOnConnectCallBack(_DUMMYMOOSCB,NULL);
    m_pMOOSComms->SetQuiet(true);
    m_pMOOSComms->Run("localhost",lPort,"DBWebServer",WEBSERVER_COMMCLIENT_TICK);

    //we'll be accessing the same DB interface from lots of threads (one for each connected browser
    //so we had better be safe
    m_pMOOSCommsLock = new CMOOSLock;

    //start a listen thread (which will spawn service threads)
    m_ListenThread.Initialise(_CB,this);
    m_ListenThread.Start();
    
    
    //ignore broken pipes as is standard for network apps
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif
    

}

CMOOSDBHTTPServer::~CMOOSDBHTTPServer(void)
{
    //clean up
    m_pMOOSComms->Close();
    delete m_pMOOSComms;
    delete m_pMOOSCommsLock;
}

void CMOOSDBHTTPServer::DoBanner()
{
    MOOSTrace("\n*****************************************************\n");
    MOOSTrace("  serving webpages HTTP on http://%s:%d\n" ,m_pMOOSComms->GetLocalIPAddress().c_str(),int(WEBSERVER_PORT));
    MOOSTrace("*****************************************************\n");
}

bool CMOOSDBHTTPServer::Listen()
{
    //ignore broken pipes as is standard for network apps
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif
    
    
    while(!m_pMOOSComms->IsConnected())
        MOOSPause(100);
    
    DoBanner();
    
    m_pListenSocket = new XPCTcpSocket(WEBSERVER_PORT);
    
    try
    {
        m_pListenSocket->vSetReuseAddr(1);
        m_pListenSocket->vBindSocket();
    }
    catch(XPCException e)
    {        
        UNUSED_PARAMETER(e);
        MOOSTrace("Error binding to HTTP listen socket - Is there another HTTPDBServer Running?\n");
        MOOSTrace("This HTTP Server Is Quitting\n");                
        delete m_pListenSocket;        
        m_pListenSocket = NULL;       
        return false;
    }
    
    //OK looks good...
    while(1)
    {
        
        try
        {
            char sClientName[200];
            
            //wait here for folk demanding attention
            m_pListenSocket->vListen();
            
            //let them in the door
            XPCTcpSocket * pNewSocket = m_pListenSocket->Accept(sClientName);

            //make a new handler object
            CHTTPConnection * pNewConnection = new CHTTPConnection(pNewSocket,m_pMOOSComms,m_pMOOSCommsLock);

            //let it run and service the client as it sees fit
            //this is a non blocking call allowing the server to 
            //dal with other requests
            pNewConnection->Run();
            
            //store this connection
            m_Connections.push_back(pNewConnection);  

            //look to clean up here..
            std::list<CHTTPConnection*>::iterator q = m_Connections.begin();
            while( q!=m_Connections.end())
            {
                //carefully remove expired handler objects
                if((*q)->HasCompleted())
                {
                    CHTTPConnection* t = *q;
                    q = m_Connections.erase(q);
                    delete t;
                }
                else
                {
                    q++;
                }
            }


        }
        catch(XPCException e)
        {
            MOOSTrace("Exception Thrown in listen loop: %s\n",e.sGetException());
        }
        
    }
    
    delete m_pListenSocket;
    return true;
}
