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
// MOOSNavEntity.cpp: implementation of the CMOOSNavEntity class.
//
//////////////////////////////////////////////////////////////////////

#include "MOOSNavSensor.h"
#include "MOOSNavEntity.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavEntity::CMOOSNavEntity()
{
    m_nStart = -1;
    m_nEnd = -1;
}

CMOOSNavEntity::~CMOOSNavEntity()
{

}

CMOOSNavEntity::CState::CState()
{
    m_dfX = 0;
    m_dfY = 0;
    m_dfZ = 0;
    m_dfH = 0;
    m_dfXdot = 0;
    m_dfYdot = 0;
    m_dfZdot = 0;
    m_dfHdot = 0;
    m_dfDepth = 0;

    m_dfPX=100;
    m_dfPY=100;
    m_dfPZ=100;
    m_dfPH=100;
    m_dfPXdot=0;
    m_dfPYdot=0;
    m_dfPZdot=0;
    m_dfPHdot=0;

    m_dfSpeed = 0;


}

bool CMOOSNavEntity::AddSensor(CMOOSNavSensor *pSensor)
{
    pSensor->SetParent(this);

    if(m_SensorMap.find(pSensor->GetName())==m_SensorMap.end())
    {
        m_SensorMap[pSensor->GetName()] = pSensor;
        return true;
    }
    else
    {
        MOOSTrace("Sensor of this name already exists");
    }

    return false;
}

CMOOSNavSensor* CMOOSNavEntity::GetSensorByName(const string &sName)
{
    SENSOR_MAP::iterator p = m_SensorMap.find(sName);

    if(p!=m_SensorMap.end())
    {
        return p->second;
    }
    return NULL;
}

CMOOSNavSensor* CMOOSNavEntity::GetSensorByType(CMOOSNavSensor::Type eType)
{
    SENSOR_MAP::iterator p;
    
    for(p= m_SensorMap.begin(); p!=m_SensorMap.end();p++)
    {
        CMOOSNavSensor* pSensor = p->second;

        if(pSensor->m_eType==eType)
        {
            return pSensor;
        }
    }
    return NULL;
}





bool CMOOSNavEntity::RefreshStateVector()
{
    Matrix XNew;
    
    GetFullState(XNew,NULL,false);

    m_pXhat->SubMatrix(m_nStart,
                    m_nEnd,
                    1,
                    1)=XNew.SubMatrix(1,GetStateSize(),1,1);

    return true;

}

bool CMOOSNavEntity::RefreshStateCovariance()
{
    (*m_pPhat)(m_nStart+iiX,m_nStart+iiX) = m_State.m_dfPX;
    (*m_pPhat)(m_nStart+iiY,m_nStart+iiY) = m_State.m_dfPY;
    (*m_pPhat)(m_nStart+iiZ,m_nStart+iiZ) = m_State.m_dfPZ;
    (*m_pPhat)(m_nStart+iiH,m_nStart+iiH) = m_State.m_dfPH;

    if(GetEntityType()==POSE_AND_RATE)
    {
        (*m_pPhat)(m_nStart+iiXdot,m_nStart+iiXdot) = m_State.m_dfPXdot;
        (*m_pPhat)(m_nStart+iiYdot,m_nStart+iiYdot) = m_State.m_dfPYdot;
        (*m_pPhat)(m_nStart+iiZdot,m_nStart+iiZdot) = m_State.m_dfPZdot;
        (*m_pPhat)(m_nStart+iiHdot,m_nStart+iiHdot) = m_State.m_dfPHdot;
    }

    return true;

}

