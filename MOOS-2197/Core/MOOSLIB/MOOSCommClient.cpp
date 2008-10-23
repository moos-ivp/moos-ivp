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
    #define isnan _isnan
#else
    #include <pthread.h>
#endif

#include <cmath>
#include <string>
#include <set>
#include <limits>
#include <iostream>
#include <iomanip>
using namespace std;

#include <MOOSGenLib/MOOSGenLib.h>
#include "XPCTcpSocket.h"
#include "MOOSCommClient.h"
#include "MOOSCommPkt.h"
#include "MOOSGlobalHelper.h"
#include "MOOSException.h"
#include "MOOSSkewFilter.h"
#include <iostream>
#include <cmath>



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




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

	m_nOutPendingLimit = OUTBOX_PENDING_LIMIT;
	m_nInPendingLimit = INBOX_PENDING_LIMIT;
	m_bConnected = false;
	m_nFundamentalFreq = CLIENT_DEFAULT_FUNDAMENTAL_FREQ;
	m_nNextMsgID=0;
	m_bFakeSource = false;
    m_bQuiet= false;
    
    //by default this client will adjust the local time skew
    //by using time information sent by the CommServer sitting
    //at the other end of this conenection.
    m_bDoLocalTimeCorrection = true;
	
	m_bMailPresent = false;
    
    SetVerboseDebug(false);

	SocketsInit();

}

CMOOSCommClient::~CMOOSCommClient()
{
	Close();
}

