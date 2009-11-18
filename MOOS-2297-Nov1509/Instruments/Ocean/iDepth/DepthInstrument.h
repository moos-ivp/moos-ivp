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
//   This file is part of a  MOOS Instrument. 
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
// DepthInstrument.h: interface for the CDepthInstrument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPTHINSTRUMENT_H__729E6929_5059_4FA5_AA51_0210B63B1FDB__INCLUDED_)
#define AFX_DEPTHINSTRUMENT_H__729E6929_5059_4FA5_AA51_0210B63B1FDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CMOOSDepthSensor;

/** The main depth instrument class. Derived from CMOOSApp it handles all data from
the sensor interface and sends it to the server */
class CDepthInstrument : public CMOOSInstrument
{
public:
    CDepthInstrument();
    virtual ~CDepthInstrument();

protected:
    bool Filter(double dfDepth);


    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();

    bool InitialiseSensor();
    bool PublishDepth();
    bool GetDepth();

    typedef std::list<double> DEPTH_HISTORY;
    DEPTH_HISTORY m_DepthHistory;
    unsigned int m_nHistoryLength;

    bool   m_bFilter;
   
    


    CMOOSDepthSensor * m_pDepthSensor;
};

#endif // !defined(AFX_DEPTHINSTRUMENT_H__729E6929_5059_4FA5_AA51_0210B63B1FDB__INCLUDED_)
