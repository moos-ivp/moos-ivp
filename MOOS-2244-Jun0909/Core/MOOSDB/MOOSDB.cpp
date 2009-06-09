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
// MOOSDB.cpp: implementation of the CMOOSDB class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#pragma warning(disable : 4786)
#endif


#include <MOOSLIB/MOOSLib.h>
#include <MOOSGenLib/MOOSGenLib.h>

#include "MOOSDB.h"
#include "assert.h"
#include <iostream>
#include <sstream>
#include <iterator>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



bool CMOOSDB::OnRxPktCallBack(const std::string & sWho,MOOSMSG_LIST & MsgListRx,MOOSMSG_LIST & MsgListTx, void * pParam)
{
    CMOOSDB* pMe = (CMOOSDB*)(pParam);
    
    return pMe->OnRxPkt(sWho,MsgListRx,MsgListTx);
}

bool CMOOSDB::OnDisconnectCallBack(string & sClient, void * pParam)
{
    CMOOSDB* pMe = (CMOOSDB*)(pParam);
    
    return pMe->OnDisconnect(sClient);
}

CMOOSDB::CMOOSDB()
{
    //here we set up default community names and DB Names
    m_sDBName = "MOOSDB#1";
    m_sCommunityName = "#1";
    
    //her is the default port to listen on
    m_nPort = DEFAULT_MOOS_SERVER_PORT;
    
    //make our own variable called DB_TIME
    {
        CMOOSDBVar NewVar("DB_TIME");
        NewVar.m_cDataType = MOOS_DOUBLE;
        NewVar.m_dfVal= HPMOOSTime();
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = HPMOOSTime();
        m_VarMap["DB_TIME"] = NewVar;
    }
    
    //make our own variable called DB_TIME
    {
        CMOOSDBVar NewVar("DB_UPTIME");
        NewVar.m_cDataType = MOOS_DOUBLE;
        NewVar.m_dfVal= 0;
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = HPMOOSTime();
        m_VarMap["DB_UPTIME"] = NewVar;
    }

    //make our own variable called DB_CLIENTS
    {
        CMOOSDBVar NewVar("DB_CLIENTS");
        NewVar.m_cDataType = MOOS_DOUBLE;
        NewVar.m_dfVal= HPMOOSTime();
        NewVar.m_sWhoChangedMe = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        NewVar.m_dfWrittenTime = HPMOOSTime();
        m_VarMap["DB_CLIENTS"] = NewVar;
    }
    
    //ignore broken pipes as is standard for network apps
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif
    
    
    
}

CMOOSDB::~CMOOSDB()
{
    
}

bool CMOOSDB::Run(const std::string  & sMissionFile )
{
    if(!m_MissionReader.SetFile(sMissionFile))
    {
        MOOSTrace("Warning no mission file found - still serving but with trepidation\n");
    }
    
    if(m_MissionReader.GetValue("COMMUNITY",m_sCommunityName))
    {
        m_sDBName = "MOOSDB_"+m_sCommunityName;
    }

    double dfWarp;
    if(m_MissionReader.GetValue("MOOSTimeWarp",dfWarp))
    {
		SetMOOSTimeWarp(dfWarp);
    }
    
    
    string sPort;
    
    if(m_MissionReader.GetValue("SERVERPORT",sPort))
    {
        m_nPort = atoi(sPort.c_str());
        if(m_nPort==0)
        {
            MOOSTrace("Error reading server port defaulting to %d \n",DEFAULT_MOOS_SERVER_PORT);
            m_nPort = DEFAULT_MOOS_SERVER_PORT;
        }
    }
    
    m_CommServer.SetOnRxCallBack(OnRxPktCallBack,this);
    
    m_CommServer.SetOnDisconnectCallBack(OnDisconnectCallBack,this);
    
    LogStartTime();
    
    m_CommServer.Run(m_nPort,m_sCommunityName);
        
    return true;
}


