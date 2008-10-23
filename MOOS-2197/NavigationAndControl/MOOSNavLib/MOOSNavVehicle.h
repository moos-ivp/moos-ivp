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
// MOOSNavVehicle.h: interface for the CMOOSNavVehicle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVVEHICLE_H__AC4BAF08_DD52_4ED9_A7B3_860B132D32BD__INCLUDED_)
#define AFX_MOOSNAVVEHICLE_H__AC4BAF08_DD52_4ED9_A7B3_860B132D32BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSNavEntity.h"

class CMOOSNavVehicle  : public CMOOSNavEntity
{
public:
    class CPlantNoise
    {
    public:
        //noise terms for static model
        //noise is in velocity
        double m_dfqXv;
        double m_dfqYv;
        double m_dfqZv;
        double m_dfqHv;

        //noise terms for dynamic model
        //noise is in acceleration
        double m_dfqXa;
        double m_dfqYa;
        double m_dfqZa;
        double m_dfqHa;

    };
public:
    bool SetDroppedState(bool bDropped);
    bool SetDynamics(double dfDynamicsXY,double dfDynamicsZ,double dfDynamicsYaw);


    CMOOSNavVehicle();
    virtual ~CMOOSNavVehicle();


    virtual bool GetFullState(Matrix &Result,Matrix * m_pXToUse=NULL,bool bUseEstimate=true);
    virtual bool RefreshState();
    virtual bool FillModelMatrices(Matrix &F,Matrix &Q,Matrix & Xhat,double dfDeltaT);

    virtual int GetStateSize();

protected:
    bool FillPoseOnlyModelMatrices(Matrix & jF,Matrix & jQ,Matrix & Xhat,double dfDeltaT);
    bool FillPoseAndRateModelMatrices(Matrix &jF, Matrix &jQ, Matrix &Xhat, double dfDeltaT);

    CPlantNoise m_Noise;
    bool    m_bDroppedState;
};

#endif // !defined(AFX_MOOSNAVVEHICLE_H__AC4BAF08_DD52_4ED9_A7B3_860B132D32BD__INCLUDED_)
