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
// MOOSNavEKFEngine.cpp: implementation of the CMOOSNavEKFEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "MOOSNavLibGlobalHelper.h"
#include "MOOSNavLibDefs.h"
#include "MOOSNavObsStore.h"
#include "MOOSNavVehicle.h"
#include "MOOSNavEKFEngine.h"

#define MAX_ALLOWED_LIN_VELOCITY 3.0
#define MAX_ALLOWED_ANG_VELOCITY MOOSDeg2Rad(15)
#define EKF_ITERATE_PERIOD 0.3
#define TIME_SLICE (EKF_ITERATE_PERIOD/2.0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavEKFEngine::CMOOSNavEKFEngine()
{
    m_dfXYDynamics=5;
    m_dfZDynamics=5;
    m_dfYawDynamics=5;    
    
    m_dfPxx0    = pow(30.0,2.0);
    m_dfPyy0    = pow(30.0,2.0);
    m_dfPzz0    = pow(10.0,2.0);
    m_dfPhh0    = pow(MOOSDeg2Rad(90),2.0);
    
    m_dfX0 = 0;
    m_dfY0 = 0;
    m_dfZ0 = 50;
    m_dfH0 = 0;
    m_dfTide0 = 50;
    
    m_dfLastIterated = 0;
    
    m_dfLag = 2.0;
    
    m_sName = "EKF";
    
    m_bBooted = 0;

    m_dfYawBiasStd = 0;
    
    m_dfMaxZVel = MAX_ALLOWED_LIN_VELOCITY;

    m_bInitialOnline = false;
}

CMOOSNavEKFEngine::~CMOOSNavEKFEngine()
{
    
}


bool CMOOSNavEKFEngine::Initialise(STRING_LIST  sParams)
{
    
    //before calling base class member peek to see if we
    //are being told to modify the defaul kind of vehcile that
    //will be built
    string sVal;
    if(MOOSGetValueFromToken(sParams,"EKF_VEHICLE_TYPE",sVal))
    {
        if(MOOSStrCmp(sVal,"MOBILE"))
        {
            m_eVehicleType =  CMOOSNavEntity::POSE_AND_RATE;
        }
    }

    //do we want to estimate heading bias?
    if(MOOSGetValueFromToken(sParams,"EKF_ESTIMATE_YAW_BIAS",sVal))
    {
        if(MOOSStrCmp(sVal,"TRUE"))
        {
            m_bEstimateHeadingBias = true;
        }
    }



    //now call base class version
    if(!CMOOSNavEngine::Initialise(sParams))
    {
        return false;
    }
    

       if(MOOSGetValueFromToken(sParams,"EKF_YAW_BIAS_NOISE",sVal))
    {
        m_dfYawBiasStd = atof(sVal.c_str());
        if(m_dfYawBiasStd<0) 
            m_dfYawBiasStd=0;
    }

    
    if(MOOSGetValueFromToken(sParams,"EKF_LAG",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal<0) dfVal=0;
        if(dfVal>4) dfVal = 4;
        m_dfLag = dfVal;
    }
    


    

    //now we may have other things to set up....
    if(MOOSGetValueFromToken(sParams,"EKF_XY_DYNAMICS",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal<0) dfVal=0;
        if(dfVal>10) dfVal = 10;
        m_dfXYDynamics = dfVal;
    }
    
    if(MOOSGetValueFromToken(sParams,"EKF_Z_DYNAMICS",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal<0) dfVal=0;
        if(dfVal>10) dfVal = 10;
        m_dfZDynamics = dfVal;
    }
    
    if(MOOSGetValueFromToken(sParams,"EKF_YAW_DYNAMICS",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal<0) dfVal=0;
        if(dfVal>10) dfVal = 10;
        m_dfYawDynamics = dfVal;
    }
    
    
    //rad in intial uncertainties
    if(MOOSGetValueFromToken(sParams,"EKF_SIGMA_XX",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal>0)
        {
            m_dfPxx0 = pow(dfVal,2);
        }
    }
    
    if(MOOSGetValueFromToken(sParams,"EKF_SIGMA_YY",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal>0)
        {
            m_dfPyy0 = pow(dfVal,2);
        }
    }
    
    if(MOOSGetValueFromToken(sParams,"EKF_SIGMA_ZZ",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal>0)
        {
            m_dfPzz0 = pow(dfVal,2);
        }
    }
    
    if(MOOSGetValueFromToken(sParams,"EKF_SIGMA_HH",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal>0)
        {
            m_dfPhh0 = pow(MOOSDeg2Rad(dfVal),2);
        }
    }
    
    if(MOOSGetValueFromToken(sParams,"EKF_SIGMA_TIDE",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal>0)
        {
            m_dfPTide0 = pow((dfVal),2);
        }
    }
    
    
    //where shall we start from?
    if(MOOSGetValueFromToken(sParams,"EKF_X",sVal))
    {
        double dfVal = atof(sVal.c_str());
        m_dfX0 = dfVal;
    }
    if(MOOSGetValueFromToken(sParams,"EKF_Y",sVal))
    {
        double dfVal = atof(sVal.c_str());
        m_dfY0 = dfVal;
    }
    if(MOOSGetValueFromToken(sParams,"EKF_Z",sVal))
    {
        double dfVal = atof(sVal.c_str());
        m_dfZ0 = dfVal;
    }
    if(MOOSGetValueFromToken(sParams,"EKF_H",sVal))
    {
        double dfVal = atof(sVal.c_str());
        //convert to yaw in degrees
        m_dfH0 = MOOSDeg2Rad(-dfVal);
    }
    if(MOOSGetValueFromToken(sParams,"EKF_TIDE",sVal))
    {
        double dfVal = atof(sVal.c_str());
        m_dfTide0 = dfVal;
    }
    if(MOOSGetValueFromToken(sParams,"EKF_MAX_Z_VEL",sVal))
    {
        double dfVal = atof(sVal.c_str());
        m_dfMaxZVel = dfVal;
    }



    m_pTracked->m_State.m_dfX = m_dfX0;
    m_pTracked->m_State.m_dfY = m_dfY0;
    m_pTracked->m_State.m_dfZ = m_dfZ0;
    m_pTracked->m_State.m_dfH = m_dfH0;
    
    m_pTracked->m_State.m_dfPX = m_dfPxx0;
    m_pTracked->m_State.m_dfPY = m_dfPyy0;
    m_pTracked->m_State.m_dfPZ = m_dfPzz0;
    m_pTracked->m_State.m_dfPH = m_dfPhh0;
    
    m_pTracked->m_State.m_dfPXdot = 0.0;
    m_pTracked->m_State.m_dfPYdot = 0.0;
    m_pTracked->m_State.m_dfPZdot = 0.0;
    m_pTracked->m_State.m_dfPHdot = 0.0;



    m_pStore->m_sOwnerName="EKF";
    
    m_nUpdates = 0;
    
    return true;
}


