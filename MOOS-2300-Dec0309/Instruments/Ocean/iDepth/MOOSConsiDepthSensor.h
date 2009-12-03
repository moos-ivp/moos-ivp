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
// MOOSConsiDepthSensor.h: interface for the CMOOSConsiDepthSensor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSCONSIDEPTHSENSOR_H__3A2CA8C4_55AF_4C14_8229_CEEE556EEA4D__INCLUDED_)
#define AFX_MOOSCONSIDEPTHSENSOR_H__3A2CA8C4_55AF_4C14_8229_CEEE556EEA4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSDepthSensor.h"

#include <string>
using namespace std;

class CMOOSConsiDepthSensor : public CMOOSDepthSensor  
{
public:
    virtual bool Zero();
    virtual bool Initialise();
    virtual bool GetDepth();

    CMOOSConsiDepthSensor();
    virtual ~CMOOSConsiDepthSensor();

protected:
    virtual bool ParseDepthString(string sStr);

};

#endif // !defined(AFX_MOOSCONSIDEPTHSENSOR_H__3A2CA8C4_55AF_4C14_8229_CEEE556EEA4D__INCLUDED_)
