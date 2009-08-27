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
// RelayBoard.h: interface for the CRelayBoard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RELAYBOARD_H__08DA8EE7_D800_4E09_96E6_474109ECA588__INCLUDED_)
#define AFX_RELAYBOARD_H__08DA8EE7_D800_4E09_96E6_474109ECA588__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef std::map<std::string, int> RELAY_FUNCTION_2_CHECK_MAP;
typedef std::map<std::string, int> RELAY_FUNCTION_2_TOGGLE_MAP;
typedef std::map<std::string, int> INPUT_NAME_2_INT_MAP;

enum RelayToggleName
{
    RELAY0_TOGGLE = 1,
    RELAY1_TOGGLE = RELAY0_TOGGLE << 1,
    RELAY2_TOGGLE = RELAY0_TOGGLE << 2,
    RELAY3_TOGGLE = RELAY0_TOGGLE << 3,
    RELAY4_TOGGLE = RELAY0_TOGGLE << 4,
    RELAY5_TOGGLE = RELAY0_TOGGLE << 5,
    RELAY6_TOGGLE = RELAY0_TOGGLE << 6,
    RELAY7_TOGGLE = RELAY0_TOGGLE << 7,
};

enum RelayCheckName
{
    RELAY0,
    RELAY1,
    RELAY2,
    RELAY3,
    RELAY4,
    RELAY5,
    RELAY6,
    RELAY7,
};


enum InputName
{
    INPUT0,
    INPUT1,
    INPUT2,
    INPUT3,
};

enum RelayState
{
    LO,
    HI,
};


class CRelayBoard : public CMOOSInstrument
{
public:


    
    RELAY_FUNCTION_2_TOGGLE_MAP  m_RelayToggleMap;
     RELAY_FUNCTION_2_CHECK_MAP   m_RelayCheckMap;
    INPUT_NAME_2_INT_MAP         m_InputNameMap;
           
    std::string m_sRECORDING;
    std::string m_sRECORD;
    std::string m_sSTOPPED; 
    std::string m_sSTOP; 
    std::string m_sON; 
    std::string m_sOFF;
    std::string m_sCAMERA_POWER;
    std::string m_sBOARD_POWER;
    std::string m_sVCR_POWER;
    std::string m_sLIGHT_POWER;
    std::string m_sLIGHT;
    std::string m_sDARK;

    bool UpdateRelay(CMOOSMsg &Msg);
    bool Stop();
    bool Record();
    bool IsPowered(std::string sRelay);

    CRelayBoard();
    virtual ~CRelayBoard();

    protected:
        bool ShowRelayBoardStatus();
        int CheckAllRelayStates();
        bool ToggleRelaySwitch(std::string sRelay, std::string sDesiredState);
        bool InitialiseSensor();
        bool Iterate();
        bool OnNewMail(MOOSMSG_LIST &NewMail);
        bool OnConnectToServer();
        bool OnStartUp();
        bool GetData();
        bool PublishData();
        bool SetRelayHi(short val);
           bool SetRelayLo(short val);
        int CheckRelayState(short val);
        int CheckInputState(short val);

};

#endif // !defined(AFX_RELAYBOARD_H__08DA8EE7_D800_4E09_96E6_474109ECA588__INCLUDED_)