bool CMOOSNavEKFEngine::AddData(const CMOOSMsg &Msg)
{
    
    //    if(Msg.m_sKey=="LBL_TOF")
    //    {
    //        MOOSTrace("EKF gets LBL\n");
    //    }
    return CMOOSNavEngine::AddData(Msg);
}


bool CMOOSNavEKFEngine::Iterate(double dfTimeNow)
{
    
    //not allowed to run if not Online!
    if(!IsOnline())
        return true;
    
    if(dfTimeNow-m_dfLastIterated<EKF_ITERATE_PERIOD)
        return true;
    
    m_dfLastIterated = dfTimeNow;
    
    //keep a copy of the time..
    m_dfTimeNow = dfTimeNow;
    
    if(!IsBooted())
    {
        return Boot();
    }
    else
    {
        
        //what time should we stop at?
        double dfTStop = m_dfTimeNow-m_dfLag;
        
        double dfNewestObsTime = m_pStore->GetNewestObsTime();
        
        if(dfNewestObsTime<=m_dfLastUpdate)
        {
            //predict only ..no new observations have come in...
            double dfDeltaT = dfTStop-m_dfLastUpdate;
            if(!DoPredict(dfDeltaT))
                return false;    
        }
        else
        {
            //get observations in small batches that are close to
            //each other...
            
            double dfDT = dfTStop-m_dfLastUpdate;
            
            int nIterations = 0;
            while(dfDT>TIME_SLICE)
            {
                //split up the whole period into even sized chunks
                dfDT=(dfTStop-m_dfLastUpdate)/(++nIterations);
            }
            
            //for each of these chuncks predict then look to update...
            for(int nIteration = 0;nIteration<nIterations;nIteration++)
            {
                if(!DoPredict(dfDT))
                    return false;
                
                m_Observations.clear();
                
                double dfT1 = m_dfLastUpdate+nIteration*dfDT;
                double dfT2 = dfT1+dfDT;
                
                //get the observations (close to each other in time)
                //MOOSTrace("EKF @ %.3f true time = %.3f\n",dfTimeNow,MOOSTime());
                if(m_pStore->GetObservationsBetween(m_Observations,dfT1,dfT2))
                {
                    if(!m_Observations.empty())
                    {
                        
                        //TraceObservationSet();
                        
                        if(nIteration==0)
                        {
                            //here we add in the fixed observations....
                            m_Observations.insert(    m_Observations.begin(),
                                m_FixedObservations.begin(),
                                m_FixedObservations.end());
                        }
                        
                        //limit the number of certain kinds of observations
                        LimitObservations(CMOOSObservation::DEPTH,1);
                        LimitObservations(CMOOSObservation::YAW,1);
                        
                        bool bDone = false;

                        //safety - we should always be removeing at least one observation
                        //per DA loop so limit the number of times we can do this!
                        int nDACycles = m_Observations.size();
                        while(!bDone && (nDACycles--)>0)
                        {
                            if(MakeObsMatrices())
                            {
                                
                                //calcuate innovation covariance
                                try
                                {
                                    if(m_Innov.Nrows()<0)
                                    {
                                        //exit condition for DA
                                        bDone = true;
                                        continue;
                                    }

                                    //Kalman equation for innovation covariance
                                    m_S = m_jH*m_Phat*m_jH.t() + m_R;

                                    RecordObsStatistics(&m_Innov,&m_S);
                                    
                                    //do data rejection...
                                    bool bAccepted = true;
                                    if(MOOSChiSquaredTest(m_Innov,m_S,false))
                                    {
                                        Matrix W = m_Phat*m_jH.t()*m_S.i();
                                        m_Phat-=W*m_S*W.t();
                                        m_Xhat+=W*m_Innov;

                                        //complete....
                                        bDone = true;   
                                    }
                                    else
                                    {
                                        if(m_Innov.Nrows()>1)
                                        {
                                            //lets figure out which observation was hurting us...
                                            Matrix Si = m_S.i();
                                            int iReject = HyperDimSelect(m_Innov,m_S,Si);
                                            
                                            //now we need to figure out what observation that was
                                            //and set its bGoodDA to false!
                                            OBSLIST::iterator t;
                                            for(t = m_Observations.begin();t!=m_Observations.end();t++)
                                            {
                                                if(t->m_bIgnore==false && t->m_nRow==iReject)
                                                {
                                                    t->SetGoodDA(false);
                                                    MOOSTrace("Hyperdim Select removes obs %d \n",
                                                                t->GetID());
                                                    t->Trace();
                                                    break;
                                                }
                                            }
                                        }
                                    }                                    
                                }
                                catch(...)
                                {
                                    AddToOutput("Trouble:EKF inversion failed!\n");
                                    MOOSMatrixTrace(m_jH,"H");
                                }
                            }
                        }
                        m_nUpdates++;
                                                    
                        LogObservationSet((dfT1+dfT2)/2,
                            m_nUpdates);

                    }
                }
            }
        }        
        
        OnIterateDone(dfTStop);
    }
    return true;
}

