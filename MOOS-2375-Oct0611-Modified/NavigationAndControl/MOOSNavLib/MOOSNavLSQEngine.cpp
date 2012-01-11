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
// MOOSNavLSQEngine.cpp: implementation of the CMOOSNavLSQEngine class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include "MOOSNavLibGlobalHelper.h"
#include "MOOSNavObsStore.h"
#include "MOOSNavVehicle.h"
#include "MOOSNavLSQEngine.h"
#include "MOOSNavLibDefs.h"
#define UKOA_W_REJECTION 2.576
#define MINIMUM_TIME_BETWEEN_SOLUTIONS 0.5


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavLSQEngine::CMOOSNavLSQEngine()
{
    m_dfLSQWindow = 4.0;
    m_nLSQIterations = 5;
    m_bTryToSolve = false;
    m_dfLastSolveAttempt = 0;
    
    //don't  bother updateing more often than once
    //ever second..
    m_dfUpdatePeriod = 1.0;
    
    m_sName = "LSQ";
    
    
    m_bGuessed = false;
    
    m_bInitialOnline = true;
}

CMOOSNavLSQEngine::~CMOOSNavLSQEngine()
{
    
}

bool CMOOSNavLSQEngine::AddData(const CMOOSMsg &Msg)
{
    bool bBaseSuccess = CMOOSNavEngine::AddData(Msg);
    
    if(bBaseSuccess)
    {
        m_bTryToSolve = true;
    }
    
    return bBaseSuccess;
}

bool CMOOSNavLSQEngine::Iterate(double dfTimeNow)
{
    //house keeping
    if(!IterateRequired(dfTimeNow))
    {
        return false;
    }
    
    //keep a count of how many times this has been called
    if(!m_bGuessed)
    {
        GuessVehicleLocation();
        m_bGuessed = true;;
    }
    
    
    
    m_dfTimeNow = dfTimeNow;        
    
    //take a copy of m_Xhat
    m_XhatTmp = m_Xhat;
    m_PhatTmp = m_Phat;
    
    
    
    
    double m_dfCutOffTime = dfTimeNow-m_dfLSQWindow;
    
    m_Observations.clear();
    if(m_pStore->GetObservationsBetween(m_Observations,m_dfCutOffTime,dfTimeNow))
    {
        
        if(m_Observations.empty())
            return false;
        
        m_dfLastSolveAttempt = dfTimeNow;
        
        
        //here we add in the fixed observations....
        m_Observations.insert(    m_Observations.begin(),
            m_FixedObservations.begin(),
            m_FixedObservations.end());
        

        //limit the number of certain kinds of observations
        LimitObservations(CMOOSObservation::DEPTH,1);
        LimitObservations(CMOOSObservation::YAW,1);
        LimitObservations(CMOOSObservation::X,1);
        LimitObservations(CMOOSObservation::Y,1);
        
        //here is out chance to look at consistency of data..
        //PreFilterData();                

        //TraceObservationSet();

        m_Logger.Comment("LSQ Iterate Attempted",dfTimeNow);

        for(int nIteration = 0;nIteration<m_nLSQIterations;nIteration++)
        {
            if(MakeObsMatrices())
            {
                //we've just figured out innovations
                //but don't have innov std - we record
                //this data in each observation
                RecordObsStatistics(&m_Innov,NULL);

                switch(Solve())
                {
                case LSQ_NO_SOLUTION:
                    //failed update...
                    //                    MOOSTrace("No Solution!\n");                    
                    //TraceObservationSet();
                    m_Xhat = m_XhatTmp;
                    m_Phat = m_PhatTmp;
                    m_bTryToSolve = false;
                    m_Logger.Comment("No Solution",dfTimeNow);
                    return false;
                    
                case LSQ_BIG_CHANGE:
                    m_bTryToSolve = false;
                    m_Logger.Comment("Big Change In Solution - solution abandoned",dfTimeNow);
                    return false;

                case LSQ_IN_PROGRESS:
                    if(nIteration==m_nLSQIterations)
                    {
                        m_Xhat = m_XhatTmp;
                        m_Phat = m_PhatTmp;
                        m_bTryToSolve = false;
                        m_Logger.Comment("No Convergence",dfTimeNow);
                        return false;
                    }
                    break;
                    
                    
                case LSQ_SOLVED:
                    if(!DoWStatistic())
                    {
                        //we will have removed the observation
                        //that is not fitting...try to iterate again..
                        nIteration = 0;
                        break;
                    }
                    else
                    {
                        return OnSolved();
                    }                
                }
            }
        }                              
    }
    
    
    return false;
}



