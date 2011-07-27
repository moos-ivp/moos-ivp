#ifndef MOOSHTTPSERVERH
#define MOOSHTTPSERVERH


#include <MOOSUtilityLib/MOOSThread.h>
#include <map>
class XPCTcpSocket;
class CHTTPConnection;
class CMOOSCommClient;



class CMOOSDBHTTPServer
{
public:

    /**simple constructor - just give it the port of the DB to contact
    it assumes localhost.  Sets Web server port to default (9080) */
    CMOOSDBHTTPServer(long lDBPort);

    /**this constructor allows the web server port to specified in
    addition to the DB Port */
    CMOOSDBHTTPServer(long lDBPort, long lWebServerPort);
    
    virtual ~CMOOSDBHTTPServer(void);

    /**method to allow Listen thread to be launched with a MOOSThread.*/
    static bool _CB(void* pParam)
    {
        CMOOSDBHTTPServer* pMe = (CMOOSDBHTTPServer*) pParam;
        return pMe->Listen();
    }

private:

    /**start everything up*/
    void Initialise(long lDBPort, long lWebServerPort);

    /**print warm fuzzies*/
    void DoBanner();

    /**do the listening*/
    bool Listen();   

    /**web server port*/
    long m_lWebServerPort;

    /**thread to execute Listen()*/
    CMOOSThread m_ListenThread;

    /**socket on which to listen*/
    XPCTcpSocket* m_pListenSocket;

    /**collection of open connections*/
    std::list<CHTTPConnection * > m_Connections;

    /**standard interface to the DB*/
    CMOOSCommClient* m_pMOOSComms;

    /**pointer to a single  lock object shared by all connection threads*/
    CMOOSLock *m_pMOOSCommsLock;

};
#endif
