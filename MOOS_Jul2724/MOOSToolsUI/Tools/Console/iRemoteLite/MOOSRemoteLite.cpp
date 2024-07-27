///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a  MOOS Basic (Common) Application.
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


// MOOSRemote.cpp: implementation of the CMOOSRemote class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
     #pragma warning(disable : 4786)
#endif



#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include "MOOSRemoteLite.h"
#include <stdio.h>



using namespace std;


#define WD_PERIOD 3.0
#define HUMAN_TIMEOUT 20.0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////

#ifdef _WIN32

DWORD WINAPI WDLoopProc( LPVOID lpParameter)
{

    CMOOSRemoteLite* pMe =     (CMOOSRemoteLite*)lpParameter;

    return pMe->WDLoop();
}

DWORD WINAPI MailLoopProc( LPVOID lpParameter)
{

    CMOOSRemoteLite* pMe =     (CMOOSRemoteLite*)lpParameter;

    return pMe->MailLoop();
}

#else

void * WDLoopProc( void * lpParameter)
{

    MOOSTrace("starting watchdog thread...");
    CMOOSRemoteLite* pMe =     (CMOOSRemoteLite*)lpParameter;

    pMe->WDLoop();

    return NULL;
}

void * MailLoopProc( void * lpParameter)
{

    MOOSTrace("starting mail reading thread,,,\n");
    CMOOSRemoteLite* pMe =     (CMOOSRemoteLite*)lpParameter;

    pMe->MailLoop();

    return NULL;
}


#endif



CMOOSRemoteLite::CMOOSRemoteLite()
{
    m_dfCurrentElevator    =    0;
    m_dfCurrentRudder    =    0;
    m_dfCurrentThrust    =    0;
    m_bQuit = true;
    m_bManualOveride = true;
    m_dfTimeLastSent = MOOSTime();
    m_dfTimeLastAck  = MOOSTime();

    EnableMailLoop(true);

}

CMOOSRemoteLite::~CMOOSRemoteLite()
{
}

bool CMOOSRemoteLite::StopThreads()
{
    m_bQuit = true;

#ifdef _WIN32
    WaitForSingleObject(m_hWDThread,INFINITE);
#else
    void * Result;
    pthread_join( (pthread_t)m_nWDThreadID,&Result);
#endif

    return true;
}

bool CMOOSRemoteLite::StartThreads()
{
    m_bQuit = false;

#ifdef _WIN32
    //this is the main WD thread
    m_hWDThread = ::CreateThread(    NULL,
        0,
        WDLoopProc,
        this,
        CREATE_SUSPENDED,
        &m_nWDThreadID);
    ResumeThread(m_hWDThread);

    //this is the main WD thread
    m_hMailThread = ::CreateThread(    NULL,
        0,
        MailLoopProc,
        this,
        CREATE_SUSPENDED,
        &m_nMailThreadID);
    ResumeThread(m_hMailThread);


#else


    int Status = pthread_create( (pthread_t*)& m_nWDThreadID,NULL,WDLoopProc,this);

    if(Status!=0)
    {
        return false;
    }


    Status = pthread_create( (pthread_t*)& m_nMailThreadID,NULL,MailLoopProc,this);

    if(Status!=0)
    {
        return false;
    }



#endif

    return true;
}



