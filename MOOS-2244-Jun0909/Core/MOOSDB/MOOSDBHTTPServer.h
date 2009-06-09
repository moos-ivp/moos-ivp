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
    it assumes localhost*/
    CMOOSDBHTTPServer(long lPort);
    virtual ~CMOOSDBHTTPServer(void);

    /**method to allow Listen thread to be launched with a MOOSThread.*/
    static bool _CB(void* pParam)
    {
        CMOOSDBHTTPServer* pMe = (CMOOSDBHTTPServer*) pParam;
        return pMe->Listen();
    }

private:

    /**print warm fuzzies*/
    void DoBanner();

    /**do the listening*/
    bool Listen();   

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
