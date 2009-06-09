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
// ScopeTabPane.cpp: implementation of the CScopeTabPane class.


//


//////////////////////////////////////////////////////////////////////


#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <FL/Fl_Group.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <string>
#include "ScopeTabPane.h"
#include <FL/Fl_Tabs.H>
#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>

#define FONT_SIZE 11

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CScopeTabPane::~CScopeTabPane()
{
    StopTimer();
}


CScopeTabPane::CScopeTabPane( int X, int Y, int W, int H,  char *l ) :BASE(X,Y,W,H,l)
{
    m_FetchThread.Initialise(FetchWorker,this);
    m_FetchThread.Start();

    label(l);
    m_sHost = "LOCALHOST";
    m_lPort = 9000;



    m_nCounts=0;


    int LHS = X+10;
    int TOP = Y+10;
    int RHS = X+W-10;
    int BOTTOM = Y+H;
    int GRID_H =  (2*H)/3;
    int BOTTOM_GRID = TOP+GRID_H;
    int PROC_W = int(0.22*W);
    int PROC_H = H-BOTTOM_GRID-10;


    m_pScopeGrid = new CScopeGrid( LHS, TOP, W-20,GRID_H, "DB" );
    m_pScopeGrid->SetDBImage(&m_DBImage);
    m_pScopeGrid->SetComms(&m_Comms);


    Fl_Group *pC = new Fl_Group(X,BOTTOM_GRID,W,H-GRID_H);

    {



        //the process list
        m_pProcessList = new Fl_Check_Browser( LHS,BOTTOM_GRID+10,PROC_W,PROC_H,"Processes");
        SetID(m_pProcessList,ID_PROCESS);
        m_pProcessList->tooltip("Click a process name to examine subscriptions and publications");
        m_pProcessList->textsize(FONT_SIZE);
        m_pProcessList->when(FL_WHEN_RELEASE);

        //the subscriber list
        m_pSubscribeList = new Fl_Browser( m_pProcessList->x()+PROC_W+30,BOTTOM_GRID+10,PROC_W,PROC_H,"Subscribes");
        m_pSubscribeList->textsize(FONT_SIZE);

        //the publish list
        m_pPublishList = new Fl_Browser(  m_pSubscribeList->x()+PROC_W+5,BOTTOM_GRID+10,PROC_W,PROC_H,"Publishes");
        m_pPublishList->textsize(FONT_SIZE);


        Fl_Button* pLB = new Fl_Button(m_pPublishList->x()+PROC_W+5,m_pPublishList->y(),15,20,"?");
        pLB->type(FL_TOGGLE_BUTTON);
        pLB->tooltip("show ? (pending) DB entries");
        SetID(pLB,ID_SHOW_PENDING);


        Fl_Group *pMOOSParams = new Fl_Group(RHS-180,BOTTOM_GRID+10,180,100);
        {
            //MOOS parameters - HOST

            m_pDBHostInput = new Fl_Input(    RHS-180,
                BOTTOM_GRID+10,
                100,25,"HostName");


            m_pDBHostInput->align(FL_ALIGN_RIGHT );
            m_pDBHostInput->textsize(FONT_SIZE);        
            m_pDBHostInput->tooltip("name or IP address of machine hosting DB");


            //MOOS parameters - Port
            m_pDBPortInput = new Fl_Int_Input(    m_pDBHostInput->x(),
                m_pDBHostInput->y()+m_pDBHostInput->h()+5,
                100,25,"Port");


            m_pDBPortInput->align(FL_ALIGN_RIGHT );
            m_pDBPortInput->textsize(FONT_SIZE);
            m_pDBPortInput->tooltip("Port Number MOOSDB is listening on");



            m_pConnectButton= new Fl_Button( m_pDBPortInput->x(),
                m_pDBPortInput->y()+m_pDBPortInput->h()+5,
                160,25,"Connect");


            SetID(m_pConnectButton,ID_CONNECT);
            m_pConnectButton->tooltip("Connect to a MOOSDB");

            //set up MOOS values
            m_pDBHostInput->value(m_sHost.c_str());
            m_pDBPortInput->value(MOOSFormat("%ld",m_lPort).c_str());

        }

        pMOOSParams->resizable(0);
    }


    pC->resizable(this);
    end();

    //make things initially grey...
    m_pScopeGrid->deactivate();
    m_pSubscribeList->deactivate();
    m_pPublishList->deactivate();
    m_pProcessList->deactivate();
    StartTimer(0.5);

};

void  CScopeTabPane::SetMask()
{
    std::map<std::string,ProcessOptions>::iterator p;
    std::set<std::string> Mask;
    for(p = m_ProcessOptions.begin();p!=m_ProcessOptions.end();p++)
    {
        if(!p->second.m_bShow)
            Mask.insert(p->first);
    }

    m_DBImage.Clear();
    m_DBImage.SetMask(Mask);
}


std::string CScopeTabPane::GetFocusProcess()
{
    const char * pStr = m_pProcessList->text(m_pProcessList->value());
    if(pStr==NULL)
        return "";
    return std::string(pStr);
}