//overloaded version of Run (don't want default behaviour
bool CMOOSRemoteLite::Run( const char * sName,
                      const char * sMissionFile)
{
    //save absolutely crucial info...
    m_sAppName      = sName;
    m_MissionReader.SetAppName(m_sAppName);
    m_sMOOSName = m_sAppName;
    m_sMissionFile = sMissionFile;
    
    //what time did we start?
    m_dfAppStartTime = MOOSTime();

    //can we see the mission file
    if(sMissionFile!=NULL)
    {
        if(!m_MissionReader.SetFile(m_sMissionFile.c_str()))
        {
            MOOSTrace("Warning Mission File not found\n");
//            return false;
        }
    }

    //can we start the communications ?
    if(!ConfigureComms())
    {
        return false;
    }

    //start WD
    StartThreads();

    MOOSPause(1000);

    OnStartUp();

    while(!m_bQuit)
    {
        char cCmd = MOOSGetch();

        HitInputWD();

        switch(cCmd)
        {
        case '?':
        case 'h':
            PrintHelp();
            break;

        case '`':
            HitInputWD();
            MOOSTrace("Ok\n");
            break;

        case '\n':
        case '\r':
            MOOSTrace(">\n");
            break;


        case 'q':
            MOOSTrace("really quit? (y)");
            if(MOOSGetch()=='y')
            {
                MOOSTrace("iRemote is shutting down...making things safe:\n");
                MOOSTrace("Setting all to zero\n");

                m_dfCurrentThrust    = 0;
                m_dfCurrentRudder    =0;
                m_dfCurrentElevator    = 0;

                MOOSTrace("Turning Manual Control On\n");
                SetManualOveride(true);

                MOOSTrace("bye...\n");
                MOOSPause(1000);
                m_bQuit = true;
            }
            else
            {
                MOOSTrace("cancelled\n");
            }
            break;

        case 'm':
            m_dfCurrentRudder+=2.0;
            MOOSTrace("Setting RudderTo %7.3f\n",m_dfCurrentRudder);
            break;

        case 'n':
            m_dfCurrentRudder-=2.0;
            MOOSTrace("Setting RudderTo %7.3f\n",m_dfCurrentRudder);
            break;

        case ',':
            m_dfCurrentRudder=0;
            MOOSTrace("Setting RudderTo %7.3f\n",m_dfCurrentRudder);
            break;


        case ' ':
            m_dfCurrentThrust    = 0;
            m_dfCurrentRudder    =0;
            m_dfCurrentElevator    = 0;
            MOOSTrace("Setting all to zero\n");
            SendDesired();
            SetManualOveride(true);

            break;

        case 'z':
            m_dfCurrentThrust -= 2.0;
            if(m_dfCurrentThrust<=-100)
            {
                m_dfCurrentThrust=-100;
            }
            MOOSTrace("Setting Thrust to %7.3f\n",m_dfCurrentThrust);
            break;

        case 'a':
            m_dfCurrentThrust += 2.0;
            if(m_dfCurrentThrust>=100)
            {
                m_dfCurrentThrust=100;
            }

            MOOSTrace("Setting Thrust to %7.3f \n",m_dfCurrentThrust);
            break;



        case 'p':
            m_dfCurrentElevator += 2.0;
            MOOSTrace("Setting Elevator to %7.3f\n",m_dfCurrentElevator);
            break;

        case 'l':
            m_dfCurrentElevator -= 2.0;
            MOOSTrace("Setting Elevator to %7.3f\n",m_dfCurrentElevator);
            break;

        case 'r':
            m_Comms.Notify("ZERO_RUDDER",true);
            MOOSTrace("Setting Rudder Zero...\n");
            m_dfCurrentRudder = 0;
            break;

        case 'e':
            m_Comms.Notify("ZERO_ELEVATOR",true);
            MOOSTrace("Setting Elevator Zero...\n");
            m_dfCurrentElevator = 0;
            break;


        case '/':
        m_Comms.Notify("COLLISION_OK",true);
        MOOSTrace("Clearing Collision Flag");
        break;

        case 'O':
        case 'o':
            SetManualOveride(!m_bManualOveride);
            break;

        case 'R':
            MOOSTrace("really restart Helm? (y/n):");
            cCmd = MOOSGetch();
            if(cCmd=='y')
            {
                m_Comms.Notify("RESTART_HELM","TRUE");
                MOOSTrace("Sending RESTART_HELM signal\n");
            }
            else
            {
                MOOSTrace("cancelled\n");
            }
            break;

        case 'F':
            FetchDB();
            break;

        case 'I':
            ReStartAll();
            break;


		case '+':
			PrintCustomSummary();
			break;



        case 'C':
            MOOSTrace("really reset DB? (y/n):");
            cCmd = MOOSGetch();
            if(cCmd=='y')
            {
                MOOSMSG_LIST List;
                MOOSTrace("Sending \"DB_CLEAR\" signal\n");
                m_Comms.ServerRequest("DB_CLEAR",List);
            }
            else
            {
                MOOSTrace("cancelled\n");
            }
            break;

        case 'S':
            m_Comms.Notify("SIM_RESET","TRUE");
            MOOSTrace("Resetting simulator...\n");
            break;

        case 'G':
            m_Comms.Notify("LOGGER_RESTART","TRUE");
            MOOSTrace("Restarting Logger...\n");
            break;

        case 'D':
            m_Comms.Notify("ZERO_DEPTH","TRUE");
            MOOSTrace("zeroing depth sensor...\n");
            break;

        case 'H':
            m_Comms.Notify("RESET_ACTUATION","TRUE");
            MOOSTrace("resetting actuation...\n");

            m_dfCurrentThrust    = 0;
            m_dfCurrentRudder    =0;
            m_dfCurrentElevator    = 0;
            break;

        case 'V':
            MOOSTrace("really restart Navigator? (y/n):\a");
            cCmd = MOOSGetch();
            if(cCmd=='y')
            {
                m_Comms.Notify("RESTART_NAV","TRUE");
                MOOSTrace("Sending RESTART_NAV signal\n");
            }
            else
            {
                MOOSTrace("cancelled\n");
            }
            break;


        case 'T':
            MOOSTrace("Requesting Navigation Status...\n");
            m_Comms.Notify("NAV_SUMMARY_REQUEST","TRUE");
            break;

        case '*':
            PrintNavSummary();
            break;



        default:
            if(!DoCustomKey(cCmd) && !DoCustomJournal(cCmd))
            {
                MOOSTrace("%c is not a command. Press \"?\" for key mappings\n",cCmd);
                continue;
            }
            break;

        }


        SendDesired();

    }

    StopThreads();

    return true;
}



