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
// MOOSCommPkt.h: interface for the MOOSCommPkt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSCOMMPKT_H__2645E53D_479F_4F8D_9020_B5C8DBCF4789__INCLUDED_)
#define AFX_MOOSCOMMPKT_H__2645E53D_479F_4F8D_9020_B5C8DBCF4789__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
//using namespace std;

#include "MOOSMsg.h"

#define MOOS_PKT_DEFAULT_SPACE 4096

typedef std::list<CMOOSMsg> MOOSMSG_LIST;

/** This class is part of MOOS's internal transport mechanism. It any number of CMOOSMsg's
can be packed into a CMOOSCommPkt and sent in one lump between a CMOOSCommServer and CMOOSCommClient
object. It is never used by a user of MOOSLib */
class CMOOSCommPkt  
{
public:
    bool    Serialize(MOOSMSG_LIST & List, bool bToStream = true, bool bNoNULL =false,double * pdfPktTime=NULL);
    int        GetStreamLength();
    bool    Fill(unsigned char * InData,int nData);
    int        GetBytesRequired();

    CMOOSCommPkt();
    virtual ~CMOOSCommPkt();

    unsigned char * m_pStream;
    unsigned char * m_pNextData;
    int                m_nStreamSpace;
    unsigned char    DefaultStream[MOOS_PKT_DEFAULT_SPACE];

protected:
    bool InflateTo(int nNewStreamSize);
    bool CopyToStream(unsigned char * pData, int nBytes);
    int m_nByteCount;
    int m_nMsgLen;
    /**true is the packet has been infated to increase capicity and m_pStream no longer
    points to DefaultStream but to heap space allocated with new */
    bool    m_bAllocated;

};

typedef std::list<CMOOSCommPkt> MOOSPKT_LIST;

#endif // !defined(AFX_MOOSCOMMPKT_H__2645E53D_479F_4F8D_9020_B5C8DBCF4789__INCLUDED_)
