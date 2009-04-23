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
// MOOSNavEntity.h: interface for the CMOOSNavEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVENTITY_H__C8ADA46B_7B80_4102_8E42_7242BC049AF7__INCLUDED_)
#define AFX_MOOSNAVENTITY_H__C8ADA46B_7B80_4102_8E42_7242BC049AF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSNavBase.h"
#include <newmat.h>
using namespace NEWMAT;

#include "MOOSNavSensor.h"


class CMOOSNavEntity : public CMOOSNavBase  
{    
public:

    bool RefreshStateCovariance();
    bool RefreshStateVector();
    virtual bool FillModelMatrices(Matrix &F,Matrix &Q,Matrix & Xhat,double dfDeltaT)=0;

    enum Type
    {
        FIXED,
        POSE_ONLY,
        POSE_AND_RATE,
    };

    Type m_eType;
    bool SetEntityType(CMOOSNavEntity::Type eType){m_eType=eType;return true;};
    CMOOSNavEntity::Type GetEntityType(){return m_eType;};



    virtual int GetStateSize()=0;
    virtual bool GetFullState(Matrix &Result,Matrix * m_pXToUse=NULL,bool bUseEstimate=true)=0;
    virtual bool RefreshState()=0;

    CMOOSNavSensor* GetSensorByName(const string & sName);
    CMOOSNavSensor* GetSensorByType(CMOOSNavSensor::Type eType);

    //add the given sensor to the vehicle
    bool AddSensor(CMOOSNavSensor * pSensor);

    CMOOSNavEntity();
    virtual ~CMOOSNavEntity();

    /// map of named sensors on vehicle
    SENSOR_MAP m_SensorMap;

    /// index to start of object in matrices
    int m_nStart;

    /// index to end of object in matrices
    int m_nEnd;

    ///pointer to estimated statevector
    Matrix * m_pXhat;
    ///pointer to estimated statevector covariance
    Matrix * m_pPhat;


    class CState
    {
    public:

        CState();

        double m_dfX;
        double m_dfY;
        double m_dfZ;
        double m_dfH;
        double m_dfXdot;
        double m_dfYdot;
        double m_dfZdot;
        double m_dfHdot;
        double m_dfDepth;

        double m_dfPX;
        double m_dfPY;
        double m_dfPZ;
        double m_dfPH;
        double m_dfPXdot;
        double m_dfPYdot;
        double m_dfPZdot;
        double m_dfPHdot;

        double m_dfSpeed;

    
    };
    CState m_State;
};

//typedef map<int,CMOOSNavEntity*> ENTITYMAP;

#endif // !defined(AFX_MOOSNAVENTITY_H__C8ADA46B_7B80_4102_8E42_7242BC049AF7__INCLUDED_)
