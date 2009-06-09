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
// MOOSNavTopDownCalEngine.cpp: implementation of the CMOOSNavTopDownCalEngine class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include "MOOSNavLibGlobalHelper.h"
#include "MOOSNavObsStore.h"
#include "MOOSNavBeacon.h"
#include "MOOSNavVehicle.h"
#define FAILED_CONVERGENCE_LIMIT 4
#define DEFAULT_MANUAL_PATH_LENGTH 100.0
#include "MOOSNavTopDownCalEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSNavTopDownCalEngine::CMOOSNavTopDownCalEngine()
{
    m_nSelectedChan = 3;
    m_eState = OFFLINE;
    m_sJobName = "TOPDOWNCALIBRATION";
    m_nNoConvergenceCounter = 0;
    m_dfCalPathLength = DEFAULT_MANUAL_PATH_LENGTH;

    m_dfTide = 0;

    m_sName = "TopDown";

    m_dfLastSolveAttempt = -1;

    m_bInitialOnline = false;

    //how often do we try to solve?
    m_dfTrialRate = 4.0;

    m_dfSpacing = 1.0;
}

CMOOSNavTopDownCalEngine::~CMOOSNavTopDownCalEngine()
{

}

bool CMOOSNavTopDownCalEngine::Initialise(STRING_LIST  sParams)
{

    
    if(CMOOSNavLSQEngine::Initialise(sParams))
    {
        m_pStore->SetSpan(200);
    }
    
    //we never use heading bias state in TDC
    m_bEstimateHeadingBias = false;



    string sVal;
    
    if(MOOSGetValueFromToken(sParams,"TDC_TIDE",sVal))
    {
        double dfVal = atof(sVal.c_str());
        m_dfTide  = dfVal;

    }
    
    if(MOOSGetValueFromToken(sParams,"TDC_SPACING",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal>0)
        {
            m_dfSpacing = dfVal;
        }
    }

    if(MOOSGetValueFromToken(sParams,"TDC_TRIAL_RATE",sVal))
    {
        double dfVal = atof(sVal.c_str());
        if(dfVal>0)
        {
            m_dfTrialRate = dfVal;
        }
    }

    //initial guesses for top down
    if(MOOSGetValueFromToken(sParams,"TDC_GUESS_DEPTHS",sVal))
    {
        //TDC_GUESS_DEPTHS = 3@2.4 , 4@5.4 etc
        while(!sVal.empty())
        {
            string sBlock = MOOSChomp(sVal,",");
            string sChan = MOOSChomp(sBlock,"@");
            //store this guess depth...
            m_GuessedDepths[atoi(sChan.c_str())] = atoi(sBlock.c_str());
        }
    }
    else
    {
        AddToOutput("Warning now beacons guess depths specified\n");
    }
   
    return true;
}


bool CMOOSNavTopDownCalEngine::AddData(const CMOOSMsg &Msg)
{
    //crack all messages for control commands...
    if(Msg.m_sKey=="TOP_DOWN_CAL_CONTROL")
    {
        return OnRxTopDownControl(Msg.m_sVal);
    }
    else if(m_eState!=OFFLINE)
    {
        bool bBaseSuccess = CMOOSNavLSQEngine::AddData(Msg);
    
        return bBaseSuccess;
    }
    return true;
}

bool CMOOSNavTopDownCalEngine::Iterate(double dfTimeNow)
{
    if(!IsOnline())
        return false;

    //a fresh start on each iteration
    Clean();

    if(dfTimeNow-m_dfLastSolveAttempt>m_dfTrialRate)
    {
        m_dfLastSolveAttempt = dfTimeNow;
        
        if(MakeVantagePoints())
        {       
            //is we got to here then we are in a fit state to try and calculate...
            if(Calculate(dfTimeNow))
            {
            }
        }
        OnIterateDone();
    }

    return true;
}



