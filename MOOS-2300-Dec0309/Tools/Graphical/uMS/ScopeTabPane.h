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
// ScopeTabPane.h: interface for the CScopeTabPane class.

//

//////////////////////////////////////////////////////////////////////

#ifdef _WIN32

    #pragma warning(disable : 4786)

#endif



#if !defined(AFX_SCOPETABPANE_H__8ED1A25B_2FF0_4D44_A7DB_D9DF331642BA__INCLUDED_)

#define AFX_SCOPETABPANE_H__8ED1A25B_2FF0_4D44_A7DB_D9DF331642BA__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Check_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FLTKVW/MOOSFLTKUI.h>
#include "ScopeGrid.h"
#include "DBImage.h"
#include <vector>
#include <map>
#include <MOOSLIB/MOOSLib.h>
#include <MOOSUtilityLib/MOOSThread.h>




class CScopeTabPane : public CMOOSFLTKUI  

{

public:
    CScopeTabPane( int X, int Y, int W, int H,  char *l=0 );
    virtual ~CScopeTabPane();
    static bool MOOSDisconnectCallback(void * pParam);
    static bool MOOSConnectCallback(void * pParam);
    bool OnMOOSConnect();
    bool OnMOOSDisconnect();
    void GetMOOSInfo(std::string & sHost,int &nPort)
    {
        nPort = (int)m_lPort;
        sHost = m_sHost;
    }
    void SetMOOSInfo(const std::string & sHost,int nPort)
    {
        m_lPort = (long)nPort;
        m_sHost = sHost;
        m_pDBHostInput->value(sHost.c_str());
        m_pDBPortInput->value(MOOSFormat("%d",nPort).c_str());

    }
    
    static bool FetchWorker(void* pParam)
    {
        CScopeTabPane* pMe = (CScopeTabPane*)pParam;
        return pMe->FetchLoop();
    }



private:

    typedef CMOOSFLTKUI BASE;
    enum UI_IDS
    {
        ID_PROCESS,
        ID_CONNECT,
        ID_SHOW_PENDING,
    };

    Fl_Button *m_pConnectButton;
    Fl_Input *m_pDBHostInput;
    Fl_Int_Input *m_pDBPortInput;
    CScopeGrid * m_pScopeGrid;
    Fl_Check_Browser *m_pProcessList;
    //Fl_Hold_Browser *m_pProcessList;
    Fl_Browser *m_pSubscribeList;
    Fl_Browser *m_pPublishList;



protected:


    class ProcessOptions
    {
    public:
        bool m_bShow;
        bool m_bUpdated;
        ProcessOptions(){m_bShow = true;m_bUpdated = true;};
    };

    std::map<std::string,ProcessOptions>  m_ProcessOptions;
    
    virtual void OnControlWidget(Fl_Widget* pWidget,int ID);
    void OnTimer();
    bool GetDBSummary();
    bool GetDBProcSummary();

    std::string m_sHost;
    long m_lPort;
    CMOOSCommClient m_Comms;
    CDBImage m_DBImage;

    bool FetchLoop();
    CMOOSThread m_FetchThread;

    std::string GetFocusProcess();
    void SetMask();

    int m_nCounts;
};



#endif // !defined(AFX_SCOPETABPANE_H__8ED1A25B_2FF0_4D44_A7DB_D9DF331642BA__INCLUDED_)

