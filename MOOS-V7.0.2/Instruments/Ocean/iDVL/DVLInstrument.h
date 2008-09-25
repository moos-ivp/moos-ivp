///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by the Seagrant AUV lab (R.Damus et.al)
//   and Paul Newman at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk. rdamus@mit.edu
//      
//   This file is part of a  MOOS Instrument
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
// DVLInstrument.h: interface for the CDVLInstrument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DVLINSTRUMENT_H__A9401921_A372_4864_959E_C1883F24F90B__INCLUDED_)
#define AFX_DVLINSTRUMENT_H__A9401921_A372_4864_959E_C1883F24F90B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum MOOSDVLSensorType
{
    MOOS_DVL_SENSOR_RDI,
    MOOS_DVL_EXP_DEBUG,
};


class CDVLInstrument : public CMOOSInstrument  
{
public:
    
    CDVLInstrument();
    virtual ~CDVLInstrument();

protected:

    bool ParseRDIReply(std::string & sReply);
    bool InitialiseSensor();
    bool Iterate();
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool OnConnectToServer();
    bool OnStartUp();
    bool GetData();
    bool PublishData();

    
    MOOSDVLSensorType m_eType;
    double m_dfX;
    double m_dfY;
    double m_dfZ;
    double m_dfPitch;
    double m_dfYaw;
    double m_dfSpeed;
    double m_dfHeading;

    
private:
    bool SendDVLUpdatedCommands(std::string &sCmdUpdateStatus, std::string sNewCmdString);
    void StartPinging();
    bool NewCommandsAreSkewed(CMOOSMsg & Msg, double dfTimeNow, std::string sNewCmdString);
    bool GetDVLAttention();
    bool ExecuteDVLUpdateCommands(CMOOSMsg & Msg);
    bool GoodReply(std::string sReply);

    struct SendTermPair
    {
        SendTermPair(std::string sToSend, std::string sReply){
            sSend = sToSend;
            sReplyTerm = sReply;
        };
        std::string sSend;
        std::string sReplyTerm;
    };


    typedef std::list<SendTermPair> TERMPAIRLIST;

    double m_dfAlignment;
    double m_dfLastSummary;
    double m_bExposeVelocities;
};

#endif // !defined(AFX_DVLINSTRUMENT_H__A9401921_A372_4864_959E_C1883F24F90B__INCLUDED_)
