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
//   This file is part of a  MOOS Core Component. 
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
// MOOSCommObject.h: interface for the CMOOSCommObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSCOMMOBJECT_H__88A30007_9205_4FDA_B938_915FBE43027D__INCLUDED_)
#define AFX_MOOSCOMMOBJECT_H__88A30007_9205_4FDA_B938_915FBE43027D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MOOSCommPkt.h"

class XPCTcpSocket;

/** A base class for the CMOOSCommServer and CMOOSCommClient objects. This 
class provides basic Receive and Transmit capabilities of CMOOSMsg's and CMOOSCommPkts.
Where messages are passed as parameters then there are transparently packed into 
packets.*/
class CMOOSCommObject  
{
public:
    CMOOSCommObject();
    virtual ~CMOOSCommObject();

protected:
    bool SendPkt(XPCTcpSocket* pSocket,CMOOSCommPkt & PktTx);
    bool ReadPkt(XPCTcpSocket* pSocket,CMOOSCommPkt & PktRx,int nSecondsTimeOut = -1);
       bool SendMsg(XPCTcpSocket* pSocket,CMOOSMsg & Msg);
       bool ReadMsg(XPCTcpSocket* pSocket,CMOOSMsg & Msg, int nSecondsTimeOut = -1);

    /// called to intialise system socket services. Only does something useful in Win32 land
public:
    static bool SocketsInit();



};

#endif // !defined(AFX_MOOSCOMMOBJECT_H__88A30007_9205_4FDA_B938_915FBE43027D__INCLUDED_)