void CMOOSDB::UpdateDBClientsVar()
{
#define CLIENT_LIST_PUBLISH_PERIOD 2
    static double dfLastTime = MOOSTime();
    double dfNow = MOOSTime();
    if(dfNow-dfLastTime>CLIENT_LIST_PUBLISH_PERIOD)
    {
        STRING_LIST Clients;
        m_CommServer.GetClientNames(Clients);

        std::ostringstream ss;
        std::copy(Clients.begin(),Clients.end(),ostream_iterator<std::string>(ss,","));
        
        CMOOSMsg DBC(MOOS_NOTIFY,"DB_CLIENTS",ss.str());
        DBC.m_sOriginatingCommunity = m_sCommunityName;
        DBC.m_sSrc = m_sDBName;
        OnNotify(DBC);
        dfLastTime = dfNow;
    }




}

void CMOOSDB::UpdateDBTimeVars()
{
    static double dfLastTime = MOOSTime();
    double dfNow = MOOSTime();
    if(dfNow-dfLastTime>1.0)
    {
        CMOOSMsg DBT(MOOS_NOTIFY,"DB_TIME",MOOSTime());
        DBT.m_sOriginatingCommunity = m_sCommunityName;
        DBT.m_sSrc = m_sDBName;
        OnNotify(DBT);
        dfLastTime = dfNow;

        CMOOSMsg DBUpT(MOOS_NOTIFY,"DB_UPTIME",MOOSTime()-GetStartTime());
        DBUpT.m_sOriginatingCommunity = m_sCommunityName;
        DBUpT.m_sSrc = m_sDBName;
        OnNotify(DBUpT);    
    }
}

/**this will be called each time a new packet is recieved*/
bool CMOOSDB::OnRxPkt(const std::string & sClient,MOOSMSG_LIST & MsgListRx,MOOSMSG_LIST & MsgListTx)
{
    

    MOOSMSG_LIST::iterator p;
    
    for(p = MsgListRx.begin();p!=MsgListRx.end();p++)
    {
        ProcessMsg(*p,MsgListTx);
    }
    
    //good spot to update our internal time    
    UpdateDBTimeVars();

    //and send clients an occasional membersip list
    UpdateDBClientsVar();

    if(!MsgListRx.empty())
    {
        
        //now we fill in the packet with our replies to THIS CLIENT
        //MOOSMSG_LIST_STRING_MAP::iterator q = m_HeldMailMap.find(MsgListRx.front().m_sSrc);
        
        MOOSMSG_LIST_STRING_MAP::iterator q = m_HeldMailMap.find(sClient);
        
        if(q==m_HeldMailMap.end())
        {
            
            //CMOOSMsg & rMsg = MsgListRx.front();
            //there is no mail waiting to be sent to this client
            //should only happen at start up...
            //string sClient = MsgListRx.front().m_sSrc;
            
            MOOSMSG_LIST NewList;
            
            m_HeldMailMap[sClient] = NewList;
            
            q = m_HeldMailMap.find(sClient);
            
            assert(q!=m_HeldMailMap.end());
        }
        
        
        if(q!=m_HeldMailMap.end())
        {
            if(!q->second.empty())
            {
                //copy all the held mail to MsgListTx
                MsgListTx.splice(MsgListTx.begin(),q->second);
            }
        }
    }
    
    return true;
}

/** This functions decides what needs to be done on a message by message basis */
bool CMOOSDB::ProcessMsg(CMOOSMsg &MsgRx,MOOSMSG_LIST & MsgListTx)
{
    
    
    switch(MsgRx.m_cMsgType)
    {
    case MOOS_NOTIFY:    //NOTIFICATION
        return OnNotify(MsgRx);
        break;
    case MOOS_UNREGISTER:
        return OnUnRegister(MsgRx);
        break;
    case MOOS_REGISTER:    //REGISTRATION
        return OnRegister(MsgRx);
        break;
    case MOOS_NULL_MSG:
        break;    
    case MOOS_COMMAND:  //COMMAND
        break;
    case MOOS_SERVER_REQUEST:
        return DoServerRequest(MsgRx,MsgListTx);
        break;
    }
    
    return true;
}


