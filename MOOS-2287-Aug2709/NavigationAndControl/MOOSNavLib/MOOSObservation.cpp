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
// MOOSObservation.cpp: implementation of the CMOOSObservation class.
//
//////////////////////////////////////////////////////////////////////
#include <math.h>
#include "MOOSNavEntity.h"
#include "MOOSNavBeacon.h"
#include "MOOSNavVehicle.h"

#include "MOOSNavSensor.h"
#include "MOOSObservation.h"
#include "MOOSNavLibGlobalHelper.h"
#include "MOOSNavLibDefs.h"

#include <sstream>
#include <iomanip>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




bool CMOOSObservation::JacCallBack( Matrix & XOut,
                                   Matrix & XIn,
                                   void * pParam)
{
    
    CMOOSObservation* pMe = (CMOOSObservation*)pParam;
    
    return pMe->JacEvaluate(XIn,XOut);
    
}



CMOOSObservation::CMOOSObservation()
{
    //good guess
    m_dfSV = 1500.0;
    
    //default of one element deep
    m_nDim = 1;

    m_nRow = -1;

    //default lbl channel is -1
    m_nChan = -1;

    //this observaiton has not been used yet
    m_bUsed = false;

    //by default do thing explicitly
    m_bNumericalJacobians = false;
    
    //by defualt don't ignore observations
    m_bIgnore = false;

    //assume good data association
    m_bGoodDA = true;

    //we by default don't expect to be a fixed observation!
    m_bFixed = false;

    m_dfInnov = 0;

    //negative->not set
    m_dfInnovStd = -1;

    //by default don't look for heading bias state!
    m_bUseHeadingBias = false;


}

CMOOSObservation::~CMOOSObservation()
{
    
}


bool CMOOSObservation::MakeMatrices(Matrix &Innov,
                                    Matrix &jH,
                                    Matrix &jR,
                                    Matrix &Xhat)
{
    
    
    
    switch(m_eType)
    {
    case X:
    case Y:
        return MakeXYMatrices(Innov,
            jH,
            jR);
    case LBL_BEACON_2WR:
        return MakeBeacon2WRMatrices(Innov,
            jH,
            jR);
        break;
    case YAW:
        return MakeYawMatrices(Innov,
            jH,
            jR);
        break;

    case DEPTH:
        return MakeDepthMatrices(Innov,
            jH,
            jR);
        break;

    case TIDE:
        return MakeTideMatrices(Innov,
            jH,
            jR);
        break;

    case HEADING_BIAS:
        return MakeHeadingBiasMatrices(Innov,
            jH,
            jR);
        break;

    case BODY_VEL_Y:
    case BODY_VEL_X:
        return MakeBodyVelMatrices(Innov,
            jH,
            jR);
        break;

    default:
        break;
    }
    
    
    return false;
}


bool CMOOSObservation::MakeBeacon2WRMatrices(Matrix &Innov,
                                             Matrix &jH,
                                             Matrix &jR)
                                             
{
   
    double dfTOF=0;
    
    ////////////// NOTE TO READERS ///////////////////////////
    //note this is somewhat confusing if you aren't careful
    //times are calculated using the current estimates of 
    //position and velocity. eg a V1 at origin moving 1m/s in
    //x direction towards a beacon at 1500 on x axis will have
    //and estimated travel time of GREATER than 2 seconds!
    //this is because if the vehcile is at the origin now
    //it would have been more than 1500 m away when it transmitted
    //the interrogation.... cunning!
    
    m_pXEvaluate = m_pInterrogateSensor->GetParent()->m_pXhat;
    
    bool bSolved = false;
    
    bSolved =  m_LBLMaths.CalculateTwoWayTOF(    m_pInterrogateSensor,
        m_pRespondingSensor,
        m_pXEvaluate,
        m_dfSV,
        dfTOF);

    if(!bSolved)
    {
        return false;
    }
    
    //fill in innovation
    Innov(m_nRow,1) = m_dfData-dfTOF;
    
//    MOOSTrace("Predicted travel time = %f seconds\n",dfTOF);
    
    //now look after jacobian
    Matrix JNumeric,JExplicit;

    //make jacobian maths use the state predicted...
    Matrix* pXPred=m_pInterrogateSensor->GetParent()->m_pXhat;
    

    if(m_bNumericalJacobians)
    {
        MOOSGetJacobian(JNumeric,
            *pXPred,
            1,
            pXPred->Nrows(),
            JacCallBack,
            this);
    }
    else
    {
        m_LBLMaths.CalculateTwoWayTOFJacobians(m_pInterrogateSensor,
                                                m_pRespondingSensor,
                                                pXPred,
                                                m_dfSV,
                                                JExplicit);
    }

//    MOOSMatrixTrace(JNumeric,"Numeric");
//    MOOSMatrixTrace(JExplicit,"Explicit");


    //fill in relevant sub matrix in jH
    jH.SubMatrix(m_nRow,m_nRow+m_nDim-1,1,jH.Ncols())=JExplicit;
    

    //and now fill in the observation noise
    jR(m_nRow,m_nRow) = pow(m_dfDataStd,2);
    

    
    return bSolved;
}





