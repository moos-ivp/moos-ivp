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



#include "MOOS/libMOOS/Comms/CommsTypes.h"

///////////////////////////////////////////////////////////////////////////////////
//Here we define the current protocol string for this version of the library
//if and when the wire protocol changes change the MOOS_PROTOCOL_STRING name
//keeping it below MOOS_PROTOCOL_STRING_BUFFER_SIZE
#define MOOS_PROTOCOL_STRING_BUFFER_SIZE 32
//#define MOOS_PROTOCOL_STRING "ELKS CAN'T DANCE 30/7/10"
#define MOOS_PROTOCOL_STRING "ELKS CAN'T DANCE 2/8/10"
#define MOOS_PKT_DEFAULT_SPACE 32768


/** This class is part of MOOS's internal transport mechanism. It any number of CMOOSMsg's
can be packed into a CMOOSCommPkt and sent in one lump between a CMOOSCommServer and CMOOSCommClient
object. It is never used by a user of MOOSLib */
class CMOOSCommPkt  
{
public:
    CMOOSCommPkt();
    virtual ~CMOOSCommPkt();

    /**
     * serialise to or from a list of CMOOSMsgs
     */
    bool    Serialize(MOOSMSG_LIST & List, bool bToStream = true, bool bNoNULL =false,double * pdfPktTime=NULL);

    /**
     * return length of serialised stream
     */
    int     GetStreamLength();

    bool    OnBytesWritten(unsigned char * PositionWrittento,int nData);

    int     GetBytesRequired();

    int    GetNumMessagesSerialised();

    unsigned char * Stream();

    unsigned char * NextWrite();

protected:
    bool InflateTo(int nNewStreamSize);
    int m_nByteCount;
    int m_nMsgLen;

    unsigned char * m_pStream;
    unsigned char * m_pNextData;
    int             m_nStreamSpace;

	//how many messages are contained in this  packet when serialsised to a stream?
    int m_nToStreamCount;

	//how many messages are serialsied
    int m_nMsgsSerialised;

};

#endif