/** called when the in focus client is telling us something
has changed. Ie this is a notify packet */
bool CMOOSDB::OnNotify(CMOOSMsg &Msg)
{
    double dfTimeNow = HPMOOSTime();
    
    
    CMOOSDBVar & rVar  = GetOrMakeVar(Msg);
    
    
    if(rVar.m_nWrittenTo==0)
    {
        rVar.m_cDataType=Msg.m_cDataType;
    }
    
    if(rVar.m_cDataType==Msg.m_cDataType)
    {
        double dfLastWrittenTime = rVar.m_dfWrittenTime;
        
        rVar.m_dfWrittenTime = dfTimeNow;
        
        rVar.m_dfTime        = Msg.m_dfTime;
        
        rVar.m_sWhoChangedMe = Msg.m_sSrc;
        
        if(Msg.m_sOriginatingCommunity.empty())
        {
            //we are the server in the originating community
            rVar.m_sOriginatingCommunity = m_sCommunityName;
            Msg.m_sOriginatingCommunity = m_sCommunityName;
        }
        else
        {
            //this message came from another community
            rVar.m_sOriginatingCommunity = Msg.m_sOriginatingCommunity;
        }
        
        switch(rVar.m_cDataType)
        {
        case MOOS_DOUBLE:
            rVar.m_dfVal = Msg.m_dfVal;
            break;
        case MOOS_STRING:
            rVar.m_sVal = Msg.m_sVal;
            break;
        }
        
        //record sSrc as a writer of this data
        rVar.m_Writers.insert(Msg.m_sSrc);
        
        //increment the number of times we have written to this variable
        rVar.m_nWrittenTo++;
        
        //how often is it being written?
        double dfDT = (dfTimeNow-dfLastWrittenTime);
        if(dfDT>0)
        {
            //this looks a little hookey - the numbers are arbitrary to give sensible
            //looking frequencies when timing is coarse
            if(dfDT>5.0)
            {
                //MIN
                rVar.m_dfWriteFreq = 0.0;
            }
            else if(dfDT<0.01)
            {
                //MAX OUT
                rVar.m_dfWriteFreq = 100.0;
            }
            else
            {
                //IIR FILTER COOEFFICENT
                double dfAlpha = 0.7;                
                rVar.m_dfWriteFreq = dfAlpha*rVar.m_dfWriteFreq + (1.0-dfAlpha)/(dfDT/++rVar.m_nOverTicks);
                rVar.m_nOverTicks=0;
            }
        }
        else
        {
            rVar.m_nOverTicks++;
        }
        
        //now comes the intersting part...
        //which clients have asked to be informed
        //of changes in this variable?
        REGISTER_INFO_MAP::iterator p;
        
        
        for(p = rVar.m_Subscribers.begin();p!=rVar.m_Subscribers.end();p++)
        {
            
            CMOOSRegisterInfo & rInfo = p->second;
            //has enough time expired since the last time we
            //sent notification for the variable?
            if(rInfo.Expired(dfTimeNow))
            {
                
                string  & sClient = p->second.m_sClientName;
                
                //the Msg we were passed has all the information we require already
                Msg.m_cMsgType = MOOS_NOTIFY;
                
                
                AddMessageToClientBox(sClient,Msg);
                
                //finally we remember when we sent this to the client in question
                rInfo.SetLastTimeSent(dfTimeNow);
            }
        }
    }
    else
    {
        MOOSTrace("Attempting to update var \"%s\" which is type %c with data type %c\n",
            Msg.m_sKey.c_str(),
            rVar.m_cDataType,
            Msg.m_cDataType);
    }
    
    
    
    return true;
}