CMOOSNavLSQEngine::LSQStatus CMOOSNavLSQEngine::Solve()
{
    
    
    try
    {
        
        if(m_jH.Nrows()<m_Xhat.Nrows())
        {
      //      MOOSMatrixTrace(m_jH,"jH");
      //      TraceObservationSet();
            return LSQ_NO_SOLUTION;
        }
        
        Matrix L = m_jH.t()*m_R.i();
        m_Ihat = (L*m_jH);

        //TraceObservationSet();
        //MOOSMatrixTrace(m_Ihat,"I");
        m_Phat = m_Ihat.i();
        
        //does the covariance look reasonable?
        if(!IsReasonableMerit())
        {

            //un comment to see failure cases..
            //MOOSMatrixTrace(m_Phat,"Unreasonable P");
            //TraceObservationSet();
            //MOOSMatrixTrace(m_Ihat,"I");
            //MOOSMatrixTrace(m_jH,"jH");
            
            //interesting we could be in a bad spot...
            //better move..
            AddToOutput("Ill conditioned LSQ - relocating vehicle to field mean\n");
            GuessVehicleLocation();
            return LSQ_NO_SOLUTION;
        }
        
        //ok shuffle state a little bit
        Matrix dX = m_Phat*L*m_Innov;
        m_Xhat += dX;
        
        //is the state change small?
        if(MOOSChiSquaredTest(dX,m_Ihat,true))          
        {            
            //Matrix DState = m_Xhat-m_XhatTmp;
            //if(DState.MaximumAbsoluteValue()>5.0)
            //{
            //    return LSQ_BIG_CHANGE;
            //}

            m_nIterations++;
            
            return LSQ_SOLVED;
        }
        else
        {
            return LSQ_IN_PROGRESS;
        }
        
    }
    catch(...)
    {
        m_Phat.Release();
        m_Phat.CleanUp();
        return LSQ_NO_SOLUTION;
    }
    
}

bool CMOOSNavLSQEngine::IterateRequired(double dfTimeNow)
{
    //if we know we could never solve don;t bother
    //for exmple if last iteratiodn failed and no new
    //data has arrived since then..
    if(!m_bTryToSolve)
        return false;
    
    //this ets the fastest possible update rate
    //at 2HZ = reasonarble
    if(dfTimeNow-m_dfLastSolveAttempt<MINIMUM_TIME_BETWEEN_SOLUTIONS)
    {
        return false;
    }
    
    if(dfTimeNow-m_dfLastUpdate<m_dfUpdatePeriod)
    {
        return false;
    }
    
    return true;
}

void CMOOSNavLSQEngine::DoDebug()
{
/*    DBF<<m_dfTimeNow<<" "<<m_Xhat(1,1)<<" "<<m_Xhat(2,1)<<" "<<m_Xhat(3,1)<<" "<<m_Xhat(4,1)<<" ";
    
    OBSLIST::iterator p;
    
    for(p = m_Observations.begin();p!=m_Observations.end();p++)
    {
        CMOOSObservation  & rObs = *p;
        DBF<<rObs.m_dfData<<" ";
    }
    DBF<<endl;
    
*/    
}

// here we look for a reasonable PSD covariance matrix
//this is an extra check to look for results of illconditioned
//inversions
bool CMOOSNavLSQEngine::IsReasonableMerit()
{
    int nRows = m_Xhat.Nrows();
    
    for(int n = 1; n<=nRows;n++)
    {
        double dfDiag = m_Phat(n,n);
        if(dfDiag<0)
            return false;
        
        if(dfDiag>1e4)
            return false;
    }
    return true;
}

