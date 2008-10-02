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
//   This file is part of a  MOOS Utility Component. 
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
#if (_MSC_VER == 1200)
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)
#endif

#include <MOOSLIB/MOOSLib.h>

#include <FL/Fl_Button.H>
#include <FL/Fl_Clock.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Check_Browser.H>
#include <FL/Fl_Preferences.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <FLTKVW/FLTKCheckList.h>
#include <string>
#include "PlaybackWindow.h"

#define DEFAULT_COMMS_TICK 40
#define DEFAULT_TIMER_INTERVAL 0.01
#define _USE_32BIT_TIME_T

class CStaticClock : public Fl_Clock
{
private :
    typedef Fl_Clock BASE;
public:    
    CStaticClock( int X, int Y, int W, int H,  char *l=0 ) : BASE(X,Y,W,H,l){};
    int handle(int event) 
    {
        switch (event) 
        {
        case FL_SHOW:            
            return 0;
            break;
        default:
            return BASE::handle(event);
            break;
        }
    }
};

void GUIRefresh(void* pParam)
{
    CPlaybackWindow *pMe = (CPlaybackWindow*)pParam;
    pMe->OnGUIRefresh();
}

CPlaybackWindow::CPlaybackWindow( int X, int Y, int W, int H,  const char *l ) : BASE(X,Y,W,H,l)
{
    m_dfTimerInterval = DEFAULT_TIMER_INTERVAL;

    CStaticClock* pClock = new CStaticClock(20,20,70,70);
    pClock->tooltip("playback time");
    SetID(pClock,ID_CLOCK);
    pClock->labelsize(10);
    pClock->align(FL_ALIGN_TOP);

    Fl_Button* pPlayButton = new Fl_Button( 20,
                                        95,
                                        15,15,"@>");
    pPlayButton->labelcolor(FL_GREEN);
    SetID(pPlayButton,ID_PLAY);
    pPlayButton->tooltip("start playing");

    Fl_Button* pStopButton = new Fl_Button( 37,
                                            95,
                                            15,15,"@-2square");
    pStopButton->labelcolor(FL_RED);
    pStopButton->tooltip("stop playing");
    SetID(pStopButton,ID_STOP);

    Fl_Button* pRewindButton = new Fl_Button(55,
                                            95,
                                            15,15,"@|<");
    pRewindButton->tooltip("rewind to start");
    SetID(pRewindButton,ID_REWIND);


    Fl_Button* pSeekButton = new Fl_Button(73,

                                            95,

                                            15,15,"@-22>");

    pSeekButton->tooltip(" go to seek time");

    SetID(pSeekButton,ID_SEEK);


    Fl_Counter* pCounter = new Fl_Counter(20,120,160,20);
    pCounter->tooltip(" % progress  (adjust to set seek time)");
    SetID(pCounter,ID_PROGRESS);
    pCounter->when(FL_WHEN_RELEASE|FL_WHEN_CHANGED);
    pCounter->range(0.0,100.0);
    pCounter->labelsize(9);
    pCounter->align(FL_ALIGN_BOTTOM| FL_ALIGN_LEFT);
    pCounter->label("no file loaded");


    pSourceCheck = new Fl_Check_Browser(200,20,120,120,"");
    pSourceCheck->tooltip("Select which processes to replay");
    pSourceCheck->align(FL_ALIGN_TOP| FL_ALIGN_LEFT);
      pSourceCheck->labelsize(10);
    pSourceCheck->label("0 KB Loaded");
    SetID(pSourceCheck,ID_SOURCE);


    Fl_Button* pFileButton = new Fl_Button( 100,
                                            20,
                                            80,20,"File...");
    pFileButton->tooltip("Open *.alog file to replay");
    SetID(pFileButton,ID_FILE);
    
    Fl_Button* pMOOSButton = new Fl_Button( 100,
                                            45,
                                            80,20,"MOOS");
    pMOOSButton->tooltip("Connect to MOOS");
    SetID(pMOOSButton,ID_MOOS);
    pMOOSButton->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

    Fl_Button* pConfigureMOOSButton = new Fl_Button( 150,
                                            48,
                                            25,14,"?");
    pConfigureMOOSButton->tooltip("Configure MOOS Connection");
    SetID(pConfigureMOOSButton,ID_MOOS_CONFIGURE);

    Fl_Counter* pWarpCounter = new Fl_Counter(100,80,80,20,"warp");
    pWarpCounter->type(FL_SIMPLE_COUNTER);
    pWarpCounter->align(FL_ALIGN_TOP);
    pWarpCounter->tooltip("alter playback speed");
    SetID(pWarpCounter,ID_WARP);
    pWarpCounter->when(FL_WHEN_RELEASE|FL_WHEN_CHANGED);
    pWarpCounter->range(0.0,10.0);
    pWarpCounter->value(1.0);

    //set initial window states
    GetByID(ID_PLAY)->deactivate();
    GetByID(ID_STOP)->deactivate();
    GetByID(ID_REWIND)->deactivate();
    GetByID(ID_PROGRESS)->deactivate();
    GetByID(ID_WARP)->deactivate();
    GetByID(ID_SOURCE)->deactivate();
    GetByID(ID_CLOCK)->deactivate();

    //here we read last moos connection info from our preferences
    Fl_Preferences app( Fl_Preferences::USER, "MOOS", "uPlayback" );
    char path[ FL_PATH_MAX ];
    app.getUserdataPath( path, sizeof(path) );
    char HostName[FL_PATH_MAX];
    app.get("HostName",HostName,"LOCALHOST",sizeof(HostName));
    int nHost;
    app.get("HostPort",nHost,9000);
    m_lServerPort = (long)nHost;
    m_sServerHost = std::string(HostName);





    m_dfSeekTime = 0.0;


    //here we add a timer to keep thinsg refreshed
    //this fixes a problem with some guio stuff happening outside thread 0
    Fl::add_timeout(0.25,GUIRefresh,this);

    m_eMode = STOPPED;
}