bool CMOOSObservation::JacEvaluate(Matrix & XIn,Matrix & XOut)
{
    //store old XEvaluate pointer
    Matrix * pOldXEvaluate = m_pXEvaluate;
    
    //make XEvaluate pointer point to our new input vector...
    m_pXEvaluate = &XIn;
    
    
    //now do the calculation....
    double dfTOF=0;
    bool bSolved =  m_LBLMaths.CalculateTwoWayTOF(    m_pInterrogateSensor,
        m_pRespondingSensor,
        m_pXEvaluate,
        m_dfSV,
        dfTOF);
    
    if(bSolved)
    {
        XOut(1,1) = dfTOF;
    }
    
    
    //restore XEvaluate pointer
    m_pXEvaluate = pOldXEvaluate;
    
    return true;
}

int CMOOSObservation::GetDimension()
{
    return m_nDim;
}


bool CMOOSObservation::MakeHeadingBiasMatrices(     Matrix &Innov,
                                             Matrix &jH,
                                             Matrix &jR)
{
    //z = Bias;
    m_pXEvaluate = m_pInterrogateSensor->GetParent()->m_pXhat;

    int nStateNdx = HEADING_BIAS_STATE_INDEX;

    double dfZPred = (*m_pXEvaluate)(nStateNdx,1);
    
    Innov(m_nRow,1)                        = m_dfData-dfZPred;

    jR(m_nRow,m_nRow)                    = pow(m_dfDataStd,2);

    jH(m_nRow,nStateNdx) = 1.0;
    
    return true;
}


bool CMOOSObservation::MakeTideMatrices(     Matrix &Innov,
                                             Matrix &jH,
                                             Matrix &jR)
{

    m_pXEvaluate = m_pInterrogateSensor->GetParent()->m_pXhat;

    int nStateNdx = TIDE_STATE_INDEX;

    double dfZPred = (*m_pXEvaluate)(nStateNdx,1);
    
    Innov(m_nRow,1)                        = m_dfData-dfZPred;

    jR(m_nRow,m_nRow)                    = pow(m_dfDataStd,2);

    jH(m_nRow,nStateNdx) = 1.0;
    
    return true;
}


bool CMOOSObservation::MakeDepthMatrices(Matrix &Innov,
                                             Matrix &jH,
                                             Matrix &jR)
{

    CMOOSNavEntity * pEntity = m_pInterrogateSensor->GetParent();
    m_pXEvaluate = pEntity->m_pXhat;

    
    //z component of entity measuring depth...
    int nStateNdx =pEntity->m_nStart+iiZ;

    double dfZv = 0.0;

    if(pEntity->GetEntityType() == CMOOSNavEntity::FIXED)
    {
        //at thios point we could worry about roll and pitch corrections
        //but we don't expect this to be big or important in non USBL applications

        //FIXED vehicle Z + sensor Z
        dfZv = pEntity->m_State.m_dfZ+m_pInterrogateSensor->m_Offset.m_dfZ;

    }
    else
    {
        //moving vehicle Z + sensor Z
        dfZv = (*m_pXEvaluate)(nStateNdx,1)+m_pInterrogateSensor->m_Offset.m_dfZ;
    }


    //depth is a function of Tide
    //depth = Tide-Zv
    double S = (*m_pXEvaluate)(TIDE_STATE_INDEX,1);

    double dfZPred = (S-dfZv);

    //always have this component
    jH(m_nRow,TIDE_STATE_INDEX)                = 1.0;

    if(pEntity->GetEntityType() !=CMOOSNavEntity::FIXED)
    {
        //add mobile state derivative
        jH(m_nRow,nStateNdx)            = -1.0;
    }

    Innov(m_nRow,1)                        = m_dfData-dfZPred;



    jR(m_nRow,m_nRow)                        = pow(m_dfDataStd,2);

    return true;

}