void CMOOSRemoteLite::PrintHelp()
{

    const char *Help[] = {
        "`     Acknowledge",
            "W     Actuation Test",
            "O     Toggle MANUAL OVERIDE",
            "R     RESTART Helm",
            "C     Clear Server",
            "S     Reset Simulator",
            "G     Restart Logger",
            "D     Zero Depth Sensor",
            "H     Restart Actuation Driver",
            "F     Fetch DataBase",
            "U     Scheduler Control",
            "m     Rudder   Right",
            "n     Rudder   Left",
            ",     Rudder   Zero",
            "/     Clear Collision Flag",
            "r     Rudder   Set Home",
            "a     Thrust   Increase",
            "z     Thrust   Decrease",
            "A     Full Ahead",
            "Z     Full Reverse",
            "p     Elevator Up",
            "l     Elevator Down",
            "e     Elevator Set Home",
            "T     Navigation Status",
            "V     Reset Navigator",
            "+     Custom Summary",
            "q     Quit",

            "space ALL      zero",
            "?/h   Help",
            "\n"

    };



    for(unsigned int i = 0;i<sizeof(Help)/sizeof(char*);i++)
    {
        MOOSTrace("%s\n",Help[i]);
    }

    MOOSTrace("Custom Bindings:\n");

    CUSTOMKEY_MAP::iterator p;

    for(p=m_CustomKeys.begin();p!=m_CustomKeys.end();p++)
    {
        CCustomKey & rKey = p->second;

        MOOSTrace("%c     %s with %s\n",
            rKey.m_cChar,
            rKey.m_sKey.c_str(),
            rKey.m_sVal.c_str());
    }



}

bool CMOOSRemoteLite::MailLoop()
{
    m_bRunMailLoop = true;
    while(!m_bQuit)
    {
        MOOSPause(300);

        MOOSMSG_LIST MailIn;
        if(m_bRunMailLoop && m_Comms.Fetch(MailIn))
        {
            //process mail
            //simply write out
            MOOSMSG_LIST::iterator p;

            //make it in time order
            MailIn.sort();
            MailIn.reverse();

            for(p = MailIn.begin();p!=MailIn.end();p++)
            {

                if(p->IsSkewed(MOOSTime()))
                    continue;
                if(MOOSStrCmp(p->m_sKey,"NAV_SUMMARY"))
                {
                    DoNavSummary(*p);
                }
                else

                if(p->GetKey().find("DEBUG")!=string::npos)
                {
                    //we print MOOS_DEBUG messages to the screen
                    string sMsg = p->m_sVal;
                    MOOSRemoveChars(sMsg,"\r\n");

                    MOOSTrace(">%-10s @ %7.2f \"%s\"\n",
                        p->m_sSrc.c_str(),
                        p->m_dfTime-GetAppStartTime(),
                        sMsg.c_str());
                }

                else
                {
                    CUSTOMJOURNAL_MAP::iterator w = m_CustomJournals.find(p->GetKey());
                    if(w!=m_CustomJournals.end())
                    {
                        w->second.Add(p->GetAsString());
                    }

                }
            }


            UpdateMOOSVariables(MailIn);

        }

    }
    return true;
}



