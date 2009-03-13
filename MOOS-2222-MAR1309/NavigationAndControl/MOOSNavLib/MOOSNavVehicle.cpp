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
// MOOSNavVehicle.cpp: implementation of the CMOOSNavVehicle class.
//
//////////////////////////////////////////////////////////////////////
#include "MOOSNavLibGlobalHelper.h"
#include "MOOSNavVehicle.h"
#include "MOOSNavLibDefs.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavVehicle::CMOOSNavVehicle()
{
    m_eType = POSE_ONLY;
    

    m_bDroppedState = false;

}

CMOOSNavVehicle::~CMOOSNavVehicle()
{

}

bool CMOOSNavVehicle::GetFullState(Matrix &Result,Matrix * m_pXToUse,bool bUseEstimate)
{
    if(Result.Nrows()!=FULL_STATES    || Result.Ncols()!=1)
    {
        Result.ReSize(FULL_STATES,1);
    }

    //have we been told to use a vector other than the global state vector?
    if(m_pXToUse==NULL)
    {
        //yes (this may happen during numerical evaluation of jacobians
        //for example
        m_pXToUse = m_pXhat;
    }

    //set all to zero..
    Result    =0;
    
    if(bUseEstimate)
    {
        I_X(Result,1)=I_X((*m_pXToUse),m_nStart);
        I_Y(Result,1)=I_Y((*m_pXToUse),m_nStart);
        I_Z(Result,1)=I_Z((*m_pXToUse),m_nStart);
        I_H(Result,1)=I_H((*m_pXToUse),m_nStart);

        if(m_eType==POSE_AND_RATE)
        {
            I_Xdot(Result,1)=I_Xdot((*m_pXToUse),m_nStart);
            I_Ydot(Result,1)=I_Ydot((*m_pXToUse),m_nStart);
            I_Zdot(Result,1)=I_Zdot((*m_pXToUse),m_nStart);
            I_Hdot(Result,1)=I_Hdot((*m_pXToUse),m_nStart);
        }
    }
    else
    {
        I_X(Result,1)=m_State.m_dfX;
        I_Y(Result,1)=m_State.m_dfY;
        I_Z(Result,1)=m_State.m_dfZ;
        I_H(Result,1)=m_State.m_dfH;

        if(m_eType==POSE_AND_RATE)
        {
            I_Xdot(Result,1)=m_State.m_dfXdot;
            I_Ydot(Result,1)=m_State.m_dfYdot;
            I_Zdot(Result,1)=m_State.m_dfZdot;
            I_Hdot(Result,1)=m_State.m_dfHdot;
        }
    }

    return true;



}


int CMOOSNavVehicle::GetStateSize()
{
    switch(m_eType)
    {
        case POSE_ONLY:        return POSE_ONLY_STATES; break;
        case POSE_AND_RATE: return POSE_AND_RATE_STATES; break;
        default:
            return 0;
    }
}

bool CMOOSNavVehicle::RefreshState()
{


//    MOOSMatrixTrace(*m_pPhat,"Phat");
    m_State.m_dfX=I_X((*m_pXhat),m_nStart);
    m_State.m_dfY=I_Y((*m_pXhat),m_nStart);
    m_State.m_dfZ=I_Z((*m_pXhat),m_nStart);
    m_State.m_dfH=I_H((*m_pXhat),m_nStart);

    m_State.m_dfPX=(*m_pPhat)(m_nStart+iiX,m_nStart+iiX);
    m_State.m_dfPY=(*m_pPhat)(m_nStart+iiY,m_nStart+iiY);
    m_State.m_dfPZ=(*m_pPhat)(m_nStart+iiZ,m_nStart+iiZ);
    m_State.m_dfPH=(*m_pPhat)(m_nStart+iiH,m_nStart+iiH);


    if(m_eType==POSE_AND_RATE)
    {
        m_State.m_dfXdot=I_Xdot((*m_pXhat),m_nStart);
        m_State.m_dfYdot=I_Ydot((*m_pXhat),m_nStart);
        m_State.m_dfZdot=I_Zdot((*m_pXhat),m_nStart);
        m_State.m_dfHdot=I_Hdot((*m_pXhat),m_nStart);        


        m_State.m_dfPXdot=(*m_pPhat)(m_nStart+iiXdot,m_nStart+iiXdot);
        m_State.m_dfPYdot=(*m_pPhat)(m_nStart+iiYdot,m_nStart+iiYdot);
        m_State.m_dfPZdot=(*m_pPhat)(m_nStart+iiZdot,m_nStart+iiZdot);
        m_State.m_dfPHdot=(*m_pPhat)(m_nStart+iiHdot,m_nStart+iiHdot);


    }

    m_State.m_dfSpeed = sqrt(pow(m_State.m_dfXdot,2)+pow(m_State.m_dfYdot,2)+pow(m_State.m_dfZdot,2));

    m_State.m_dfDepth = (*m_pXhat)(TIDE_STATE_INDEX,1)-m_State.m_dfZ;
    return true;
}

bool CMOOSNavVehicle::FillModelMatrices(Matrix &F,Matrix &Q,Matrix & Xhat,double dfDeltaT)
{

    switch(m_eType)
    {
    case POSE_AND_RATE:
        return FillPoseAndRateModelMatrices(F,Q,Xhat,dfDeltaT);
        break;    
    case POSE_ONLY:
        return FillPoseOnlyModelMatrices(F,Q,Xhat,dfDeltaT);
        break;    
    default:
        MOOSTrace("err what kind of vehicle is this?\n");
        return false;
    }

}