bool CMOOSNavEKFEngine::DoPredict(double dfDeltaT)
{
    
    if(!PreparePredictionMatrices())
        return false;
    
    if(!m_pTracked->FillModelMatrices(m_jF,m_jQ,m_Xhat,dfDeltaT))
        return false;
    
    if(!FillGlobalParamModelMatrices(dfDeltaT))
        return false;
    
    //////////////////////////////////////////
    //  KALMAN PREDICTION 
    //
    // note that in this simple case we have 
    // a linear model change this later
    //
    //////////////////////////////////////////
    
    //    MOOSMatrixTrace(m_jF,"F");
    //    MOOSMatrixTrace(m_jQ,"Q");
    
    
    m_Xhat = m_jF*m_Xhat;
    m_Phat = m_jF*m_Phat*(m_jF.t()) + m_jQ;
    
    
    return true;
}

bool CMOOSNavEKFEngine::FillGlobalParamModelMatrices(double dfDeltaT)
{
    //firstly do tide
    m_jF(TIDE_STATE_INDEX,TIDE_STATE_INDEX) = 1;
    m_jQ(TIDE_STATE_INDEX,TIDE_STATE_INDEX) = pow(2e-4,2); //noise on tide...~ 9m in 12 hours
    
    if(m_bEstimateHeadingBias)
    {
        //optionally do heading bias
        m_jF(HEADING_BIAS_STATE_INDEX,HEADING_BIAS_STATE_INDEX) = 1;
        m_jQ(HEADING_BIAS_STATE_INDEX,HEADING_BIAS_STATE_INDEX) = pow(m_dfYawBiasStd,2); 
    }

    return true;
}


