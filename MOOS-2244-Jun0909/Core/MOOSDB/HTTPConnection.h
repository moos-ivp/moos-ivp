/** class to handle an HTTP connection for the MOOSDBHTTPServer class */
/** Written by pnewman@robots.ox.ac.uk                                */


#ifndef CHTTPCONNECTIONH

#include <MOOSUtilityLib/MOOSThread.h>
#include <map>

//forward decalarations
class XPCTcpSocket;
class CMOOSCommClient;
class CMOOSLock;



class CHTTPConnection
{
public:

    /**************************************************************/
    /** structure encapsulating request*/
    struct HTTPRequest
    {
        //type of request
        enum eRequest
        {
            GET,
            POST
        };
        eRequest m_eRequest;

        /**collection of header strings to list of properties*/
        typedef std::map<std::string,std::list<std::string> > HEADERMAP;

        /** add and process a header line */
        void AddHeader(std::string sLine)
        {
            MOOSRemoveChars(sLine," ");
            std::string sHeader = MOOSChomp(sLine,":");
            std::list<std::string> Values;

            while(!sLine.empty())
            {
                Values.push_back(MOOSChomp(sLine,","));
            }
            MOOSToUpper(sHeader);
            m_Headers[sHeader] = Values;

        }

        /** returns true if connection stays live between requests */
        bool IsKeepAlive()
        {
            if(HasProperty("CONNECTION","keep-alive"))
                return true;
            if(HasProperty("CONNECTION","close"))
                return false;

            //default is true
            return true;
        }

        /** how long is tolerated between requests? */
        bool  GetKeepAliveTime(double & dfTimeOut)
        {
            std::list<std::string> Values;

            if(!GetHeader("Keep-Alive",Values))
                return false;

            if(Values.empty())
                return false;

            dfTimeOut =  atof(Values.begin()->c_str());

            return true;
        }

        /** returns true if connection has a required property*/
        bool HasProperty(std::string sHeader, std::string sProp)
        {
            std::list<std::string> Values;

            if(!GetHeader(sHeader,Values))
                return false;

            std::list<std::string>::iterator q;
            for(q = Values.begin();q!=Values.end();q++)
            {
                if(MOOSStrCmp(*q,sProp))
                    return true;
            }
            return false;

        }

        /** fill in all propertirs of a given header*/
        bool GetHeader(std::string sHeader, std::list<std::string> & Values)
        {
            MOOSToUpper(sHeader);
            if(m_Headers.find(sHeader)==m_Headers.end())
                return false;

            Values = m_Headers[sHeader];

            return true;

        }
        /**clean down collection */
        void Clean()
        {
            m_Headers.clear();
        }

        /**the collection of headers*/
        HEADERMAP m_Headers;
    };
    /************ END OF HTTPRequest definition ******************/



    /** simple constructor*/
    CHTTPConnection(XPCTcpSocket* pNewSocket,CMOOSCommClient* pMOOSComms,CMOOSLock* pMOOSCommsLock);

    /** start the connection processing...*/
    bool Run();

    /** has the connection terminated?*/
    bool HasCompleted();

    /** call back to allow Serve to run in MOOSThread*/
    static bool _CB(void* pParam)
    {
        CHTTPConnection* pMe = (CHTTPConnection*) pParam;
        return pMe->Serve();
    }


protected:
    /** main sequence of response actions */
    bool Serve();
    /** Makes a webpage */
    bool MakeWebPage();
    /** Sends an all OK header detailing length of page to follow */
    bool SendHeader();
    /** Sends a 404 error header */
    bool SendFailureHeader();
	/** Send the webpage itself*/
    bool SendWebPage();
    /** respond to someone putting data */
    bool HandlePoke(std::string sPokeURL);
    /* build webpage of single variable */
    bool BuildSingleVariableWebPageContents( std::ostringstream & wp,MOOSMSG_LIST & MsgList);
    /** build we page of whole DB*/
    bool BuildFullDBWebPageContents( std::ostringstream & wp,MOOSMSG_LIST & MsgList);
	/** read a request header*/
    bool ReadRequest();

    /** read a single line from client */
    bool ReadLine(std::string &);
	/** lower level send string with appended \r\n */
    void SendLine(std::string sLine);
    /** lower level send string (no line feed cr appended) */
    void SendString(std::string sLine);
    
    
    /** string containing the webpage iteself*/
    std::string m_sWebPage;
    /** Thread object which runs the servicing sequence of commands */
    CMOOSThread m_ServeThread; 
    /** Socket which hold the Connection */
    XPCTcpSocket* m_pSocket;
    
    /** name of focus variable (empty for whole site request)*/
    std::string m_sFocusVariable;

    /** A HTTP Request instantiation */
    HTTPRequest m_Request;

    /** pointer to a COmms Client (The DB Connection is shared between all HTTPConnections)*/
    CMOOSCommClient * m_pMOOSComms;

    /** pointer to a lock which provides protected access to the MOOS comms client object*/
    CMOOSLock * m_pMOOSCommsLock;

};
#endif
