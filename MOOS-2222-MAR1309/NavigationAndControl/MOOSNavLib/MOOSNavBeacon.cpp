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
// MOOSNavBeacon.cpp: implementation of the CMOOSNavBeacon class.
//
//////////////////////////////////////////////////////////////////////
#include "MOOSNavSensor.h"
#include "MOOSNavBeacon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavBeacon::CMOOSNavBeacon()
{
    m_eType = FIXED;
    m_bPseudo = false;

    CMOOSNavSensor * pLBL = new CMOOSNavSensor;
    pLBL->m_sName = "LBL";
    pLBL->m_eType = CMOOSNavSensor::LBL;

    AddSensor(pLBL);

}

CMOOSNavBeacon::~CMOOSNavBeacon()
{

}


bool CMOOSNavBeacon::GetFullState(Matrix &Result,Matrix * m_pXToUse,bool bUseEstimate)
{
    if(Result.Nrows()!=FULL_STATES    || Result.Ncols()!=1)
    {
        Result.ReSize(FULL_STATES,1);
    }

    //set all to zero..
    Result    =0;
    
    I_X(Result,1)=m_State.m_dfX;
    I_Y(Result,1)=m_State.m_dfY;
    I_Z(Result,1)=m_State.m_dfZ;


    return true;



}

int CMOOSNavBeacon::GetStateSize()
{
    return 0;
}

void CMOOSNavBeacon::Trace()
{
    CMOOSNavBase::Trace();
}


bool CMOOSNavBeacon::RefreshState()
{
    //nothing to do!
    //nothing can change!
    return true;
}


bool CMOOSNavBeacon::FillModelMatrices(Matrix &F,Matrix &Q,Matrix & Xhat,double dfDeltaT)
{
    return true;
}