bool CMOOSNavVehicle::SetDynamics(double dfDynamicsXY, double dfDynamicsZ, double dfDynamicsYaw)
{
    double dfSFX = dfDynamicsXY/10.0;
    double dfSFY = dfDynamicsXY/10.0;
    double dfSFZ = dfDynamicsZ/10.0;
    double dfSFH = dfDynamicsYaw/10.0;

    m_Noise.m_dfqXv = pow((dfSFX*FULL_SCALE_Q_VEL),2);
    m_Noise.m_dfqYv = pow((dfSFY*FULL_SCALE_Q_VEL),2);
    m_Noise.m_dfqZv = pow((dfSFZ*FULL_SCALE_Q_VEL),2);
    m_Noise.m_dfqHv = pow((dfSFH*FULL_SCALE_HEADING_VEL),2);


    m_Noise.m_dfqXa =dfSFX*FULL_SCALE_Q_ACC_XY;
    m_Noise.m_dfqYa =dfSFY*FULL_SCALE_Q_ACC_XY;
    m_Noise.m_dfqZa    =dfSFZ*FULL_SCALE_Q_ACC_Z;
    m_Noise.m_dfqHa    =dfSFH*FULL_SCALE_HEADING_ACC;

    return true;
}



bool CMOOSNavVehicle::FillPoseOnlyModelMatrices(Matrix &jF, Matrix &jQ, Matrix &Xhat, double dfDeltaT)
{

    //static vehicles don't move!
/*
    F = 1    0    0    0
        0    1    0    0    
        0    0    1    0
        0    0    0    1

    Q = v    0    0    0
        0    v    0    0
        0    0    v    0
        0    0    0    v

    where v = dT
    ( x= v * t -> x^2 = v^2 * dT^2 )

*/

    double dfT2 = pow(dfDeltaT,2);
    int iPx = m_nStart+iiX;
    int iPy    = m_nStart+iiY;
    int iPz = m_nStart+iiZ;
    int iPh = m_nStart+iiH;


    
    jQ(iPx,iPx) = m_Noise.m_dfqXv*dfT2;  //qv^2* dT^2
    jQ(iPy,iPy) = m_Noise.m_dfqYv*dfT2;
    jQ(iPz,iPz) = m_Noise.m_dfqZv*dfT2;
    jQ(iPh,iPh) = m_Noise.m_dfqHv*dfT2;


    jF(iPx,iPx) = 1.0;  
    jF(iPy,iPy) = 1.0;
    jF(iPz,iPz) = 1.0;
    jF(iPh,iPh) = 1.0;                

    return true;
}

bool CMOOSNavVehicle::FillPoseAndRateModelMatrices(Matrix &jF, Matrix &jQ, Matrix &Xhat, double dfDeltaT)
{




/*
        F =        |1    0    0    0    dT    0    0    0 |
                |0    1    0    0    0    dT    0    0 |    
                |0    0    1    0    0    0    dT    0 |    
                |0    0    0    1    0    0    0    dT|    
                |0    0    0    0    1    0    0    0 |    
                |0    0    0    0    0    1    0    0 |    
                |0    0    0    0    0    0    1    0 |    
                |0    0    0    0    0    0    0    1 |    
    */

    
    int iPx = m_nStart+iiX;
    int iPy    = m_nStart+iiY;
    int iPz = m_nStart+iiZ;
    int iPh = m_nStart+iiH;
    int iVx = m_nStart+iiXdot;
    int iVy    = m_nStart+iiYdot;
    int iVz = m_nStart+iiZdot;
    int iVh = m_nStart+iiHdot;

    int i = 0;
    for(i = m_nStart; i<=m_nEnd;i++)
    {
        jF(i,i) = 1.0;
    }


    for(i = 0; i<4;i++)
    {
        jF(m_nStart+i,m_nStart+i+4) = dfDeltaT;
    }

    

    ///////////////////////////////////////////////////
    // now sort out Q

    double dfT4 = pow(dfDeltaT,4)/4;
    double dfT3 = pow(dfDeltaT,3)/2;
    double dfT2 = pow(dfDeltaT,2);




    //do the diagonals first 
    jQ(iPx,iPx) = m_Noise.m_dfqXa*dfT4;  //qa^2* dT^4/4
    jQ(iPy,iPy) = m_Noise.m_dfqYa*dfT4;
    jQ(iPz,iPz) = m_Noise.m_dfqZa*dfT4;
    jQ(iPh,iPh) = m_Noise.m_dfqHa*dfT4;

    jQ(iVx,iVx) = m_Noise.m_dfqXa*dfT2;  //qa^2* dT^2
    jQ(iVy,iVy) = m_Noise.m_dfqYa*dfT2;
    jQ(iVz,iVz) = m_Noise.m_dfqZa*dfT2;
    jQ(iVh,iVh) = m_Noise.m_dfqHa*dfT2;
        

    //cross correlation terms
    jQ(iPx,iVx) = m_Noise.m_dfqXa*dfT3;  //qa^2 * dT^3 /3
    jQ(iPy,iVy) = m_Noise.m_dfqYa*dfT3;
    jQ(iPz,iVz) = m_Noise.m_dfqZa*dfT3;
    jQ(iPh,iVh) = m_Noise.m_dfqHa*dfT3;


    jQ(iVx,iPx) = m_Noise.m_dfqXa*dfT3;
    jQ(iVy,iPy) = m_Noise.m_dfqYa*dfT3;
    jQ(iVz,iPz) = m_Noise.m_dfqZa*dfT3;
    jQ(iVh,iPh) = m_Noise.m_dfqHa*dfT3;

//    MOOSMatrixTrace(jQ,"Q");
    
    return true;
}

//used to mark the vehicle as an 'old' vehicle location and
//not used actively in tracking...
bool CMOOSNavVehicle::SetDroppedState(bool bDropped)
{
    m_bDroppedState = bDropped;
    return true;
}