bool CMOOSNavLSQEngine::OnSolved()
{
    
    //MOOSTrace("\n\n++++ IterateLSQ @%f ++++\n",m_dfTimeNow);
    //MOOSMatrixTrace(m_Xhat,"XSolved:");
    
    //so we used these obs...
    m_pStore->MarkAsUsed(m_Observations);
    
    //our last good time was right now! :-)
    m_dfLastUpdate = m_dfTimeNow;
    
    //don't try and solve until mode data is in..
    m_bTryToSolve = false;
    
    //look after wrapped angle states..
    WrapAngleStates();
    
    if(m_pTracked->RefreshState())
    {
        PublishData();               
        LogObservationSet(m_dfTimeNow,m_nIterations);
        m_Logger.LogState(m_dfTimeNow,m_Xhat,m_Phat);
    }
    
    
    return true;
    
}

bool CMOOSNavLSQEngine::Initialise(STRING_LIST  sParams)
{
    m_bGuessed = false;
    
    if(!CMOOSNavEngine::Initialise(sParams))
        return false;
   
    //we never use heading bias state in LSQ
    m_bEstimateHeadingBias = false;

    //here we set up sensor trajectory based rejection for the LSQ filter
    //alwasy on for least square filter
    if(!SetUpSensorChannels(sParams,"REJECTION"))
        return false;

    
    return true;
}



bool CMOOSNavLSQEngine::DoWStatistic()
{
    return true;

    //now recalculate the innovation based on our present
    //estimate;
    if(!MakeObsMatrices())
        return false;
    
    //calculate innovation covariance
    Matrix S = m_R-(m_jH*m_Phat*m_jH.t());
    
    //    MOOSMatrixTrace(S,"S");
    //   MOOSMatrixTrace(m_jH,"H");
    //    MOOSMatrixTrace(m_R,"m_R");
    
    int nRows = m_Innov.Nrows();
    
    OBSLIST::iterator p,q;    
    
    q= m_Observations.end();
    
    //calculate w statistics
    double dfMaxW = 0;
    
    
    for(p= m_Observations.begin();p!=m_Observations.end();p++)
    {
        CMOOSObservation & rObs = *p;
        
        if(!rObs.IsFixed() && !rObs.m_bIgnore)
        {         
            int iRow = rObs.m_nRow;
            
            double dfSigma = sqrt(S(iRow,iRow));
            
            if(dfSigma>1e-10)
            {
                double dfResidual = m_Innov(iRow,1);
                
                double dfW = fabs(dfResidual)/dfSigma;
                
                if(dfW>dfMaxW)
                {
                    //store largest absolute value
                    q=p;
                    dfMaxW = dfW;
                }
            }
        }
    }
    
    //if we have found an observation that doesn't fit
    if(dfMaxW>UKOA_W_REJECTION)
    {
        if(q!=m_Observations.end())
        {
            CMOOSObservation & rObs = *q;
            //ignore it
            rObs.m_bIgnore = true;
            rObs.m_bGoodDA = false;
            
            MOOSTrace("Failed W test:\n");
            rObs.Trace();
            
            
            return false;
        }
    }
    
    return true;
}


bool CMOOSNavLSQEngine::PublishData()
{
    if(m_nIterations%20==0)
    {
        AddToOutput("LSQ Completes iteration %d",m_nIterations);
    }
    
    //Ok so lets make some results..
    CMOOSMsg MsgX(MOOS_NOTIFY,"LSQ_X", m_pTracked->m_State.m_dfX,m_dfTimeNow);
    m_ResultsList.push_front(MsgX);
    
    CMOOSMsg MsgY(MOOS_NOTIFY,"LSQ_Y", m_pTracked->m_State.m_dfY,m_dfTimeNow);
    m_ResultsList.push_front(MsgY);
    
    CMOOSMsg MsgZ(MOOS_NOTIFY,"LSQ_Z", m_pTracked->m_State.m_dfZ,m_dfTimeNow);
    m_ResultsList.push_front(MsgZ);
    
    CMOOSMsg MsgYaw(MOOS_NOTIFY,"LSQ_YAW", m_pTracked->m_State.m_dfH,m_dfTimeNow);
    m_ResultsList.push_front(MsgYaw);
    
    CMOOSMsg MsgDepth(MOOS_NOTIFY,"LSQ_DEPTH", m_pTracked->m_State.m_dfDepth,m_dfTimeNow);
    m_ResultsList.push_front(MsgDepth);
    
    CMOOSMsg MsgTide(MOOS_NOTIFY,"LSQ_TIDE", m_Xhat(TIDE_STATE_INDEX,1),m_dfTimeNow);
    m_ResultsList.push_front(MsgTide);
    
    return true;
}