bool CMOOSRemoteLite::WDLoop()
{
    MOOSTrace("WD Loop is running...\n");
    while(!m_bQuit)
    {
        MOOSPause((int)(1000*WD_PERIOD*0.8));
        if(MOOSTime()-m_dfTimeLastSent>WD_PERIOD)
        {
            SendDesired();
        }

        if(m_bManualOveride)
        {
            double dfDT = MOOSTime()-m_dfTimeLastAck;

            if(dfDT>HUMAN_TIMEOUT*0.5 && dfDT<HUMAN_TIMEOUT*0.65)
            {
                MOOSTrace("Requesting Operator Acknowledgement... (hit `)\n\a");
            }
            if(dfDT>HUMAN_TIMEOUT)
            {
                m_dfCurrentRudder=0;
                m_dfCurrentThrust=0;
                m_dfCurrentElevator = 0;
                SendDesired();
                if(dfDT<HUMAN_TIMEOUT+2)
                {
                    MOOSTrace("No operator input : ALLSTOPPED @ %f\n",MOOSTime()-m_dfAppStartTime);

                }
            }
        }
    }

    return true;
}

bool CMOOSRemoteLite::SendDesired()
{
    m_Lock.Lock();
    if(m_Comms.IsConnected())
    {
        if(m_bManualOveride)
        {
            m_Comms.Notify("DESIRED_RUDDER",m_dfCurrentRudder);
            m_Comms.Notify("DESIRED_THRUST",m_dfCurrentThrust);
            m_Comms.Notify("DESIRED_ELEVATOR",m_dfCurrentElevator);
            //MOOSTrace("Sending Control @ %12.3f\n",MOOSTime());
            m_dfTimeLastSent = MOOSTime();
        }
    }
    m_Lock.UnLock();
    return true;
}


bool CMOOSRemoteLite::FetchDB()
{
    EnableMailLoop(false);

    MOOSTrace("\n\n******   DB Fetch   ******\n");

    MOOSMSG_LIST MsgList;
    if(m_Comms.ServerRequest("ALL",MsgList))
    {
        MOOSMSG_LIST::iterator p;
        for(p=MsgList.begin();p!=MsgList.end();p++)
        {
            CMOOSMsg & rMsg = *p;
            ostringstream os;
            //ostrstream os;

            os<<setw(22);
            os<<rMsg.m_sKey.c_str();
            os<<" ";


            os<<setw(16);
            os<<rMsg.m_sSrc.c_str();
            os<<" ";

            os<<setw(9);
            os<<rMsg.m_dfTime;
            os<<" ";

            os<<setw(10);
            switch(rMsg.m_cDataType)
            {
            case MOOS_STRING:
                os<<rMsg.m_sVal.c_str();
                break;
            case MOOS_NOT_SET:
                os<<"*";
                break;
			case MOOS_BINARY_STRING:
				os<<rMsg.GetAsString().c_str();
				break;
					
            default:
                os<<" ";
                break;
            }

            os<<" ";
            os<<setw(14);
            switch(rMsg.m_cDataType)
            {
            case MOOS_DOUBLE:
                os<<rMsg.m_dfVal;
                break;

            case MOOS_NOT_SET:
                os<<"*";
                break;

            default:
                os<<" ";
                break;
            }


            os<<endl<<ends;

            string sText = os.str();
            MOOSTrace("%s",sText.c_str());

            //os.rdbuf()->freeze(0);

        }
    }
    else
    {
        MOOSTrace("DB failed to talk to me\n");
    }

    //renable mail loop...
    EnableMailLoop(true);

    return true;
}

bool CMOOSRemoteLite::OnStartUp()
{


    MakeCustomSummary();

    MakeCustomJournals();

    MakeCustomKeys();


    m_NavVars["NAV_X"] = CNavVar("NAV_X","X(m)",1.0);
    m_NavVars["NAV_Y"] = CNavVar("NAV_Y","Y(m)",1.0);
    m_NavVars["NAV_Z"] = CNavVar("NAV_Z","Z(m)",1.0);
    m_NavVars["NAV_YAW"] = CNavVar("NAV_YAW","Heading(deg)",-MOOSRad2Deg(1.0));
    m_NavVars["NAV_ALTITUDE"] = CNavVar("NAV_ALTITIUDE","Altitude(m)",1.0);
    m_NavVars["NAV_DEPTH"] = CNavVar("NAV_DEPTH","Depth(m)",1.0);
    m_NavVars["NAV_SPEED"] = CNavVar("NAV_SPEED","Speed(m/s)",1.0);


    //print all the bindings..
    //PrintHelp();

    MOOSPause(1000);
    MOOSTrace("\n******** Welcome to the MOOS **********\n>");

    return true;
}




