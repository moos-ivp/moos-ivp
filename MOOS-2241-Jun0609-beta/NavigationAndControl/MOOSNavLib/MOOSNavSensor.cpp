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
//   This file is part of a  MOOS Basic (Common) Application. 
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
// MOOSNavSensor.cpp: implementation of the CMOOSNavSensor class.
//
//////////////////////////////////////////////////////////////////////
#include <math.h>
#include "MOOSNavSensor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavSensor::CMOOSNavSensor()
{
    m_Offset.m_dfX = 0;
    m_Offset.m_dfY = 0;
    m_Offset.m_dfZ = 0;
    m_dfNoise = -1;

    m_pParent = NULL;
}

CMOOSNavSensor::~CMOOSNavSensor()
{

}

/** this function calculates the offsets of the sensor in a coordinates frame
that is aligned with the gloabl one. So if the Vehicle is pointing at dfAng
we rotate the sensor backwards through -dfAng and relove into the 
cardinal axes */
bool CMOOSNavSensor::GetAlignedOffsets(double dfAng, double &dfX, double &dfY, double &dfZ)
{

    dfX = m_Offset.m_dfX* cos(dfAng) - m_Offset.m_dfY*sin(dfAng);
    dfY = m_Offset.m_dfX* sin(dfAng) + m_Offset.m_dfY*cos(dfAng);
    dfZ = m_Offset.m_dfZ;

    return true;
}

bool CMOOSNavSensor::SetNoise(double dfNoise)
{
    m_dfNoise = dfNoise;
    return true;
}

double CMOOSNavSensor::GetNoise()
{
    return m_dfNoise;
}