bool CMOOSNavEKFEngine::Boot()
{
    CMOOSNavVehicle * pVeh = dynamic_cast< CMOOSNavVehicle* > (m_pTracked);
    
    if(pVeh==NULL)
        return false;
    
    //set up vehcile mobility/dynamics..
    pVeh->SetDynamics(m_dfXYDynamics,m_dfZDynamics,m_dfYawDynamics);
    
    //set up initial conditions
    InitialiseEstimates();
    
    //start the clock
    m_dfLastUpdate = m_dfTimeNow;
    
    m_bBooted = true;
    
    return m_bBooted;
}


bool CMOOSNavEKFEngine::InitialiseEstimates()
{
    
    m_Phat = 0;
    m_Xhat = 0;
    
    //do tide first
    m_Phat(TIDE_STATE_INDEX,TIDE_STATE_INDEX) = m_dfPTide0;
    m_Xhat(TIDE_STATE_INDEX,1) = m_dfTide0;
    
    
    
    
    m_pTracked->RefreshStateVector();
    
    m_pTracked->RefreshStateCovariance();
       
    
    return true;
}

bool CMOOSNavEKFEngine::IsBooted()
{
    return m_bBooted;
}

bool CMOOSNavEKFEngine::PreparePredictionMatrices()
{
    if(m_jF.Nrows()!=m_Xhat.Nrows() || m_jF.Ncols()!=m_Xhat.Nrows())
    {
        m_jF.ReSize(m_Xhat.Nrows(),m_Xhat.Nrows());
    }
    
    if(m_jQ.Nrows()!=m_Xhat.Nrows() || m_jQ.Ncols()!=m_Xhat.Nrows())
    {
        m_jQ.ReSize(m_Xhat.Nrows(),m_Xhat.Nrows());
    }
    
    //zero all matrices before we start
    m_jQ = 0;
    m_jF = 0;
    
    return true;
}

bool CMOOSNavEKFEngine::OnIterateDone(double dfUpdateTime)
{
    //remeber our success!
    m_dfLastUpdate = dfUpdateTime;
    
    //limit velocities
    LimitVelocityStates();
    
    //look after PI
    WrapAngleStates();
    
    MakeSymmetric();
    
    //copy estimates 
    m_XTmp = m_Xhat;
    m_PTmp = m_Phat;
    
    
    //predict forwards
    PredictForward(dfUpdateTime,m_dfTimeNow);
    
    //we won;t predict the covariance forwads for siplay purposes
    m_Phat = m_PTmp;
    
    //look after PI
    WrapAngleStates();
    
    if(m_pTracked->RefreshState())
    {
        m_nIterations++;
        PublishResults();
        
        m_Logger.LogState(m_dfTimeNow,m_Xhat,m_Phat);
    }
    
    //copy estimate back (things went really well we want to
    //keep our results...
    m_Xhat = m_XTmp;


    //now we keep statistics on observations
    
    
    
    return true;
}