bool CMOOSRemoteLite::MakeCustomKeys()
{
    STRING_LIST sParams;

    if(m_MissionReader.GetConfiguration(GetAppName(),sParams))
    {
        STRING_LIST::iterator p;

        for(p = sParams.begin();p!=sParams.end();p++)
        {
            string sLine = *p;
            string sTok,sVal;
            m_MissionReader.GetTokenValPair(sLine,sTok,sVal);



            if(MOOSStrCmp(sTok,"CUSTOMKEY"))
            {

                MOOSRemoveChars(sVal," ");
                string sChar = MOOSChomp(sVal,":");
                string sKey = MOOSChomp(sVal,"@");
                MOOSRemoveChars(sVal,"\"");
                string sTx = MOOSChomp(sVal,"$");

                bool  bIsNumeric = MOOSIsNumeric(sTx);						
				bool bAskToConfirm = (sVal == "confirm");

                if(!sChar.empty())
                {

                    CCustomKey NewKey;
                    NewKey.m_cChar = sChar[0];
                    NewKey.m_sKey = sKey;
                    NewKey.m_sVal = sTx;
                    NewKey.bIsNumeric = bIsNumeric;
                    NewKey.bAskToConfirm = bAskToConfirm;

                    if(isdigit(NewKey.m_cChar))
                    {
                        m_CustomKeys[NewKey.m_cChar] = NewKey;
                    }
                    else
                    {
                        MOOSTrace("CMOOSRemote: can only bind custom keys to numeric characters!\n");
                    }
                }

            }
        }
    }
    return true;
}

bool CMOOSRemoteLite::OnConnectToServer()
{
    //subscribe to ALL debug text messages
    m_Comms.Register("MOOS_DEBUG",0);
    m_Comms.Register("NAV_SUMMARY",1.5);

    RegisterMOOSVariables();


    //initially always take control...
    return SetManualOveride(true);
}

bool CMOOSRemoteLite::SetManualOveride(bool bOveride)
{
    m_bManualOveride = bOveride;
    if(m_bManualOveride)
    {

        m_dfCurrentThrust    = 0;
        m_dfCurrentRudder    =0;
        m_dfCurrentElevator    = 0;
        SendDesired();

    //tell the helm to shut up
    m_Comms.Notify("MOOS_MANUAL_OVERIDE","TRUE");
    //wait for a suitable time
        MOOSPause(500);

    //trap case where helm sent a command in the between
    //being told to go off line and reaching this point
        SendDesired();
    }
    else
    {
        MOOSTrace("really relinquish manual control? (y/n)\n");
        char c = MOOSGetch();
        if(c=='y')
        {
            m_Comms.Notify("MOOS_MANUAL_OVERIDE","FALSE");
        }
        else
        {
            MOOSTrace("Cancelled: ");
            m_bManualOveride = true;
        }
    }

    MOOSTrace("Manual Control is %s!\n",m_bManualOveride?"on":"off");


    return true;
}


bool CMOOSRemoteLite::HitInputWD()
{
    m_dfTimeLastAck = MOOSTime();
    return true;
}

bool CMOOSRemoteLite::DoCustomJournal(char cCmd)
{
    CUSTOMJOURNAL_MAP::iterator p;
    for(p = m_CustomJournals.begin();p!=m_CustomJournals.end();p++)
    {
        if(p->second.m_cKey==cCmd)
        {
            MOOSTrace("\nJournal Entries for \"%s\" :\n  ",p->first.c_str());
            STRING_LIST Cpy = p->second.m_Entries;
            std::copy(Cpy.begin(),Cpy.end(),std::ostream_iterator<std::string>(std::cout,"\n  "));
            return true;
        }
    }
    return false;
}