bool CMOOSCommClient::Run(const char *sServer, long lPort, const char * sMyName, unsigned int nFundamentalFrequency)
{
	m_bQuit = false;

	//do advert
	DoBanner();

	//who are we going to be talking to?
	m_sDBHost = sServer;

	//and on what port are they listening
	m_lPort = lPort;

	//and what are we called?
	m_sMyName = sMyName;

	if(m_pfnConnectCallBack==NULL)
	{
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
        MOOSTrace("setting comms tick\n");
        m_nFundamentalFreq = (int)nCommTick;
        if(m_nFundamentalFreq==0)//catch a stupid setting
            m_nFundamentalFreq = 1;
        return true;
    }
    
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


bool CMOOSCommClient::ClientLoop()
{

	//MOOSTrace("ClientLoop() Begins\n");s
    double dfTDebug = MOOSLocalTime();
	while(!m_bQuit)
	{
		//this is the connect loop...
		m_pSocket = new XPCTcpSocket(m_lPort);

		if(ConnectToServer())
		{

			while(!m_bQuit)
			{
                
                if(m_bVerboseDebug)
                {
					MOOSTrace("COMMSCLIENT DEBUG: Tick period %f ms (should be %d ms)\n",MOOSLocalTime()-dfTDebug,(int)(1000.0/m_nFundamentalFreq));
                    dfTDebug = MOOSLocalTime();
                }
                                   
				if(!DoClientWork())
					break;
                
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

	MOOSTrace("CMOOSCommClient::ClientLoop() quits\n");

	m_bConnected = false;
	return true;
}


bool CMOOSCommClient::DoClientWork()
{
	//this is the IO Loop
	try
	{

		//note the symmetry here... a warm feeling

		CMOOSCommPkt PktTx,PktRx;
        
        bool bNullPacket  = false;

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
                bNullPacket = true;
			}


			//convert our out box to a single packet
			PktTx.Serialize(m_OutBox,true);

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

		//quick! grab this time
		double dfLocalPktRxTime = MOOSLocalTime();
        
        if(m_bVerboseDebug)
        {
            MOOSTrace("COMMSERVER DEBUG: completed call to DB after %f s\n",dfLocalPktRxTime-dfLocalPktRxTime);
        }
                 


		m_InLock.Lock();
		{
			if(m_InBox.size()>m_nInPendingLimit)
			{
				MOOSTrace("Too many unread incoming messages [%d] : purging\n",m_InBox.size());
				MOOSTrace("The user must read mail occasionally");
				m_InBox.clear();
			}

			//convert reply into a list of mesasges :-)
			//but no NULL messages
			//we ask serialise also to return the DB time
			//by looking in the first NULL_MSG in the packet
			double dfServerPktTxTime=numeric_limits<double>::quiet_NaN();

			//extract...
			PktRx.Serialize(m_InBox,false,true,&dfServerPktTxTime);

			//did you manage to grab the DB time while you were there?
			if(m_bDoLocalTimeCorrection && !isnan(dfServerPktTxTime))
            {
				UpdateMOOSSkew(dfLocalPktTxTime, dfServerPktTxTime, dfLocalPktRxTime);
            }
            
       
			m_bMailPresent = true;
		}
		m_InLock.UnLock();

        if(m_pfnMailCallBack!=NULL && m_bMailPresent)
        {
            bool bUserResult = (*m_pfnMailCallBack)(m_pMailCallBackParam);
            if(!bUserResult)
                MOOSTrace("user mail callback returned false..is all ok?\n");
        }

        
        
	}
	catch(CMOOSException e)
	{
		MOOSTrace("Exception in ClientLoop() : %s\n",e.m_sReason);
		OnCloseConnection();
		return false;//jump out to connect loop....				
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
	    MOOSTrace("\n---------------MOOS CONNECT-----------------------\n");


	while(!m_bQuit)
	{
        if(!m_bQuiet)
		    MOOSTrace("  contacting a MOOS server %s:%d -  try %.5d ",m_sDBHost.c_str(),m_lPort,++nAttempt);

		try
		{
			m_pSocket->vConnect(m_sDBHost.c_str());
			break;
		}
		catch(XPCException e)
		{
			//connect failed....
		    UNUSED_PARAMETER(e);

			if(m_pSocket)
				delete m_pSocket;

			m_pSocket = new XPCTcpSocket(m_lPort);

			MOOSPause(100);
			MOOSTrace("\r");
		}
	}

	if(m_bQuit)
	{
		MOOSTrace("ConnectToServer returns early\n");
		return false;
	}

    if(!m_bQuiet)
	    MOOSTrace("\n  Contact Made\n");


	if(HandShake())
	{
		if(m_pfnConnectCallBack!=NULL)
		{

			//we must be connected for user callback to work..
			m_bConnected = true;

			if(!m_bQuiet)
                MOOSTrace("  Invoking User OnConnect() callback...");

			//invoke user defined callback
			bool bUserResult = (*m_pfnConnectCallBack)(m_pConnectCallBackParam);
			if(bUserResult)
			{
    			if(!m_bQuiet)
	    			MOOSTrace("ok\n");
			}
			else
			{
		    	if(!m_bQuiet)
			    	MOOSTrace("fail\n");
                else
                    MOOSTrace("failed User OnConnect() callback\n");
			}

		}
	}
	else
	{
		m_bQuit = true;

		if(m_pSocket)
			delete m_pSocket;

		m_pSocket = new XPCTcpSocket(m_lPort);
		return false;
	}

    if(!m_bQuiet)
        MOOSTrace("--------------------------------------------------\n\n");
	return true;
}

/** this is called by user of a CommClient object
to send a Msg to MOOS */
bool CMOOSCommClient::Post(CMOOSMsg &Msg)
{
	if(!IsConnected())
		return false;

	m_OutLock.Lock();

	//stuff our name in here  - prevent client from having to worry about
	//it...
	if(!m_bFakeSource )
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
	//set up Message ID;
	Msg.m_nID=m_nNextMsgID++;

	m_OutBox.push_front(Msg);

	if(m_OutBox.size()>m_nOutPendingLimit)
	{	
		MOOSTrace("\nThe outbox is very full. This is suspicious and dangerous.\n");
		MOOSTrace("\nRemoving old unsent messages as new ones are added\n");
		//remove oldest message...
		m_OutBox.pop_back();
	}

	m_OutLock.UnLock();

	return true;

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

	for(p = m_InBox.begin();p!=m_InBox.end();p++)
	{
		CMOOSMsg & rMsg = *p;
		if(!rMsg.IsType(MOOS_NULL_MSG))
		{
			//only give client non NULL Msgs

			MsgList.push_front(rMsg);
		}
	}

	//remove all elements
	m_InBox.clear();

	m_bMailPresent = false;

	m_InLock.UnLock();

	return !MsgList.empty();
}

bool CMOOSCommClient::HandShake()
{
	try
	{
        if(!m_bQuiet)
		    MOOSTrace("  Handshaking as \"%s\"\n",m_sMyName.c_str());

        if(m_bDoLocalTimeCorrection)
		    SetMOOSSkew(0);

		//a little bit of handshaking..we need to say who we are
		CMOOSMsg Msg(MOOS_DATA,"",(char *)m_sMyName.c_str());

		SendMsg(m_pSocket,Msg);

		CMOOSMsg WelcomeMsg;

		ReadMsg(m_pSocket,WelcomeMsg);

		if(WelcomeMsg.IsType(MOOS_POISON))
		{
            if(!m_bQuiet)
            {
			    MOOSTrace("..failed\n");
			    MOOSTrace("->   MOOS Server Poisoned me....\n");
			    MOOSTrace("->   What I did wrong was :\"%s\"",WelcomeMsg.m_sVal.c_str());
            }
            else
            {
                MOOSTrace("Breaking a vow of silence - handshaking failed (poisoned)\n");
            }
			return false;
		}
		else
		{
			//read our skew
	       if(!m_bQuiet)
                MOOSTrace("  Handshaking Complete\n");

			double dfSkew = WelcomeMsg.m_dfVal;
            if(m_bDoLocalTimeCorrection)
			    SetMOOSSkew(dfSkew);


		}
	}
	catch(CMOOSException e)
	{
		MOOSTrace("Exception in hand shaking :%s",e.m_sReason);
		return false;
	}
	return true;

}

bool CMOOSCommClient::IsConnected()
{
	return m_bConnected;
}

bool CMOOSCommClient::OnCloseConnection()
{
	if(!m_bQuiet)
		MOOSTrace("closing connection...");
	m_pSocket->vCloseSocket();
	if(m_pSocket)
		delete m_pSocket;
	m_pSocket= NULL;
	m_bConnected = false;
	if(!m_bQuiet)
		MOOSTrace("done\n");

	ClearResources();

	if(m_pfnDisconnectCallBack!=NULL)
	{
		if(!m_bQuiet)
			MOOSTrace("Invoking User OnDisconnect() callback...");
		//invoke user defined callback
		bool bUserResult = (*m_pfnDisconnectCallBack)(m_pDisconnectCallBackParam);
		if(bUserResult)
		{
			if(!m_bQuiet)
				MOOSTrace("ok\n");
		}
		else
		{
			if(!m_bQuiet)
				MOOSTrace("returned fail\n");
		}

	}


	return true;
}

void CMOOSCommClient::DoBanner()
{
    if(m_bQuiet)
        return ;

	MOOSTrace("****************************************************\n");
	MOOSTrace("*                                                  *\n");
	MOOSTrace("*       This is MOOS Client                        *\n");
	MOOSTrace("*       c. P Newman 2001                           *\n");
	MOOSTrace("*                                                  *\n");
#ifdef _WIN32
	//MOOSTrace("*       built on %s\n",__TIMESTAMP__);             
#endif
	MOOSTrace("****************************************************\n");

}

bool CMOOSCommClient::UnRegister(const string &sVar)
{
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

bool CMOOSCommClient::Register(const string &sVar, double dfInterval)
{
	if(sVar.empty())
		return MOOSFail("\n ** WARNING ** Cannot register for \"\" (empty string)\n");

	if(m_Registered.find(sVar)==m_Registered.end() || m_Registered.empty())
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

bool CMOOSCommClient::Notify(const string &sVar, double dfVal, double dfTime)
{
	CMOOSMsg Msg(MOOS_NOTIFY,sVar.c_str(),dfVal,dfTime);

	m_Published.insert(sVar);

	return Post(Msg);

}

bool CMOOSCommClient::Notify(const string &sVar, const string & sVal, double dfTime)
{
	CMOOSMsg Msg(MOOS_NOTIFY,sVar.c_str(),sVal.c_str(),dfTime);

	m_Published.insert(sVar);

	return Post(Msg);
}

bool CMOOSCommClient::ServerRequest(const string &sWhat,MOOSMSG_LIST  & MsgList, double dfTimeOut, bool bClear)
{
    if(!IsConnected())
        return false;
    
	CMOOSMsg Msg(MOOS_SERVER_REQUEST,sWhat.c_str(),"");
	Post(Msg);

	//now the ID of the transaction  is now in Msg as we passed a reference

	int nIDRequired = Msg.m_nID;

	int nSleep = 100;

	double dfWaited = 0.0;

	while(dfWaited<dfTimeOut)
	{
		if (Peek(MsgList, nIDRequired, bClear)) 
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
		p++;
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

	for(p = Mail.begin();p!=Mail.end();p++)
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

bool CMOOSCommClient::Close(bool bNice )
{

	m_bQuit = true;
	
    m_ClientThread.Stop();
    
    int i = 0;

	while(m_bConnected )
	{
		MOOSPause(100);

		if(++i>100)
		{
			MOOSTrace("failed to close MOOSClient object! Most Strange.\n");
			return false;
		}
	}

	ClearResources();

	return true;
}

bool CMOOSCommClient::FakeSource(bool bFake)
{
	m_bFakeSource = bFake;
	return true;
}

bool CMOOSCommClient::ClearResources()
{
	if(!m_bQuiet)
		MOOSTrace("purging out box...");
	m_OutLock.Lock();
	m_OutBox.clear();
	m_OutLock.UnLock();
	if(!m_bQuiet)
		MOOSTrace("done\n");

	if(!m_bQuiet)
		MOOSTrace("purging in box...");
	m_InLock.Lock();
	m_InBox.clear();
	m_InLock.UnLock();
	if(!m_bQuiet)
        MOOSTrace("done\n");

	if(!m_bQuiet)
		MOOSTrace("clearing registered set...");
	m_Registered.clear();
	if(!m_bQuiet)
		MOOSTrace("done\n");

	return true;

}

string CMOOSCommClient::GetDescription()
{
	return MOOSFormat("%s@%d",m_sDBHost.c_str(),m_lPort);
}

string CMOOSCommClient::GetLocalIPAddress()
{
	char Name[255];
	if(gethostname(Name,sizeof(Name))!=0)
	{
		MOOSTrace("Error getting host name\n");
		return "unknown";
	}
	return std::string(Name);
}


//std::auto_ptr<std::ofstream> SkewLog(NULL);

bool CMOOSCommClient::UpdateMOOSSkew(double dfRqTime, double dfTxTime,double dfRxTime)
{
	double dfOldSkew = GetMOOSSkew();

	// This function need to be provided MOOSLocal time stamps!

	//back out correction which has already been made..
	//dfRqTime-=dfOldSkew;
	//dfRxTime-=dfOldSkew;

	if (!m_pSkewFilter.get())
	{
		// Make a fresh skew filter
		m_pSkewFilter = std::auto_ptr<MOOS::CMOOSSkewFilter>(new MOOS::CMOOSSkewFilter);
		if (!m_pSkewFilter.get()) return false;
	}

	MOOS::CMOOSSkewFilter::tSkewInfo skewinfo;
	double dfNewSkew = m_pSkewFilter->Update(dfRqTime, dfTxTime, dfRxTime, &skewinfo);


/*
	This is the stuff that was replaced
	
	double dfMeasuredSkew = dfTxTime-dfRxTime;

	double dfNewSkew;
	if(dfOldSkew!=0.0)
	{
		dfNewSkew = 0.9*dfOldSkew+0.1*dfMeasuredSkew;	
		//MOOSTrace("Tx Time (@DB) = %.4f Localtime (@localhost) = %.4f Skew = %.4f smoothed skew = %.5f seconds\n",dfTxTime,HPMOOSTime(),dfMeasuredSkew,dfNewSkew);
	}
	else
	{
		dfNewSkew = dfMeasuredSkew;
	}
*/

/*
	if (SkewLog.get())
	{
	    SkewLog->setf(std::ios::fixed);
	    (*SkewLog) << 
		"RQ=" << setprecision(6) << dfRqTime << "," <<
		"TX=" << setprecision(6) << dfTxTime << "," <<
		"RX=" << setprecision(6) << dfRxTime << "," <<
		"m=" << setprecision(6) << skewinfo.m << "," <<
		"c=" << setprecision(6) << skewinfo.c << "," <<
		"LB=" << setprecision(6) << skewinfo.LB << "," <<
		"UB=" << setprecision(6) << skewinfo.UB << "," <<
		"envLB=" << setprecision(6) << skewinfo.envLB << "," <<
		"envUB=" << setprecision(6) << skewinfo.envUB << "," <<
		"envEst=" << setprecision(6) << skewinfo.envEst << "," <<
		"filtEst=" << setprecision(6) << skewinfo.filtEst <<
		std::endl;		
	}
*/

	SetMOOSSkew(dfNewSkew);

	return true;
}