bool CMOOSObservation::MakeXYMatrices(Matrix &Innov,
                                             Matrix &jH,
                                             Matrix &jR)
{

    CMOOSNavVehicle* pVeh = dynamic_cast< CMOOSNavVehicle * >(m_pInterrogateSensor->GetParent());

    if(pVeh == NULL)
        return false;

    m_pXEvaluate = pVeh->m_pXhat;

    


    ////////////////////////////////////////////////
    // global and body stablised coordinates

    m_pXEvaluate = m_pInterrogateSensor->GetParent()->m_pXhat;

    double dfX = (*m_pXEvaluate)(pVeh->m_nStart+iiX,1);
    double dfY = (*m_pXEvaluate)(pVeh->m_nStart+iiY,1);
    double dfH = (*m_pXEvaluate)(pVeh->m_nStart+iiH,1);

    double dfx,dfy,dfz;
    m_pInterrogateSensor->GetAlignedOffsets(dfH,dfx,dfy,dfz);



    
    double dfHdot = 0;
    if(pVeh->GetEntityType()==CMOOSNavVehicle::POSE_AND_RATE)
    {
        dfHdot = (*m_pXEvaluate)(pVeh->m_nStart+iiHdot,1);
    }
    

    double dT = 0.0;    
                                                
    //////////////////////////////////////////////
    //   prediction part.... note rotation matrix is
    //
    //   | c -s | 
    //   | s  c | 
    //
    //  ie the transpose (inverse) of what we might expect
    //  as we are taking rotated pojnts back to the base frame

    double     dfPX = dfX + dfx* cos(dfH) - dfy*sin(dfH);
    double     dfPY = dfY + dfx* sin(dfH) + dfy*cos(dfH);


    //////////////////////////////////////////////
    //    jacobian part....

    //differential of X w.r.t heading
    double dXdH = -(dfx* sin(dfH)+dfy*cos(dfH)- ( (dfx*cos(dfH)-dfy*sin(dfH) )*dfHdot*dT));

    //differential of Y w.r.t heading
    double dYdH = dfx*cos(dfH) - dfy*sin(dfH) + (( dfx* sin(dfH) + dfy*cos(dfH))*dfHdot*dT);

    double dXdX = 1.0;
    
    double dYdY = 1.0;

    //all other differntials dX/dY are clearly zero
    //remain slightly concerned about dZ/dw though...

    /////////////////////////////////////////////
    // Now fill in the matrices!

    double dfZPred;
    switch(m_eType)
    {
    case X:
        dfZPred = dfPX;
        jH(m_nRow,pVeh->m_nStart+iiX) = dXdX;
        jH(m_nRow,pVeh->m_nStart+iiH) = dXdH;
        break;
    case Y:
        dfZPred = dfPY;
        jH(m_nRow,pVeh->m_nStart+iiY) = dYdY;
        jH(m_nRow,pVeh->m_nStart+iiH) = dYdH;
        break;
    default:
        return false;
    }


    Innov(m_nRow,1)        = (m_dfData-dfZPred);
    jR(m_nRow,m_nRow)    = pow(m_dfDataStd,2);


    return true;

}



bool CMOOSObservation::MakeYawMatrices(Matrix &Innov,
                                             Matrix &jH,
                                             Matrix &jR)

{
    
    m_pXEvaluate = m_pInterrogateSensor->GetParent()->m_pXhat;

    int nStateNdx = m_pInterrogateSensor->GetParent()->m_nStart+iiH;


    double dfZPred = (*m_pXEvaluate)(nStateNdx,1);

    if(m_bUseHeadingBias)
    {
        dfZPred+=(*m_pXEvaluate)(HEADING_BIAS_STATE_INDEX,1);
    }
    

    Innov(m_nRow,1)                        = MOOS_WRAP_ANGLE(m_dfData-dfZPred);

    jR(m_nRow,m_nRow)                    = pow(m_dfDataStd,2);

    jH(m_nRow,nStateNdx) = 1.0;

    if(m_bUseHeadingBias)
    {
        //z = Yaw+Bias
        jH(m_nRow,HEADING_BIAS_STATE_INDEX) = 1.0;
    }


    return true;
}

bool CMOOSObservation::MakeBodyVelMatrices(Matrix &Innov,
                                             Matrix &jH,
                                             Matrix &jR)