bool CMOOSRemoteLite::DoCustomKey(char cCmd)
{
    //look after custom key bindings
    CUSTOMKEY_MAP::iterator p = m_CustomKeys.find(cCmd);
    bool confirmed = true;

    if(p!=m_CustomKeys.end())
    {
        CCustomKey & rKey = p->second;
        if(rKey.bAskToConfirm)
        {
            MOOSTrace("Press 'y' to confirm command: " + rKey.m_sVal + "\n");
            char cResponse = MOOSGetch();
            confirmed = (cResponse == 'y');
        }
        if(confirmed)
        {
            if(rKey.bIsNumeric)
            {
                m_Comms.Notify(rKey.m_sKey,atof(rKey.m_sVal.c_str()));
                MOOSTrace("CustomKey[%c] : %s {%f}\n",
                rKey.m_cChar,
                rKey.m_sKey.c_str(),
                atof(rKey.m_sVal.c_str()));
            }
            else
            {
            m_Comms.Notify(rKey.m_sKey,rKey.m_sVal);
            MOOSTrace("CustomKey[%c] : %s {%s}\n",
                rKey.m_cChar,
                rKey.m_sKey.c_str(),
                rKey.m_sVal.c_str());
            }
        }
        else
        {
            MOOSTrace("Command Cancelled\n");
        }
        return true;
    }
    else
    {
        return false;
    }
}


bool CMOOSRemoteLite::EnableMailLoop(bool bEnable)
{
    m_bRunMailLoop = bEnable;
    return true;
}

bool CMOOSRemoteLite::DoNavSummary(CMOOSMsg &Msg)
{

    map<string,double> NavVarsValue;
    map<string,bool>   NavVarsValid;

    while(!Msg.m_sVal.empty())
    {
        string sLump = MOOSChomp(Msg.m_sVal,",");

        string sWhat = MOOSChomp(sLump,"=");

        double dfVal = atof(sLump.c_str());

        MOOSChomp(sLump,"@");

        double dfTime = atof(sLump.c_str());
        NAVVAR_MAP::iterator p= m_NavVars.find(sWhat);

        if(p!=m_NavVars.end())
        {
            CNavVar & rVar = p->second;
            rVar.m_dfVal = dfVal;
            rVar.m_dfTime = dfTime;
        }
    }

    return true;
}

bool CMOOSRemoteLite::PrintNavSummary()
{
    STRING_LIST ToPrint;
    ToPrint.push_back("NAV_X");
    ToPrint.push_back("NAV_Y");
    ToPrint.push_back("NAV_Z");
    ToPrint.push_back("NAV_YAW");
    ToPrint.push_back("NAV_ALTITUDE");
    ToPrint.push_back("NAV_DEPTH");
    ToPrint.push_back("NAV_SPEED");



    ostringstream os;
//    ostrstream os;

    os<<"Navigation Summary:"<<endl;


    STRING_LIST::iterator p;

    for(p = ToPrint.begin();p!=ToPrint.end();p++)
    {
        string sWhat = *p;
        NAVVAR_MAP::iterator q = m_NavVars.find(sWhat);

        if(q!=m_NavVars.end())
        {
            CNavVar & rVar = q->second;

            //make left justified
            os.setf(ios::left);
            os<<setw(15)<<rVar.m_sPrintName.c_str()<<"t=";

            os.setf(ios::fixed);

            if(rVar.m_dfTime==-1)
            {
                os<<setw(10)<<"****";
                os<<setw(3)<<" : ";
                os<<setw(10)<<"****";
            }
            else
            {
                os<<setw(10)<<setprecision(1)<<rVar.m_dfTime;
                os<<setw(3)<<" : ";
                os<<setw(10)<<setprecision(1)<<rVar.m_dfVal*rVar.m_dfScaleBy;
            }
            os<<endl;
//            os.unsetf(ios::right);

        }
    }
    os<<ends;


    MOOSTrace("%s",string(os.str()).c_str());

    //  os.rdbuf()->freeze(0);

    return true;
}


bool CMOOSRemoteLite::ReStartAll()
{
    char Confirm;
    MOOSTrace("COMPLETE RESTART...");

    //get control..
    SetManualOveride(true);

    //ask
    MOOSTrace("Restart navigation? (y/n)");
    if((Confirm = MOOSGetch())=='y')
    {
        MOOSTrace("Restarting Navigation...\n");
        m_Comms.Notify("RESTART_NAV","TRUE");
    }

    MOOSTrace("Restart helm? (y/n)");
    if((Confirm = MOOSGetch())=='y')
    {
        MOOSTrace("Restarting Helm...\n");
        m_Comms.Notify("RESTART_HELM","TRUE");
    }

    MOOSTrace("New Log file? (y/n)");
    if((Confirm = MOOSGetch())=='y')
    {
        MOOSTrace("Restarting Logger...\n");
        m_Comms.Notify("LOGGER_RESTART","TRUE");
    }

    MOOSTrace("Wait....\n");
    MOOSPause(2000);


    PrintNavSummary();

    MOOSTrace("*** Check Navigation before launching\n ***");

    return true;
}