void CPlaybackWindow::OnGUIRefresh()
{    
    ManageWidgetStates();
    SetTitle();
    Fl::redraw();
    Fl::add_timeout(0.25,GUIRefresh,this);
}
void CPlaybackWindow::OnControlWidget(Fl_Widget* pWidget,int ID)
{
    //this is the swicth yard for all messages
    switch(ID)
    {        
    case ID_PLAY:
        OnPlayButton();
        ShowProgress();
        m_eMode = PLAYING;
        break;
    case ID_STOP:
        OnStopButton();
        m_eMode = STOPPED;
        break;
    case ID_MOOS:
        OnMOOS();
        GetByID(ID_MOOS_CONFIGURE)->take_focus();
        break;
    case ID_REWIND:
        StopTimer();
        m_PlayBack.Reset();

        m_eMode = STOPPED;
        ShowProgress();
        break;
    case ID_WARP:
        OnWarp();
        break;
    case ID_FILE:
        OnFile();    
        m_PlayBack.Reset();
        ShowProgress();
        break;

    case ID_SEEK:

        StopTimer();

        m_eMode = STOPPED;

        m_PlayBack.Reset();

        m_PlayBack.GotoTime(m_dfSeekTime);

        ShowProgress();

        break;
    case ID_PROGRESS:
        OnProgress();
        break;
    case ID_MOOS_CONFIGURE:
        OnMOOSConfigure();
        break;
    }

    ManageWidgetStates();

}
bool CPlaybackWindow::OnMOOSConfigure()
{
    //ask the user wher he/she wants to connect to
    const char * scHost=fl_input("MOOSDB Host Name", m_sServerHost.c_str());
    if(scHost==NULL)
        return false;
    m_sServerHost = std::string(scHost);

    std::string sPort = MOOSFormat("%d",(int)m_lServerPort);
    const char * scPort=fl_input("MOOSDB Port", sPort.c_str());
    if(scPort==NULL)
        return false;
    m_lServerPort = atol(scPort);
    SetTitle();
    return true;
}

