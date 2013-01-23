///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Core Component. 
//        
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; either version 2 of the 
//   License, or (at your option) any later version. 
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//   General Public License for more details. 
//            
//   You should have received a copy of the GNU General Public License 
//   along with this program; if not, write to the Free Software 
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
//   02111-1307, USA. 
//
//////////////////////////    END_GPL    //////////////////////////////////
// MOOSDB.h: interface for the CMOOSDB class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(MOOSDBH)
#define MOOSDBH

#include "MOOS/libMOOS/Thirdparty/PocoBits/Platform_WIN32.h"
#include "MOOS/libMOOS/Thirdparty/PocoBits/UnWindows.h"



#include "MOOSDBVar.h"
#include <string>
#include <map>
#include <memory>
#include "MOOSDBHTTPServer.h"
#include "MsgFilter.h"

#include "MOOS/libMOOS/Comms/ThreadedCommServer.h"

using namespace std;

typedef map<string,MOOSMSG_LIST> MOOSMSG_LIST_STRING_MAP;
typedef map<string,CMOOSDBVar> DBVAR_MAP;


#define DEFAULT_MOOS_SERVER_PORT 9000

/** The CMOOSDB class is the core of the MOOS comms protocol. It is only of interest
to the developer modifying the MOOSDB application server*/ 
class CMOOSDB  
{
public:
    bool OnDisconnect(string & sClient);

    /** callback function passed to CMOOSCommServer member object. This STATIC function allows
    entry back into this object by invoking OnRxPkt()*/
    static  bool OnRxPktCallBack(const std::string & sClient, MOOSMSG_LIST & MsgLstRx,MOOSMSG_LIST & MsgLstTx, void * pParam);

    /** called internally when a client disconnects */
    static bool OnDisconnectCallBack(string & sClient,void * pParam);

    static bool OnFetchAllMailCallBack(const std::string & sWho,MOOSMSG_LIST & MsgListTx, void * pParam);

    /** called internally when a MOOSPkt (a collection of MOOSMsg's ) is
    received by the server */
    bool OnRxPkt(const std::string & sClient,MOOSMSG_LIST & MsgLstRx,MOOSMSG_LIST & MsgLstTx);

    bool OnFetchAllMail(const std::string & sWho,MOOSMSG_LIST & MsgListTx);


    /** called by the owning application to start the DB running. It launches threads
    and returns */
    bool Run(int argc = 0, char * argv[] =0);

    /** returns the port on which this DB is listening */
    long GetDBPort(){return m_nPort;};

    /**
     * return the mission file that is being read
     */
    std::string GetMissionFile(){return m_MissionReader.GetFileName();};


    CMOOSDB();
    virtual ~CMOOSDB();

protected:

    bool OnClearRequested(CMOOSMsg & Msg, MOOSMSG_LIST & MsgTxList);
    void Var2Msg(CMOOSDBVar & Var, CMOOSMsg &Msg);
    bool AddMessageToClientBox(const string &sClient,CMOOSMsg & Msg);
    bool VariableExists(const string & sVar);
    bool DoVarLookup(CMOOSMsg & Msg, MOOSMSG_LIST &MsgTxList);

    /** Next three functions are unusual and their genus should not proliferate.
    Very occasionally a client with a singularly unusual role may want to ask questions
    directly to the DB. The paired function is in MOOCCommClient::ServerRequest which
    unusually, is blocking (with timeout) - hence my edgey feel about these utilities*/
    bool OnServerAllRequested(CMOOSMsg & Msg, MOOSMSG_LIST & MsgTxList);
    bool OnProcessSummaryRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList);
    bool OnVarSummaryRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList);

    void UpdateDBTimeVars();
    void UpdateDBClientsVar();
    bool DoServerRequest(CMOOSMsg & Msg, MOOSMSG_LIST & MsgTxList);
    CMOOSDBVar & GetOrMakeVar(CMOOSMsg & Msg);
    bool OnRegister(CMOOSMsg & Msg);
    bool OnUnRegister(CMOOSMsg &Msg);
    bool OnNotify(CMOOSMsg & Msg);
    bool ProcessMsg(CMOOSMsg & MsgRx,MOOSMSG_LIST & MsgLstTx);
    double GetStartTime(){return m_dfStartTime;}
private:
    string m_sDBName;
    string m_sCommunityName;
    CMOOSFileReader m_MissionReader;
    int m_nPort;
    double m_dfStartTime;


    /**a map of client name to a list of Msgs that will be sent
    the next time a client calls in*/
    MOOSMSG_LIST_STRING_MAP m_HeldMailMap;
    DBVAR_MAP    m_VarMap;



    std::map<std::string,std::set< MOOS::MsgFilter > > m_ClientFilters;

    //pointer to a webserver if one is needed
    std::auto_ptr<CMOOSDBHTTPServer> m_pWebServer;

    //pointer to the comms server (could be a threaded one but base class is CMOOSCommServer
    std::auto_ptr<CMOOSCommServer> m_pCommServer;





private:
    void LogStartTime();
};

#endif 