{

    if(m_eType!=BODY_VEL_Y && m_eType!=BODY_VEL_X)
    {
        jH.SubMatrix(m_nRow,m_nRow,1,jH.Ncols())=0;
        MOOSTrace("Only Body Vel X and Y supprted so far\n");
        return false;
    }
    if(m_pInterrogateSensor->GetParent()->GetEntityType()!=CMOOSNavEntity::POSE_AND_RATE)
    {
        MOOSTrace("cannot use body vel obs on static vehcile\n");
        return false;
    }

    //ok begin....
    m_pXEvaluate = m_pInterrogateSensor->GetParent()->m_pXhat;

    int nStart = m_pInterrogateSensor->GetParent()->m_nStart;
    double dfYaw = (*m_pXEvaluate)(nStart+iiH,1);

    if(m_eType==BODY_VEL_Y)
    {
        //resolve onto axis 90 deg further around..
        dfYaw+=PI/2;
    }

    double dfXdot = (*m_pXEvaluate)(nStart+iiXdot,1);
    double dfYdot = (*m_pXEvaluate)(nStart+iiYdot,1);

    double dfZPred = dfXdot*cos(dfYaw)+dfYdot*sin(dfYaw);
    
    Innov(m_nRow,1)                        = m_dfData-dfZPred;

    jR(m_nRow,m_nRow)                    = pow(m_dfDataStd,2);

    jH(m_nRow,nStart+iiXdot) = cos(dfYaw);
    jH(m_nRow,nStart+iiYdot) = sin(dfYaw);
    jH(m_nRow,nStart+iiH) = -dfXdot*sin(dfYaw)+dfYdot*cos(dfYaw);


    return true;
}




void CMOOSObservation::Trace()
{
    string sType;
    switch(m_eType)
    {
        case    UNKNOWN_TYPE: sType = "Unknown"; break;
        case    X: sType = "X"; break;
        case    Y: sType = "Y"; break;
        case     YAW: sType = "Yaw"; break;
        case    LBL_BEACON_2WR: sType = "Beacon 2WR"; break;
        case    DEPTH: sType = "Depth"; break;
        case    SPEED: sType = "Speed"; break;
        case    THRUST: sType = "Thrust"; break;
        case    RUDDER: sType = "Rudder"; break;
        case    ELEVATOR: sType = "Elevator"; break;
        case    TIDE: sType = "Tide";break;
        case    BODY_VEL_X:sType = "BodyVelX";break;
        case    BODY_VEL_Y:sType = "BodyVelY";break;
        case    BODY_VEL_Z:sType = "BodyVelZ";break;
    }

    MOOSTrace("ID %d Type=%s,Time = %f,Data=%f\n",
        GetID(),sType.c_str(),m_dfTime,m_dfData);

    if(m_eType ==LBL_BEACON_2WR)
    {
        MOOSTrace("\t%s -> %s\n",
            m_pInterrogateSensor->GetParent()->GetName().c_str(),
            m_pRespondingSensor->GetParent()->GetName().c_str());
    }


}


void CMOOSObservation::DoDebug()
{
    
}

//returns a unique string describing the type obs
//I use source sensor and if acoustic the channel
string CMOOSObservation::GetName()
{
    ostringstream os;

    os<<m_pInterrogateSensor->GetName();

    if(m_eType ==LBL_BEACON_2WR)
    {
        os<<"["<<m_nChan<<"]";
    }
    if(m_eType==X)
    {
        os<<"[X]";
    }
    if(m_eType==Y)
    {
        os<<"[Y]";
    }


    os<<ends;

    string sAnswer = os.str();
//    os.rdbuf()->freeze(0);

    return sAnswer;
}

bool CMOOSObservation::IsFixed()
{
    return m_bFixed;
}

bool CMOOSObservation::SetFixed(bool bFixed)
{
    m_bFixed = bFixed;

    return true;
}

bool CMOOSObservation::IsType(CMOOSObservation::Type eType)
{
    return m_eType == eType;
}

bool CMOOSObservation::Ignore(bool bIgnore)
{
    m_bIgnore = bIgnore;

    return true;
}

bool CMOOSObservation::SetGoodDA(bool bGoodDA)
{
    m_bGoodDA = bGoodDA;
    return true;
}

void CMOOSObservation::UsingHeadingBias(bool bUsing)
{
    m_bUseHeadingBias = bUsing;
}