/** we now want to store some message in anoth cleints message box, when they next call
in they shall be informed of the change by stuffing this msg into a return packet */
bool    CMOOSDB::AddMessageToClientBox(const string &sClient,CMOOSMsg & Msg)
{
    MOOSMSG_LIST_STRING_MAP::iterator q = m_HeldMailMap.find(sClient);
    
    if(q==m_HeldMailMap.end())
    {
        //there is no mail waiting to be sent to this client
        //should only happen at start up...
        MOOSMSG_LIST NewList;
        m_HeldMailMap[sClient] = NewList;
        
        q = m_HeldMailMap.find(sClient);
        
        assert(q!=m_HeldMailMap.end());
    }
    
    //q->second is now a reference to a list of messages that will be
    //sent to sClient the next time it calls into the database...
    
    q->second.push_front(Msg);
    
    return true;
}


/** Called when a msg containing a unregistration (desubscribe) 
request is received */
bool CMOOSDB::OnUnRegister(CMOOSMsg &Msg)
{
    //what are we looking to un register for?
    
    //if the variable already exists then post a notification message
    //to the client
    bool bAlreadyThere = VariableExists(Msg.m_sKey);
    if(bAlreadyThere)
    {
        CMOOSDBVar & rVar  = GetOrMakeVar(Msg);
        rVar.RemoveSubscriber(Msg.m_sSrc);
    }
    
    return true;
}


/** Called when a msg containing a registration (subscription) 
request is received */
bool CMOOSDB::OnRegister(CMOOSMsg &Msg)
{
    
    
    //what are we looking to register for?
    
    //if the variable already exists then post a notification message
    //to the client
    bool bAlreadyThere = VariableExists(Msg.m_sKey);
    
    CMOOSDBVar & rVar  = GetOrMakeVar(Msg);
    
    if(!rVar.AddSubscriber(Msg.m_sSrc,Msg.m_dfVal))
        return false;
    
    
    if(bAlreadyThere && rVar.m_nWrittenTo!=0)
    {
        //when the client registered the variable already existed...
        //better tell them
        CMOOSMsg ReplyMsg;
        Var2Msg(rVar,ReplyMsg);
        
        ReplyMsg.m_cMsgType = MOOS_NOTIFY;
        
        AddMessageToClientBox(Msg.m_sSrc,ReplyMsg);      
        
    }
    
    return true;
}


/** return a reference to a DB variable is it already exists
and if not make one and then return a reference to it.
@param Msg      Msg.m_sKey contains name of variable
*/
CMOOSDBVar & CMOOSDB::GetOrMakeVar(CMOOSMsg &Msg)
{    
    
    //look up this variable name
    DBVAR_MAP::iterator p = m_VarMap.find(Msg.m_sKey);
    
    if(p==m_VarMap.end())
    {
        //we need to make a new variable here for this key
        //as we don't know about it!
        
        CMOOSDBVar NewVar(Msg.m_sKey);
        
        if(Msg.m_cMsgType==MOOS_REGISTER)
        {
            //interesting case is when this method is being used to 
            //register for a variable that has not been written to. Here
            //we simply make the variable but don't commit to its data type
            NewVar.m_cDataType = MOOS_NOT_SET;
        }
        else
        {
            //new variable will have data type of request message
            NewVar.m_cDataType = Msg.m_cDataType;
        }
        
        //index our new creation
        m_VarMap[Msg.m_sKey] = NewVar;
        
        //check we can get it back ok!!
        p = m_VarMap.find(Msg.m_sKey);
        
        assert(p!=m_VarMap.end());
        
#ifdef DB_VERBOSE
        
        MOOSTrace("\nMaking new variable: \n");
        MOOSTrace("    Name=\"%s\"\n",NewVar.m_sName.c_str());
        MOOSTrace("    Src =\"%s\"\n",Msg.m_sSrc.c_str());
        MOOSTrace("    Type =\"%s\"\n",NewVar.m_cDataType);
#endif
    }
    
    //ok we know what you are talking about
    CMOOSDBVar & rVar = p->second;
    
    //return the reference
    return rVar;
}