bool CMOOSRemoteLite::MakeCustomJournals()
{

    STRING_LIST sParams;
    if(m_MissionReader.GetConfiguration(GetAppName(),sParams))
    {
        STRING_LIST::iterator p;

        for(p = sParams.begin();p!=sParams.end();p++)
        {
            string sLine = *p;
            string sTok,sVal;
            m_MissionReader.GetTokenValPair(sLine,sTok,sVal);

            //CUSTOMJOURNAL = Name = <MOOSNAME>, Key = <key>, History  = Size , Period = <Time>
            if(MOOSStrCmp(sTok,"CUSTOMJOURNAL"))
            {
                std::string sName;
                if(!MOOSValFromString(sName,sVal,"Name"))
                {
                    MOOSTrace("Error in CUSTOMJOURNAL string  -> no  \"Name\" field\n");
                    continue;
                }
                std::string sKey;
                if(!MOOSValFromString(sKey,sVal,"Key"))
                {
                    MOOSTrace("Error in CUSTOMJOURNAL string  -> no  \"Key\" field\n");
                    continue;
                }
                int nSize=10;
                if(!MOOSValFromString(nSize,sVal,"History"))
                {
                }

                double dfT=0;
                if(!MOOSValFromString(dfT,sVal,"Period"))
                {
                }


                char cKey = sKey[0];

                if(sKey.size()!=1 || !isdigit(cKey))
                {
                    MOOSTrace("error CustomJournal \"Key\" must be a character\n");
                    continue;
                }



                MOOSTrace("  Journal Started for %s:\n    bound to \"%c\"\n    history of %d\n    collecting %s \n",
                    sName.c_str(),
                    cKey,
                    nSize,
                    dfT>0 ? MOOSFormat("at %f Hz",1.0/dfT).c_str() : "every notification");

                m_CustomJournals[sName] = CJournal(sName,nSize,cKey);;

                AddMOOSVariable(sName,sName,"",dfT);

            }
        }
    }

    RegisterMOOSVariables();

    return true;
}



bool CMOOSRemoteLite::MakeCustomSummary()
{

    STRING_LIST sParams;
    if(m_MissionReader.GetConfiguration(GetAppName(),sParams))
    {
        STRING_LIST::iterator p;

        for(p = sParams.begin();p!=sParams.end();p++)
        {
            string sLine = *p;
            string sTok,sVal;
            m_MissionReader.GetTokenValPair(sLine,sTok,sVal);

            if(MOOSStrCmp(sTok,"CUSTOMSUMMARY"))
            {
                m_CustomSummaryList.push_front(sVal);

                AddMOOSVariable(sVal,sVal,"",0.2);
            }
        }
    }

    RegisterMOOSVariables();

    return true;
}



bool CMOOSRemoteLite::PrintCustomSummary()
{
    ostringstream os;

    os<<"\n******    User Custom Summary    ******"<<endl;

    STRING_LIST::iterator p;

    for(p = m_CustomSummaryList.begin();p!=m_CustomSummaryList.end();p++)
    {
        string sWhat = *p;

        CMOOSVariable * pVar = GetMOOSVar(sWhat);

        if(pVar==NULL)
            continue;

        //make left justified
        os.setf(ios::left);
        os<<setw(20)<<sWhat.c_str()<<"t=";

        os.setf(ios::fixed);

        if(!pVar->IsFresh())
        {
            os<<setw(10)<<"****";
            os<<setw(3)<<" : ";
            os<<setw(10)<<"****";
        }
        else
        {
            os<<setw(10)<<setprecision(1)<<pVar->GetTime()-GetAppStartTime();
            os<<setw(3)<<" : ";

            if(pVar->IsDouble())
            {
                os<<setw(10)<<setprecision(1)<<pVar->GetDoubleVal();
            }
            else
            {
                os<<setw(10)<<pVar->GetStringVal().c_str();;
            }
        }
        os<<endl;
    }
    os<<ends;

    MOOSTrace("%s",string(os.str()).c_str());

    return true;

}
