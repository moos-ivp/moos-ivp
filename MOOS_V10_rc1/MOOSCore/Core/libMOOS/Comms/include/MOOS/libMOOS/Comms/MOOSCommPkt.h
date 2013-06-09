///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
// MOOSCommPkt.h: interface for the MOOSCommPkt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(MOOSCOMMPKTH)
#define MOOSCOMMPKTH

#include <list>
//using namespace std;

#include "MOOSMsg.h"

#define MOOS_PKT_DEFAULT_SPACE 32768



///////////////////////////////////////////////////////////////////////////////////
//Here we define the current protocol string for this version of the library
//if and when the wire protocol changes change the MOOS_PROTOCOL_STRING name
//keeping it below MOOS_PROTOCOL_STRING_BUFFER_SIZE
#define MOOS_PROTOCOL_STRING_BUFFER_SIZE 32
//#define MOOS_PROTOCOL_STRING "ELKS CAN'T DANCE 30/7/10"
#define MOOS_PROTOCOL_STRING "ELKS CAN'T DANCE 2/8/10"



typedef std::list<CMOOSMsg> MOOSMSG_LIST;

/** This class is part of MOOS's internal transport mechanism. It any number of CMOOSMsg's
can be packed into a CMOOSCommPkt and sent in one lump between a CMOOSCommServer and CMOOSCommClient
object. It is never used by a user of MOOSLib */
class CMOOSCommPkt  
{
public:
    bool    Serialize(MOOSMSG_LIST & List, bool bToStream = true, bool bNoNULL =false,double * pdfPktTime=NULL);
    int     GetStreamLength();
    bool    Fill(unsigned char * InData,int nData);
    int     GetBytesRequired();
	double	GetCompression();

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
	double m_dfCompression;

};

typedef std::list<CMOOSCommPkt> MOOSPKT_LIST;

#endif // !defined(AFX_MOOSCOMMPKT_H__2645E53D_479F_4F8D_9020_B5C8DBCF4789__INCLUDED_)