bool CMOOSDB::OnDisconnect(string &sClient)
{
    //for all variables remove subscriptions to sClient
    
    DBVAR_MAP::iterator p;
    
    for(p=m_VarMap.begin();p!=m_VarMap.end();p++)
    {
        CMOOSDBVar  & rVar = p->second;
        
        rVar.RemoveSubscriber(sClient);
    }
    
    MOOSTrace("removing held mail for \"%s\"...\n",sClient.c_str());
    
    m_HeldMailMap.erase(sClient);
    
    return true;
}

bool CMOOSDB::DoServerRequest(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
    //explictly requesting the server to do something...
    
    if(Msg.m_sKey=="ALL")
    {
        return OnServerAllRequested(Msg,MsgTxList);
    }
    else if(Msg.m_sKey.find("PROC_SUMMARY")!=string::npos)
    {
        return OnProcessSummaryRequested(Msg,MsgTxList);
    }
    else if (Msg.m_sKey.find("VAR_SUMMARY") != string::npos) 
    {
        return OnVarSummaryRequested(Msg, MsgTxList);
    }
    else if(Msg.m_sKey.find("DB_CLEAR")!=string::npos)
    {
        return OnClearRequested(Msg,MsgTxList);
    }
    
    
    
    return false;
}

bool CMOOSDB::OnProcessSummaryRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
    DBVAR_MAP::iterator p;
    STRING_LIST::iterator q;
    
    STRING_LIST Clients;
    
    m_CommServer.GetClientNames(Clients);
    
    
    for(q=Clients.begin();q!=Clients.end();q++)
    {
        string sWho = *q;
        
        string sPublished= "PUBLISHED=";
        string sSubscribed = "SUBSCRIBED=";
        
        for(p=m_VarMap.begin();p!=m_VarMap.end();p++)
        {
            CMOOSDBVar  & rVar = p->second;
            
            if(rVar.m_Writers.find(sWho)!=rVar.m_Writers.end())
            {
                if(!sPublished.empty())
                {
                    sPublished+=",";
                }
                sPublished+=rVar.m_sName;
                
            }
            
            if(rVar.m_Subscribers.find(sWho)!=rVar.m_Subscribers.end())
            {
                if(!sSubscribed.empty())
                {
                    sSubscribed+=",";
                }
                sSubscribed+=rVar.m_sName;
            }
        }
        
        
        CMOOSMsg MsgReply;
        
        MsgReply.m_nID        = Msg.m_nID;
        
        MsgReply.m_cMsgType   = MOOS_NOTIFY;
        MsgReply.m_cDataType  = MOOS_STRING;
        MsgReply.m_dfTime     = MOOSTime()-m_dfStartTime; //for display
        MsgReply.m_sSrc       = m_sDBName;
        MsgReply.m_sKey       = "PROC_SUMMARY";
        MsgReply.m_sVal       = sWho+":"+sSubscribed+","+sPublished;
        MsgReply.m_dfVal      = -1;
        
        
        MsgTxList.push_front(MsgReply);
    }
    
    return true;
    
}

bool CMOOSDB::OnServerAllRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
    
    
    DBVAR_MAP::iterator p;
    
    for(p=m_VarMap.begin();p!=m_VarMap.end();p++)
    {
        CMOOSDBVar  & rVar = p->second;
        
        CMOOSMsg MsgVar;
        
        MsgVar.m_nID        = Msg.m_nID;
        
        MsgVar.m_cMsgType   = MOOS_NOTIFY;
        MsgVar.m_cDataType  = rVar.m_cDataType;
        MsgVar.m_dfTime     = rVar.m_dfWrittenTime-m_dfStartTime; //for display
        MsgVar.m_sSrc       = rVar.m_sWhoChangedMe;
        MsgVar.m_sKey       = rVar.m_sName;
        MsgVar.m_sVal       = rVar.m_sVal;
        MsgVar.m_dfVal      = rVar.m_dfVal;
        MsgVar.m_dfVal2     = rVar.m_dfWriteFreq;
        MsgVar.m_sOriginatingCommunity = rVar.m_sOriginatingCommunity;
        
        if(MsgVar.m_dfTime<0) 
        {
            MsgVar.m_dfTime =-1;
        }
        
        MsgTxList.push_front(MsgVar);
        
        
    }
    
    
    return true;
}

