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
// BluefinBatteryDriverV2.h: interface for the CBluefinBatteryDriverV2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLUEFINBATTERYDRIVERV2_H__1CA91A09_950F_4DF3_9192_698F350F2145__INCLUDED_)
#define AFX_BLUEFINBATTERYDRIVERV2_H__1CA91A09_950F_4DF3_9192_698F350F2145__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
using namespace std;

#include "BatteryDriver.h"
#define CELLS_PER_PACK 8
class CBluefinBatteryDriverV2 : public CBatteryDriver  
{
public:

    enum Status
    {
        OFF,
        ON,
        CHARGING,
        BALANCING,
    };


    class CBatteryPack
    {
    public:
        class CCell
        {
        public:
            double m_dfVoltage;
        };

        CCell m_Cells[CELLS_PER_PACK];

        int m_nPackAddress;
        double m_dfVoltage;
        Status m_eState;   
        string m_sState;        
        string m_sError;
        string m_sComment;
        bool  m_bIgnore;

    };
    typedef map<int,CBatteryPack> BATTERY_MAP;

public:

    CBluefinBatteryDriverV2();
    virtual ~CBluefinBatteryDriverV2();

       virtual string GetCellsString();
    virtual string GetErrorString();
    virtual string GetCommentString();

    virtual bool IsError();
    bool  Switch(bool bOn);
protected:
    string GetStateAsString();
    bool QueryCellsState(CBatteryPack & rPack);
    bool ProcessStateString(CBatteryPack & rPack,string sState);
    bool SwitchPack(CBatteryPack & rPack, bool bOn);
    bool QueryBattery(CBatteryPack & rPack);
    bool WriteAndRead(string sOut, string & sReply,double dfTimeOut=-1.0,bool bNoTerm = false);
    bool DiscoverAndMakeBatteries();
    virtual bool    Initialise();
    virtual bool    GetData();
    BATTERY_MAP     m_BatteryMap;
    bool            m_bEchoing;

};

#endif // !defined(AFX_BLUEFINBATTERYDRIVERV2_H__1CA91A09_950F_4DF3_9192_698F350F2145__INCLUDED_)