void CScopeTabPane::OnControlWidget(Fl_Widget* pWidget,int ID)
{
    //this is the switch yard for all messages
    switch(ID)
    {
    case ID_PROCESS:
        {
            if(Fl::event_shift())
            {
                //toggling visibility masks...
                std::string sFocus = GetFocusProcess();

                if(m_ProcessOptions.find(sFocus)!=m_ProcessOptions.end())
                {
                    ProcessOptions & rOptions = m_ProcessOptions[sFocus];
                    rOptions.m_bShow = !rOptions.m_bShow;
                }

                m_pProcessList->checked(m_pProcessList->value(),m_ProcessOptions[GetFocusProcess()].m_bShow);

                SetMask();

            }
            else 
            {
                //changing what appear isn teh subscribed/published box
                std::string sFocus= GetFocusProcess();                

                STRING_LIST sSubs,sPubs;
                if(m_DBImage.GetProcInfo(sFocus,sSubs,sPubs))
                {
                    m_pSubscribeList->clear();
                    m_pPublishList->clear();
                    STRING_LIST::iterator p;
                    for(p = sSubs.begin();p!=sSubs.end();p++)
                    {
                        m_pSubscribeList->add(p->c_str());
                    }
                    for(p = sPubs.begin();p!=sPubs.end();p++)
                    {
                        m_pPublishList->add(p->c_str());
                    }
                    static char sPubTxt[1024];
                    sprintf(sPubTxt,"%s Publishes",sFocus.c_str());
                    static char sSubTxt[1024];
                    sprintf(sSubTxt,"%s Subscribes",sFocus.c_str());

                }
                m_pProcessList->checked(m_pProcessList->value(),m_ProcessOptions[GetFocusProcess()].m_bShow);
            }
        }

        break;
    case ID_CONNECT:
        {
            std::string sMOOSName = "uMS["+m_Comms.GetLocalIPAddress()+"]";
            m_sHost = std::string(m_pDBHostInput->value());
            m_lPort = atoi(m_pDBPortInput->value());
            //set up callbacks
            m_Comms.SetOnConnectCallBack(MOOSConnectCallback,this);
            m_Comms.SetOnDisconnectCallBack(MOOSDisconnectCallback,this);

            //go!
            if(!m_Comms.Run(m_sHost.c_str(),m_lPort,sMOOSName.c_str()))
            {
            }
        }
        break;

    case ID_SHOW_PENDING:
        {
            Fl_Button* pB = (Fl_Button*)GetByID(ID_SHOW_PENDING);
            m_DBImage.Clear();
            SetMask();
            m_DBImage.ShowPending(pB->value()!=0);
        }
        break;

    }

}


bool CScopeTabPane::MOOSConnectCallback(void * pParam)
{
    if(pParam)
    {
        return ((CScopeTabPane*)pParam)->OnMOOSConnect();
    }
    return false;
}

bool CScopeTabPane::MOOSDisconnectCallback(void * pParam)
{
    if(pParam)
    {
        return ((CScopeTabPane*)pParam)->OnMOOSDisconnect();
    }
    return false;
}


bool CScopeTabPane::OnMOOSConnect()
{
    m_pScopeGrid->activate();
    m_pProcessList->activate();
    m_pSubscribeList->activate();
    m_pPublishList->activate();

    m_pConnectButton->deactivate();
    m_pDBPortInput->deactivate();
    m_pDBHostInput->deactivate();

    return true;
}


bool CScopeTabPane::OnMOOSDisconnect()
{

    m_pScopeGrid->deactivate();
    m_pProcessList->deactivate();
    m_pSubscribeList->deactivate();

    m_pPublishList->deactivate();

    m_pConnectButton->activate();
    m_pDBPortInput->activate();
    m_pDBHostInput->activate();

    return true;

}

bool CScopeTabPane::FetchLoop()
{
    while(!m_FetchThread.IsQuitRequested())
    {
        if(m_Comms.IsConnected())
        {
            if(m_nCounts++%5==0)
            {
                if(!GetDBProcSummary())
                {                
                }
            }
            if(!GetDBSummary())
            {
            }
        }
        //basic 4Hz Tick - 
        MOOSPause(250);
    }

    return true;

}




void CScopeTabPane::OnTimer()
{
    Fl_Widget* pActive = ((Fl_Tabs*)parent())->value();
    if(pActive==this)
    {
        m_pScopeGrid->redraw();

        //we'll update the process list much slower...
        if(m_nCounts%5==0)
        {
            STRING_LIST sProcs;
            if(m_DBImage.GetProcesses(sProcs))
            {
                const char * pStr = m_pProcessList->text(m_pProcessList->value());

                std::string sSel = pStr==NULL ? "": std::string(pStr);

                m_pProcessList->clear();
                int nSel = -1;
                STRING_LIST::iterator q;
                for(q = sProcs.begin();q!=sProcs.end();q++)
                {            
                    ProcessOptions & rOptions = m_ProcessOptions[q->c_str()];

                    m_pProcessList->add(q->c_str());
                    nSel= m_pProcessList->nitems();

                    m_pProcessList->checked(nSel,rOptions.m_bShow);

                }
            }
            m_pScopeGrid->SetTitle(MOOSFormat("%d Processes %d Variables",sProcs.size(),
                m_DBImage.GetNumVariables()).c_str());
        }
    }
}


bool CScopeTabPane::GetDBSummary()
{
    if(!m_Comms.IsConnected())
    {
        return true;
    }
    else
    {
        MOOSMSG_LIST InMail;
        if(m_Comms.ServerRequest("ALL",InMail))
        {
            return m_DBImage.Set(InMail);
        }
    }
    return false;
}

bool CScopeTabPane::GetDBProcSummary()
{
    MOOSMSG_LIST InMail;
    if(!m_Comms.IsConnected())
    {
        return true;
    }
    else
    {
        if(m_Comms.ServerRequest("PROC_SUMMARY",InMail))
        {
            if(m_DBImage.SetProcInfo(InMail))
            {
            }
        }
    }

    return false;
}