bool CMOOSNavTopDownCalEngine::OnSolved()
{

    if(m_pTracked->RefreshState())
    {

        AddToOutput("Bcn(C%d)->[%7.1f,%7.1f,%7.1f]",
                                m_nSelectedChan,
                                m_pTracked->m_State.m_dfX,
                                m_pTracked->m_State.m_dfY,
                                m_pTracked->m_State.m_dfZ);
    }
    if(m_pTracked->RefreshStateCovariance())
    {
        AddToOutput("Bcn std: %7.1f,%7.1f,%7.1f",
                                sqrt(m_pTracked->m_State.m_dfPX),
                                sqrt(m_pTracked->m_State.m_dfPY),
                                sqrt(m_pTracked->m_State.m_dfPZ));



    }

    return true;    

}


bool CMOOSNavTopDownCalEngine::OnIterateDone()
{

    BEACONLIST::iterator p,q;

    for(p = m_Beacons.begin();p!=m_Beacons.end();p++)
    {
        CMOOSNavBeacon * pBcn = *p;

        if(pBcn->m_bPseudo)
        {
            delete pBcn;
            q=p;
            p++;
            m_Beacons.erase(q);
        }
    }
    
    return true;
}


bool CMOOSNavTopDownCalEngine::GetXYZ(double &dfX,
                                      double &dfY,
                                      double &dfZ,
                                      double dfTime,
                                      double dfTolerance)
{
    dfZ = 0;

    OBSLIST * pGPSX = m_pStore->GetListByType(CMOOSObservation::X);
    OBSLIST * pGPSY = m_pStore->GetListByType(CMOOSObservation::Y);

    if(pGPSX==NULL || pGPSY==NULL)
    {
        AddToOutput("TDC: Cannot locate GPS data..is iGPS running?");
        MOOSPause(2000);
        return false;
    }

    if(pGPSX->empty() || pGPSY->empty())
        return false;

    OBSLIST::iterator q,w;
    w = pGPSX->end();
    double dfMinDT = 1e9;
    for(q = pGPSX->begin();q!=pGPSX->end();q++)
    {
        double dfDT =fabs((*q).m_dfTime -dfTime);
        if( dfDT<dfTolerance && dfDT<dfMinDT)
        {
            w = q;
            dfMinDT = dfDT;
        }
    }
    if(w == pGPSX->end())
    {
        return false;
    }
    dfX = (*w).m_dfData;



    dfMinDT = 1e9;
    w = pGPSY->end();
    for(q = pGPSY->begin();q!=pGPSY->end();q++)
    {
        double dfDT =fabs((*q).m_dfTime -dfTime);
        if( dfDT<dfTolerance && dfDT<dfMinDT)
        {
            w = q;
            dfMinDT = dfDT;
        }
    }

    if(w == pGPSY->end())
    {
        return false;
    }
    dfY = (*w).m_dfData;


    return true;
}


double CMOOSNavTopDownCalEngine::GetPathLength()
{
    
    if(m_VantagePoints.size()<2)
        return 0;

    double dfXLast = m_VantagePoints.front().m_dfX;
    double dfYLast = m_VantagePoints.front().m_dfY;
    double dfXNow = 0;
    double dfYNow = 0;
    double dfS = 0;

    VANTAGEPOINT_LIST::iterator p;

    for(p = m_VantagePoints.begin();p!=m_VantagePoints.end();p++)
    {
        CVantagePoint & rVP = *p;

        dfXNow = p->m_dfX;
        dfYNow = p->m_dfY;

        dfS+=sqrt(pow(dfXNow-dfXLast,2)+pow(dfYNow-dfYLast,2));

        dfXLast = dfXNow;
        dfYLast = dfYNow;

    }

//    MOOSTrace("Paths Length = %f\n",dfS);
    return dfS;
}


