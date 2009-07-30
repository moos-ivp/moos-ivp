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
// GPSInstrument.h: interface for the CGPSInstrument class.
//
//////////////////////////////////////////////////////////////////////



#if !defined(AFX_GPSINSTRUMENT_H__D59AB67F_0212_45A1_A108_219C70F687A9__INCLUDED_)
#define AFX_GPSINSTRUMENT_H__D59AB67F_0212_45A1_A108_219C70F687A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MOOSUtilityLib/MOOSGeodesy.h>
class CGPSInstrument : public CMOOSInstrument  
{
protected:
    
    struct CGPSData
    {
        bool   bGood;
        double dfLat_deg;  // Decimal degrees
        double dfLong_deg; // Decimal degrees
        double dfHDOP;
        int    nSatellites;

        CGPSData()
        {
            bGood       = false;
            dfLat_deg   = 0;
            dfLong_deg  = 0;
            dfHDOP      = 0;
            nSatellites = 0;
        }
    };

public:
    CGPSInstrument();
    virtual ~CGPSInstrument();

protected:
    CMOOSGeodesy m_Geodesy;
    bool ParseNMEAString(const std::string & sNMEAString, CGPSData & out_data);
    bool InitialiseSensor();
    bool Iterate();
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool OnConnectToServer();
    bool OnStartUp();    
    bool GetData();
    bool PublishData();
    std::string m_sType;
    bool m_bCombineMessages;

};

#endif // !defined(AFX_GPSINSTRUMENT_H__D59AB67F_0212_45A1_A108_219C70F687A9__INCLUDED_)