bool CPlaybackWindow::SetTitle(bool bOnline)
{
    Fl_Window* p = GetRootWindow();

    static char sTitle[1024];
    sprintf(sTitle,"uPlayback : %s:%d %s",m_sServerHost.c_str(),(int)m_lServerPort,m_Comms.IsConnected()|| bOnline ? "Online":"Offline");
    p->label(sTitle);
    p->labelcolor(FL_RED);
    return true;
}

bool CPlaybackWindow::ManageWidgetStates()
{
    if(m_Comms.IsConnected())
    {
        GetByID(ID_MOOS)->deactivate();        
        if(m_PlayBack.IsOpen())
        {
            GetByID(ID_CLOCK)->activate();
            GetByID(ID_REWIND)->activate();
            GetByID(ID_PROGRESS)->activate();
            GetByID(ID_WARP)->activate();
            GetByID(ID_MOOS)->deactivate();
            if(m_eMode == PLAYING)
            {
                GetByID(ID_PLAY)->deactivate();
                GetByID(ID_STOP)->activate();            
            }
            else if(m_eMode == STOPPED)
            {
                GetByID(ID_PLAY)->activate();
                GetByID(ID_STOP)->deactivate();            
            }
        }
        else
        {
            GetByID(ID_CLOCK)->deactivate();
        }
    }
    return true;
}

bool CPlaybackWindow::OnProgress()
{
    if(Fl::event()&FL_RELEASE)
    {
        double dfPC = ((Fl_Counter*)GetByID(ID_PROGRESS))->value();
        double dfDuration = m_PlayBack.GetFinishTime()-m_PlayBack.GetStartTime();
        double dfT = m_PlayBack.GetStartTime()+dfPC/100.0*dfDuration;
        ((Fl_Clock*)GetByID(ID_CLOCK))->value((int)dfT);
        m_dfSeekTime = dfT;
        return m_PlayBack.GotoTime(dfT);
    }
    else
    {
        StopTimer();
    }
    return true;

}
bool CPlaybackWindow::OnWarp()
{
    if(Fl::event()&FL_RELEASE)
    {
        double dfWarp = ((Fl_Counter*)GetByID(ID_WARP))->value();
        m_PlayBack.SetTickInterval(dfWarp*m_dfTimerInterval);
    }
    return true;
}

bool CPlaybackWindow::OnFile()
{
    //here we try to recall the last file we opened
    Fl_Preferences app( Fl_Preferences::USER, "MOOS", "uPlayback" );

    char path[ FL_PATH_MAX ];
    app.getUserdataPath( path, sizeof(path) );

    char LastFile[FL_PATH_MAX];
    app.get("LastFile",LastFile,"*",sizeof(LastFile));

    const char * pFile = fl_file_chooser("Select an alog file", "*.alog", LastFile);
    if(pFile)
    {
        //CWaitCursor Cursor;
        GetRootWindow()->cursor(FL_CURSOR_WAIT );



        GetByID(ID_PROGRESS)->label("LOADING AND PARSING ALOG...PLEASE WAIT");

        redraw();

        Fl::wait();


        if(m_PlayBack.Initialise(pFile))
        {
            m_sFileName = std::string(pFile);
            pSourceCheck->clear();

            STRING_SET Sources = m_PlayBack.GetSources();
            STRING_SET::iterator p;
            int i =0;
            for(p = Sources.begin();p!= Sources.end();p++)
            {
                pSourceCheck->add(p->c_str(),1);
            }

            GetByID(ID_SOURCE)->activate();
            GetByID(ID_CLOCK)->activate();

            //here we save the last file we opened
            app.set( "LastFile",pFile );
            
            GetByID(ID_PROGRESS)->label(pFile);

            //lets tell th user how mucg data has been loaded
            static char sFileSize[128];
            sprintf(sFileSize,"%.2f KB loaded",(double)m_PlayBack.m_ALog.GetSize()/1024);
            GetByID(ID_SOURCE)->label(sFileSize);

        }
        else
        {
            fl_alert("Failed to initialise playback");
        }
        GetRootWindow()->cursor(FL_CURSOR_DEFAULT );

    }    

    take_focus();
    return true;
}