bool CMOOSNavEKFEngine::PublishResults()
{
    //MOOSMatrixTrace(m_Phat,"PHat");

//    TraceDiagPhat();

    //Ok so lets make some results..
    CMOOSMsg MsgX(MOOS_NOTIFY,"EKF_X", m_pTracked->m_State.m_dfX,m_dfTimeNow);
    m_ResultsList.push_front(MsgX);
    
    CMOOSMsg MsgY(MOOS_NOTIFY,"EKF_Y", m_pTracked->m_State.m_dfY,m_dfTimeNow);
    m_ResultsList.push_front(MsgY);
    
    CMOOSMsg MsgZ(MOOS_NOTIFY,"EKF_Z", m_pTracked->m_State.m_dfZ,m_dfTimeNow);
    m_ResultsList.push_front(MsgZ);
    
    CMOOSMsg MsgYaw(MOOS_NOTIFY,"EKF_YAW", m_pTracked->m_State.m_dfH,m_dfTimeNow);
    m_ResultsList.push_front(MsgYaw);
    
    CMOOSMsg MsgDepth(MOOS_NOTIFY,"EKF_DEPTH", m_pTracked->m_State.m_dfDepth,m_dfTimeNow);
    m_ResultsList.push_front(MsgDepth);
    
    CMOOSMsg MsgTide(MOOS_NOTIFY,"EKF_TIDE", m_Xhat(TIDE_STATE_INDEX,1),m_dfTimeNow);
    m_ResultsList.push_front(MsgTide);
    
    if(m_pTracked->GetEntityType()==CMOOSNavEntity::POSE_AND_RATE)
    {
        CMOOSMsg MsgX(MOOS_NOTIFY,"EKF_X_VEL", m_pTracked->m_State.m_dfXdot,m_dfTimeNow);
        m_ResultsList.push_front(MsgX);
        
        CMOOSMsg MsgY(MOOS_NOTIFY,"EKF_Y_VEL", m_pTracked->m_State.m_dfYdot,m_dfTimeNow);
        m_ResultsList.push_front(MsgY);
        
        CMOOSMsg MsgZ(MOOS_NOTIFY,"EKF_Z_VEL", m_pTracked->m_State.m_dfZdot,m_dfTimeNow);
        m_ResultsList.push_front(MsgZ);
        
        CMOOSMsg MsgYaw(MOOS_NOTIFY,"EKF_YAW_VEL", m_pTracked->m_State.m_dfHdot,m_dfTimeNow);
        m_ResultsList.push_front(MsgYaw);
        
        CMOOSMsg MsgSpeed(MOOS_NOTIFY,"EKF_SPEED", m_pTracked->m_State.m_dfSpeed,m_dfTimeNow);
        m_ResultsList.push_front(MsgSpeed);
        
    }

    //have we been estiamting bias on heading?
    if(this->m_bEstimateHeadingBias)
    {
        CMOOSMsg MsgBias(MOOS_NOTIFY,"EKF_YAW_BIAS", m_Xhat(HEADING_BIAS_STATE_INDEX,1),m_dfTimeNow);
        m_ResultsList.push_front(MsgBias);
        
    }
    
    
    string sCov = MOOSFormat("SigmaX = %f SigmaY = %f",
        sqrt(m_pTracked->m_State.m_dfPX),
        sqrt(m_pTracked->m_State.m_dfPY));
    
    CMOOSMsg MsgCov(MOOS_NOTIFY,"EKF_SIGMA",sCov.c_str(),m_dfTimeNow);
    m_ResultsList.push_front(MsgCov);

    DoObservationStatistics();
    
    if(m_nIterations%100==0)
    {
        string sResult = MOOSFormat("EKF Completes iteration %d",m_nIterations);
        AddToOutput(sResult);

        DoObservationSummary();
    }
    


    return true;
}

bool CMOOSNavEKFEngine::PredictForward(double dfStop, double dfTimeNow)
{
    //so have stopped at time dfStop but want a result at time dfTimeNow
    //->Forward Predict
    double dfDT = dfTimeNow-dfStop;
    
    //MOOSTrace("Forward Predicting by %7.2f seconds:\n",dfDT); 
    
    return DoPredict(dfDT);
    
}

bool CMOOSNavEKFEngine::MakeSymmetric()
{
    for(int i = 1;i<m_Phat.Nrows();i++)
    {
        for(int j = i;j<m_Phat.Nrows();j++)
        {
            m_Phat(i,j) = m_Phat(j,i);
        }
    }
    
    return true;
}

bool CMOOSNavEKFEngine::Reset()
{
    
    m_bBooted = false;
    
    //try to reboot....
    Boot();
    
    if(m_bBooted)
    {
        AddToOutput("EKF Reset: OK");
    }
    else
    {
        AddToOutput("EKF Reset: FAILED");
    }
    
    return m_bBooted;
}

