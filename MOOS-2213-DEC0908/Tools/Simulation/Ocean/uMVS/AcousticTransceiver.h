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


// AcousticTransceiver.h: interface for the CAcousticTransceiver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACOUSTICTRANSCEIVER_H__0DE9C296_01AE_460B_9163_3B88F35EB99F__INCLUDED_)
#define AFX_ACOUSTICTRANSCEIVER_H__0DE9C296_01AE_460B_9163_3B88F35EB99F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
using namespace std;

#include "AcousticNode.h"

class CAcousticTransceiver : public CAcousticNode  
{
public:


    bool            Iterate(double dfTimeNow);
    bool            IsReceiving(double dfTimeNow);
    bool            Ping(double dfTimeNow);
                    CAcousticTransceiver();
    virtual         ~CAcousticTransceiver();
    virtual bool    OnAcousticHit(CAcousticSignal & Signal,double dfTime);
    double          m_dfRxWindow;
    double          m_dfLastPingTime;


protected:
    
    //local class
    class CPingReply
    {
    public:
        AcousticChannel m_eChannel;
        double            m_dfTOF;
        double            m_dfRxTime;
           string          m_sResponder;
    };
    typedef list<CPingReply> PING_REPLY_LIST;


    PING_REPLY_LIST m_Replies;
    bool            m_bIsReceiving;
    bool            LogReply(CPingReply & rReply);
};

#endif // !defined(AFX_ACOUSTICTRANSCEIVER_H__0DE9C296_01AE_460B_9163_3B88F35EB99F__INCLUDED_)