Fl_Window * CPlaybackWindow::GetRootWindow()
{
    Fl_Widget * p=this;
    while(1)
    {
        if(p->parent()==NULL)
            return (Fl_Window *)p;
        else
            p = p->parent();
    }
}

bool CPlaybackWindow::OnMOOS()
{

    if(m_Comms.IsConnected())
    {
        m_Comms.Close();
    }
    
    //next line is pretty crucial...
    m_Comms.FakeSource(true);
    m_Comms.SetOnConnectCallBack(MOOSConnectCallBack,(void*)this);
    m_Comms.SetOnDisconnectCallBack(MOOSDisconnectCallBack,(void*)this);

    //here we try to 
    Fl_Preferences app( Fl_Preferences::USER, "MOOS", "uPlayback" );
    app.set("HostName",m_sServerHost.c_str());
    app.set("HostPort",(int)(m_lServerPort));

    return m_Comms.Run(m_sServerHost.c_str(),m_lServerPort,"uPlayback",DEFAULT_COMMS_TICK);
}

bool CPlaybackWindow::OnMOOSDisconnect()
{
    SetTitle();
    return true;
}


bool CPlaybackWindow::OnMOOSConnect()
{
    SetTitle();
    redraw();
    return true;
}


bool CPlaybackWindow::OnPlayButton()
{
    MakeMessageFilter();
    m_nTimerHits = 0;

    StartTimer(m_dfTimerInterval);

    return true;
}
bool CPlaybackWindow::OnNewMail(MOOSMSG_LIST &NewMail)
{

    CMOOSMsg Msg;
    if (m_Comms.PeekMail(NewMail,"PLAYBACK_CHOKE",Msg))
    {
        double dfLastTimeProcessed = Msg.m_dfVal;
        m_PlayBack.SetLastTimeProcessed(dfLastTimeProcessed);
    }

    return true;
}

bool CPlaybackWindow::OnStopButton() 
{
    StopTimer();
    return true;
}


void CPlaybackWindow::OnTimer()
{
    if (m_Comms.IsConnected())
    {
        MOOSMSG_LIST NewMail;
        if(m_Comms.Fetch(NewMail))
        {
            OnNewMail(NewMail);
        }
    }

    MOOSMSG_LIST Output;

    if(m_PlayBack.Iterate(Output))
    {
        MOOSMSG_LIST::iterator p;

        for(p = Output.begin();p!=Output.end();p++)
        {
            CMOOSMsg & rMsg = *p;

            if(m_Comms.IsConnected())
            {
                m_Comms.Post(rMsg);
            }
        }
    }
    else
    {
        if(m_PlayBack.IsEOF())
        {
            OnStopButton();
        }
    }

    if((m_nTimerHits++)%25==0)
    {
        ShowProgress();
    }
    
}

bool CPlaybackWindow::ShowProgress()
{
    //show percent elapsed
    double dfProgress =0;
    double dfDuration = m_PlayBack.GetFinishTime()-m_PlayBack.GetStartTime();
    double dfElapsed = m_PlayBack.GetLastMessageTime()-m_PlayBack.GetStartTime();

    ((Fl_Clock*)GetByID(ID_CLOCK))->value((int)m_PlayBack.GetLastMessageTime());
    if(1 || dfElapsed>=0)
    {
        dfProgress = 100.0*(dfElapsed)/(dfDuration);
        ((Fl_Counter*)GetByID(ID_PROGRESS))->value(dfProgress);
    }
    Fl_Clock* pClock = (Fl_Clock*)GetByID(ID_CLOCK);

    static char Time[256];
    sprintf(Time,"%.2d:%.2d.%.2d",pClock->hour(),pClock->minute(),pClock->second());
    pClock->label(Time);

    
    return true;
}


bool CPlaybackWindow::MakeMessageFilter()
{
    m_PlayBack.ClearFilter();

    for(int i = 0;i<=pSourceCheck->nitems();i++)
    {
        char * pText = pSourceCheck->text(i);
        if(pText!=NULL)
        {
            std::string sSrc(pText);
            bool bWanted = pSourceCheck->checked(i)?true:false;
            m_PlayBack.Filter(sSrc,bWanted);
        }
    }

    return true;
}