bool CMOOSNavTopDownCalEngine::MakeVantagePoints()
{
    OBSLIST * pAcoustic = m_pStore->GetListByType(CMOOSObservation::LBL_BEACON_2WR);

    if(pAcoustic==NULL)
        return false;

    if(pAcoustic->size()<10)
        return true;

    OBSLIST::iterator p;

    //build a list using only the observations on our selected
    //channel...
    double dfX,dfXOld=0;
    double dfY,dfYOld=0;
    double dfZ;

    
    for(p = pAcoustic->begin();p!=pAcoustic->end();p++)
    {
        CMOOSObservation & rObs = *p;
        
        if(rObs.m_nChan==m_nSelectedChan)
        {

            if(GetXYZ(dfX,dfY,dfZ,rObs.m_dfTime,0.5))
            {

                double dfS = sqrt(pow(dfX-dfXOld,2)+pow(dfY-dfYOld,2));

                if(dfS>m_dfSpacing || p == pAcoustic->begin() )
                {
                    CVantagePoint VP;
                
                    VP.m_dfX = dfX;
                    VP.m_dfY = dfY;
                    VP.m_dfZ = 0;

                    VP.m_dfTOF = rObs.m_dfData;
                    VP.m_dfTOFStd = rObs.m_dfDataStd;
                    VP.m_dfTime = rObs.m_dfTime;
                    VP.m_pInterrogateSensor = rObs.m_pInterrogateSensor;

                    m_VantagePoints.push_back(VP);

                    dfXOld = dfX;
                    dfYOld = dfY;
                }
            }
        }
    }
    
    return !m_VantagePoints.empty();
}

bool CMOOSNavTopDownCalEngine::MakePseudoBeacons()
{
    //need a certain number to be able to solve in any case..
    if(m_VantagePoints.size()<3)
    {
        return false;
    }

    CMOOSNavBeacon* pB = GetBeaconByChannel(m_nSelectedChan);
    double dfTAT = pB->m_dfTAT;

    VANTAGEPOINT_LIST::iterator p;

    int nBcn = 0;
    for(p = m_VantagePoints.begin();p!=m_VantagePoints.end();p++)
    {
        CVantagePoint & rVP = *p;
                
        string sName = MOOSFormat("B%d[%7.2f,%7.2f]",
                nBcn++,
                rVP.m_dfX,
                rVP.m_dfY);
        
        AddAcousticBeacon(  sName,
                            m_nSelectedChan,
                            dfTAT,
                            rVP.m_dfX,
                            rVP.m_dfY,
                            0);

        //fetch newly added beacons...
        CMOOSNavBeacon*  pNewBeacon = GetBeaconByName(sName);
        pNewBeacon->m_bPseudo = true;

        if(pNewBeacon)
        {
            rVP.m_pRespondingSensor = pNewBeacon->GetSensorByType(CMOOSNavSensor::LBL);
        }
    
    }

    return true;
}

bool CMOOSNavTopDownCalEngine::MakeObservations()
{
    m_Observations.clear();
    m_FixedObservations.clear();

    VANTAGEPOINT_LIST::iterator p;
    for(p = m_VantagePoints.begin();p!=m_VantagePoints.end();p++)
    {
        CVantagePoint & rVP = *p;
        CMOOSObservation NewObs;
        NewObs.m_eType=CMOOSObservation::LBL_BEACON_2WR;
        NewObs.m_dfTime = rVP.m_dfTime;
        NewObs.m_dfData = rVP.m_dfTOF;
        NewObs.m_dfDataStd = rVP.m_dfTOFStd;
        NewObs.m_pInterrogateSensor = rVP.m_pInterrogateSensor;
        NewObs.m_pRespondingSensor = rVP.m_pRespondingSensor;

        m_Observations.push_back(NewObs);
    }

    //finally make things observable...
    AddFixedObservation(CMOOSObservation::YAW,0,0.1);
    AddFixedObservation(CMOOSObservation::TIDE,0,0.1);

    //guess a depth...
    double dfGuessDepth = -10.0;

    map<int,int>::iterator q = m_GuessedDepths.find(m_nSelectedChan);
    if(q!=m_GuessedDepths.end())
    {
        dfGuessDepth = -(q->second);
    }
    AddToOutput("Guessing Depth of %f m\n",-dfGuessDepth);


    AddFixedObservation(CMOOSObservation::DEPTH,dfGuessDepth,2.0);

    m_Observations.insert(    m_Observations.begin(),
                            m_FixedObservations.begin(),
                            m_FixedObservations.end());

    
    return true;
}

bool CMOOSNavTopDownCalEngine::Clean()
{
    m_Observations.clear();
    m_FixedObservations.clear();
    m_VantagePoints.clear();
    m_GuessedDepths.clear();

    return true;
}


bool CMOOSNavTopDownCalEngine::SetFocus(int nChannel)
{
    m_nSelectedChan=nChannel;

    return true;
}


