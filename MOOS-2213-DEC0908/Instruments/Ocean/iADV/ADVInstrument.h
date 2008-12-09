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
// ADVInstrument.h: interface for the CADVInstrument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADVINSTRUMENT_H__8E35F2A9_EE8C_418F_83A7_42E8B4B12736__INCLUDED_)
#define AFX_ADVINSTRUMENT_H__8E35F2A9_EE8C_418F_83A7_42E8B4B12736__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CADVInstrument : public CMOOSInstrument  
{
public:
    CADVInstrument();
    virtual ~CADVInstrument();

protected:
    bool InitialiseSensor();
    bool Iterate();
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool OnConnectToServer();
    bool OnStartUp();
    bool GetData();
    bool PublishData();
   

private:
    bool ParseADVReply(string &sReply);
    bool SendADVUpdatedCommands(string &sCmdUpdateStatus, string sNewCmdString);
    void StartPinging();
    bool NewCommandsAreSkewed(CMOOSMsg & Msg, double dfTimeNow, string sNewCmdString);
    bool GetADVAttention();
    bool ExecuteADVUpdateCommands(CMOOSMsg & Msg);
    bool GoodReply(string sReply);

     struct SendTermPair
    {
        SendTermPair(string sToSend, string sReply){
            sSend = sToSend;
            sReplyTerm = sReply;
        };
        string sSend;
        string sReplyTerm;
    };


    typedef list<SendTermPair> TERMPAIRLIST;
    typedef list<string> ADVINFO_LIST;
};

#endif // !defined(AFX_ADVINSTRUMENT_H__8E35F2A9_EE8C_418F_83A7_42E8B4B12736__INCLUDED_)
