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
// AVTRAKDriver.h: interface for the CAVTRAKDriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVTRAKDRIVER_H__1685CFA1_0248_4594_9808_E80CC9F53C5E__INCLUDED_)
#define AFX_AVTRAKDRIVER_H__1685CFA1_0248_4594_9808_E80CC9F53C5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define MAX_AVTRAK_CHANNELS 14
#define AVTRAK_RANGING_TX_DELAY 0.0257

class CMOOSSerialPort;
#ifdef _WIN32
    class CMOOSNTSerialPort;
#else
    class CMOOSLinuxSerialPort;
#endif


#include <vector>
#include <list>
#include <string>
using namespace std;

#include "TwoWayTOF.h"
typedef vector<CTwoWayTOF > TOF_VECTOR;
typedef vector<int > INT_VECTOR;





class CAVTRAKDriver 
{
public:
    bool GetTOFByChannel(int nChan,string & sName,double & dfTime,double & dfTOF);
    bool SetAcousticTimeOut(double dfTimeOut);
    bool GetTOFString(string & sResult);
    bool SetSerialPort(CMOOSSerialPort * pPort);
    bool Reset();
    bool SetTransceiverMode();
    bool GetRanges();
    CAVTRAKDriver();
    virtual ~CAVTRAKDriver();

    virtual bool SetRxChannel(INT_VECTOR Channels);
    


protected:
    double GetTAT(int nChan);
    bool SetRangingParams();

    TOF_VECTOR      m_TOFs;

    double          m_dfTxTime;
    double          m_dfAcousticTimeOut;

    INT_VECTOR m_RxOrder;

    /** A sensor port */
    #ifdef _WIN32
        CMOOSNTSerialPort * m_pPort;
    #else
        CMOOSLinuxSerialPort * m_pPort;
    #endif


};

#endif // !defined(AFX_AVTRAKDRIVER_H__1685CFA1_0248_4594_9808_E80CC9F53C5E__INCLUDED_)