bool CMOOSNavTopDownCalEngine::OnRxTopDownControl(string sInstruction)
{
    MOOSToUpper(sInstruction);
    if(MOOSStrCmp(sInstruction,"STOP"))
    {
        SetOnline(false);
        SetState(OFFLINE);
        return true;
    }
    else if(MOOSStrCmp(sInstruction,"START"))
    {
        OnStart();
    
        return true;
    }
    else if(MOOSStrCmp(sInstruction,"CALCULATE"))
    {
        //ok try and calculate now....
        return Calculate(MOOSTime());    
    }
    else if(sInstruction.find("FOCUS")!=string::npos)
    {
        MOOSChomp(sInstruction,"=");
        int nNew = atoi(sInstruction.c_str());
        if(nNew!=0)
        {
            m_nActiveChannel = nNew;
            AddToOutput("Manual set of TDC focus to channel %d",nNew);
            SetFocus(nNew);
        }
    }
    else
    {
        AddToOutput("Unknown top down cal control command");
        return false;
    }

    return true;
}

bool CMOOSNavTopDownCalEngine::SetState(State eState)
{
    m_eState = eState;

    AddToOutput("Top Down Cal is %s\n",
                GetStateAsString(m_eState).c_str());



    return true;

}

string CMOOSNavTopDownCalEngine::GetStateAsString(State eState)
{
    switch(eState)
    {
        case OFFLINE: return "OFFLINE";
        case GATHERING: return "GATHERING";
        case THINKING:    return "THINKING";
    }
    return "";
}

bool CMOOSNavTopDownCalEngine::OnStart()
{
    
    //start with a clean slate...
    m_pStore->Flush();

    if(m_nSelectedChan==-1)
    {
        AddToOutput("TDC: no focus channel specified");
        return false;
    }

    //the minute we start we are gathering data...
    SetState(GATHERING);
    SetOnline(true);
    
    return true;
}


bool CMOOSNavTopDownCalEngine::IndicateGatherProgress()
{

    AddToOutput("%.1f m travelled %d observations gathered",
            GetPathLength(),
            m_Observations.size());
    
    return true;
}

bool CMOOSNavTopDownCalEngine::SeedSolution()
{
    //we will take the mean position of the vantage points
    //in and y
    
    VANTAGEPOINT_LIST::iterator p;

    double dfXMean=0;
    double dfYMean=0;

    for(p = m_VantagePoints.begin();p!=m_VantagePoints.end();p++)
    {
        CVantagePoint & rVP = *p;

        dfXMean += p->m_dfX;
        dfYMean += p->m_dfY;
    }

    dfXMean/=m_VantagePoints.size();
    dfYMean/=m_VantagePoints.size();

    m_pTracked->m_State.m_dfX = dfXMean;
    m_pTracked->m_State.m_dfY = dfYMean;
    m_pTracked->m_State.m_dfZ = -m_dfTide;


    return m_pTracked->RefreshStateVector();

}


bool CMOOSNavTopDownCalEngine::Calculate(double dfTimeNow)
{

    AddToOutput("TDC: Attempting to calculate");

    if(!MakePseudoBeacons())
        return false;

    AddToOutput("TDC: Pseudo Beacons Made OK");

    if(!MakeObservations())
        return false;

    AddToOutput("TDC: made observations");


    //send progress information..
    IndicateGatherProgress();


    //make an intial guess...
    SeedSolution();

    for(int nIteration = 0;nIteration<m_nLSQIterations;nIteration++)
    {
        if(MakeObsMatrices())
        {
            switch(Solve())
            {
            case LSQ_NO_SOLUTION:
//                StatusMessage("FAILED TO SOLVE TDC");
                MOOSTrace("FAILED TO SOLVE TDC");
                return false;
            
            case LSQ_IN_PROGRESS:
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

    //if we have spect ages trying to figure it out we had better bail
    if(m_nNoConvergenceCounter++>FAILED_CONVERGENCE_LIMIT)
    {
        AddToOutput("Could not solve for beacon (no convergence)... ");
    }

    return true;

}


double CMOOSNavTopDownCalEngine::GetRequiredPathLength()
{
    return m_dfCalPathLength;
}
