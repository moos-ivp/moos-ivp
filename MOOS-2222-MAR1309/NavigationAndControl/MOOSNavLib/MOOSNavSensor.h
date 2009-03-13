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
// MOOSNavSensor.h: interface for the CMOOSNavSensor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVSENSOR_H__183C9839_C558_4AC4_BC3D_B622EAA415DA__INCLUDED_)
#define AFX_MOOSNAVSENSOR_H__183C9839_C558_4AC4_BC3D_B622EAA415DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "newmat.h"
using namespace NEWMAT;

#include "MOOSNavBase.h"

class CMOOSNavEntity;

class CMOOSNavSensor  : public CMOOSNavBase
{
public:
    double GetNoise();
    bool SetNoise(double dfNoise);

    enum Type
    {
        INVALID = -1,
        FIXED, //a default sensor used for fixed observations...
        XY,
        LBL,
        ORIENTATION,
        DEPTH,
        ALTITUDE,
        BODY_VEL,
        CONTROL,
    };

    bool GetAlignedOffsets(double dfAng,double &dfX, double &dfY,double &dfZ);
    CMOOSNavSensor();
    virtual ~CMOOSNavSensor();

    void SetParent(CMOOSNavEntity * pParent){m_pParent = pParent;};

    CMOOSNavEntity* GetParent(){return m_pParent;};

    Type m_eType;

    class COffset
    {
    public:
        double m_dfX;
        double m_dfY;
        double m_dfZ;
    };
    COffset m_Offset;

    string m_sMOOSSource;

protected:
    CMOOSNavEntity * m_pParent;
    double m_dfNoise;

};
typedef map<string,CMOOSNavSensor*> SENSOR_MAP;

#endif // !defined(AFX_MOOSNAVSENSOR_H__183C9839_C558_4AC4_BC3D_B622EAA415DA__INCLUDED_)