//Suggested addition by MIT users 2006 - shorter version of OnProcessSummary
bool CMOOSDB::OnVarSummaryRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
    std::string TheVars;
    DBVAR_MAP::iterator p;
    for(p = m_VarMap.begin(); p != m_VarMap.end(); p++) 
    {
        //look to a comma
        if(p!=m_VarMap.begin())
            TheVars += ",";

        TheVars += p->first;
    }
    
    CMOOSMsg Reply;

    //so the client knows the query result correspondences
    Reply.m_nID = Msg.m_nID; 
    Reply.m_cMsgType = MOOS_NOTIFY;
    Reply.m_cDataType = MOOS_STRING;
    Reply.m_dfTime = MOOSTime();
    Reply.m_sSrc = m_sDBName;
    Reply.m_sKey = "VAR_SUMMARY";
    Reply.m_sVal = TheVars;
    Reply.m_dfVal = -1;

    MsgTxList.push_front(Reply);

    return true;
}





bool CMOOSDB::OnClearRequested(CMOOSMsg &Msg, MOOSMSG_LIST &MsgTxList)
{
    MOOSTrace("Clear Down Requested:\n");
    
    MOOSTrace("    Resetting %d variables...",m_VarMap.size());
    
    DBVAR_MAP::iterator p;
    
    for(p = m_VarMap.begin();p!=m_VarMap.end();p++)
    {
        CMOOSDBVar & rVar = p->second;
        rVar.Reset();
    }
    MOOSTrace("done\n");
    
    
    MOOSTrace("    Removing %d existing notification queues...",m_HeldMailMap.size());
    MOOSMSG_LIST_STRING_MAP::iterator q;
    
    for(q = m_HeldMailMap.begin();q!=m_HeldMailMap.end();q++)
    {
        MOOSMSG_LIST & rList = q->second;
        rList.clear();
    }
    MOOSTrace("done\n");
    
    //MOOSTrace("    resetting DB start Time...done\n");
    LogStartTime();
    
    
    return true;
    
}


void CMOOSDB::LogStartTime()
{
    m_dfStartTime = MOOSTime();
    
    return;
    string sKey = "DB_START_TIME";
    
    
    DBVAR_MAP::iterator p = m_VarMap.find("DB_START_TIME");
    
    if(p==m_VarMap.end())
    {
        
        CMOOSDBVar NewVar(sKey);
        
        NewVar.m_cDataType = MOOS_DOUBLE;
        NewVar.m_dfVal  = m_dfStartTime;
        NewVar.m_dfTime = m_dfStartTime;
        NewVar.m_sWhoChangedMe   = m_sDBName;
        NewVar.m_sOriginatingCommunity = m_sCommunityName;
        
        m_VarMap[sKey] = NewVar;
        
    }
    else
    {
        CMOOSDBVar & rVar = p->second;
        rVar.m_dfVal = m_dfStartTime;
        rVar.m_dfTime = m_dfStartTime;
    }
    
}



bool CMOOSDB::VariableExists(const string &sVar)
{
    DBVAR_MAP::iterator p=m_VarMap.find(sVar);
    
    return (!m_VarMap.empty()) && (p!=m_VarMap.end());
}

void CMOOSDB::Var2Msg(CMOOSDBVar &Var, CMOOSMsg &Msg)
{
    Msg.m_dfTime = Var.m_dfTime;
    Msg.m_cDataType = Var.m_cDataType;
    Msg.m_sSrc = Var.m_sWhoChangedMe;
    Msg.m_sKey = Var.m_sName;
    Msg.m_sOriginatingCommunity = Var.m_sOriginatingCommunity;
    switch(Var.m_cDataType)
    {
    case MOOS_DOUBLE:
        Msg.m_dfVal = Var.m_dfVal;
        break;
    case MOOS_STRING:
        Msg.m_sVal = Var.m_sVal;
        break;
    }
}