bool CMOOSNavEKFEngine::LimitVelocityStates()
{
    
    
    if(m_pTracked->GetEntityType()==CMOOSNavEntity::POSE_AND_RATE)
    {
        double dfXdot = (*m_pTracked->m_pXhat)(m_pTracked->m_nStart+iiXdot,1);
        if(fabs(dfXdot)>MAX_ALLOWED_LIN_VELOCITY)
        {
            AddToOutput("EKF X Vel is too large (divergence?)-> resetting");
            (*m_pTracked->m_pXhat)(m_pTracked->m_nStart+iiXdot,1)=0;
        }
        
        double dfYdot = (*m_pTracked->m_pXhat)(m_pTracked->m_nStart+iiYdot,1);
        if(fabs(dfYdot)>MAX_ALLOWED_LIN_VELOCITY)
        {
            AddToOutput("EKF Y Vel is too large (divergence?)-> resetting");
            (*m_pTracked->m_pXhat)(m_pTracked->m_nStart+iiYdot,1)=0;
        }
        
        double dfZdot = (*m_pTracked->m_pXhat)(m_pTracked->m_nStart+iiZdot,1);
        if(fabs(dfZdot)>m_dfMaxZVel)
        {
            if(m_dfMaxZVel>0)
            {
                AddToOutput("EKF Z Vel is too large (divergence?)-> resetting");
            }
            (*m_pTracked->m_pXhat)(m_pTracked->m_nStart+iiZdot,1)=0;
        }
        
        double dfHdot = (*m_pTracked->m_pXhat)(m_pTracked->m_nStart+iiHdot,1);
        if(fabs(dfHdot)>MAX_ALLOWED_ANG_VELOCITY)
        {
            AddToOutput("EKF YAW Vel is too large (divergence?)-> resetting");
            (*m_pTracked->m_pXhat)(m_pTracked->m_nStart+iiHdot,1)=0;
        }
        
    }
    
    return true;
}


/** We want to find the components of v that is most responsible for us being outside the 
innovation covariance ellpsoid ( scaled by Chi squared test point as v'Si v = e is scaled version
of v'Si v = 1 - all axes scaled by 1/sqrt(e) )
This method finds the intesection of the innovation vector with the normalised ellipsoid (shapes are identical)
then determines the component that is most responsible for the length of the vecotr between
innovation and point of intersection */
int  CMOOSNavEKFEngine::HyperDimSelect(Matrix & Innov,Matrix & Cov,Matrix & InvCov)
{
    int nDimension = Innov.Nrows();
    
    Matrix    ZUpper    = Innov;
    Matrix    ZLower    = Innov;
    ZLower    = 0.0;
    Matrix    ZTmp    = ZLower;
    
    //make sure Zupper is outside
    if(IsInside(ZUpper,InvCov))
    {
        return -1;
    }
    
    
    
    Matrix  Tmp        = (ZUpper-ZLower);
    Matrix    Tmp2    = Tmp.t()*Tmp;
    double Dist = sqrt(Tmp2(1,1));
    
    int nIt = 0;
    while(Dist>0.04)
    {
        ZTmp = (ZUpper+ ZLower)/2;
        
        if(IsInside(ZTmp,InvCov))
        {
            ZLower = ZTmp;
        }
        else
        {
            ZUpper = ZTmp;
        }
        
        //vector between bracing solutions
        Tmp = (ZUpper-ZLower);
        Tmp2 = Tmp.t()*Tmp;
        
        //length of vector
        Dist = sqrt(Tmp2(1,1));
        
        if(++nIt>1000)
        {
            return -1;
        }
        
    }
    
    //////////////////////////////////////////////////////
    //
    //    OK so ZTmp is now the intersection of
    //    the hyper ellpsoid with the innovation vector
    //    find the maximum component ratios
    //
    
    
    double MaxRatio =0.0;
    int        iReject = -1;
    for(int i = 1; i<=nDimension;i++)
    {
        double dfTmp =fabs((Innov(i,1)-ZTmp(i,1))/Cov(i,i));
        
        if(dfTmp>=MaxRatio)
        {
            MaxRatio = dfTmp;
            //this obs to be rejected..
            iReject = i;
        }
    }
    
    
    return iReject;
    
}
/** Simple function returning true if vector v is inside ellipsoid defined by Ellipse*/
bool CMOOSNavEKFEngine::IsInside(Matrix &v, Matrix &Ellipse)
{
    Matrix Tmp = v.t()*Ellipse*v;
    if(Tmp(1,1) <1.0)
        return true;
    else
        return false;
}


