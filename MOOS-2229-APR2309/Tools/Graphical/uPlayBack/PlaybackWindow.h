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
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#ifndef PlayBackWindowh
#define PlayBackWindowh
#include "MOOSPlayBackV2.h"


#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FLTKVW/MOOSFLTKUI.h>
#include <FL/Fl_Check_Browser.H>

class CPlaybackWindow : public CMOOSFLTKUI
{
private :
    typedef CMOOSFLTKUI BASE;
    enum ID
    {
        ID_MOOS,
        ID_PLAY,
        ID_STOP,
        ID_REWIND,
        ID_WARP,
        ID_PROGRESS,
        ID_FILE,
        ID_SOURCE,
        ID_CLOCK,
        ID_MOOS_CONFIGURE,

        ID_SEEK,
    };
    enum Mode{PLAYING,STOPPED} m_eMode;

public:

    CPlaybackWindow( int X, int Y, int W, int H,  const char *l );
    void OnControlWidget(Fl_Widget* pWidget,int ID);
    bool OnPlayButton();
    bool OnStopButton();
    bool MakeMessageFilter();
    virtual void OnTimer();
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool OnMOOS();
    bool OnFile();
    bool OnWarp();
    bool ShowProgress();
    bool OnMOOSConnect();
    bool OnMOOSDisconnect();
    bool OnProgress();
    bool OnMOOSConfigure();
    void OnGUIRefresh();
    bool SetTitle(bool bOnline = false);
    bool ManageWidgetStates();

    Fl_Window * GetRootWindow();
//    CMOOSPlayBack m_PlayBack;
    CMOOSPlayBackV2 m_PlayBack;
    std::string m_sFileName;
    Fl_Check_Browser * pSourceCheck;
    CMOOSCommClient m_Comms;
    std::string m_sServerHost;
    long m_lServerPort;
    int m_nTimerHits;
    double m_dfTimerInterval;

    double m_dfSeekTime;

};

#endif
