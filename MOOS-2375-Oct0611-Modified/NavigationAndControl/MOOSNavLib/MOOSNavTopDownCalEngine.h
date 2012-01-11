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
// MOOSNavTopDownCalEngine.h: interface for the CMOOSNavTopDownCalEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVTOPDOWNCALENGINE_H__DCD111D4_671C_47EC_B704_680F9BAE8C14__INCLUDED_)
#define AFX_MOOSNAVTOPDOWNCALENGINE_H__DCD111D4_671C_47EC_B704_680F9BAE8C14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSNavLSQEngine.h"
//#include "TopDownCalibrationController.h"    // Added by ClassView

class CMOOSNavTopDownCalEngine : public CMOOSNavLSQEngine  
{
public:
    bool SetFocus(int nChannel);

    enum State
    {
        GATHERING,
        THINKING,
        OFFLINE,
    };
    class CVantagePoint
    {
    public:
        double m_dfX;
        double m_dfY;
        double m_dfZ;
        double m_dfTOF;
        double m_dfTOFStd;
        double m_dfTime;
        int    m_nChan;
    
        CMOOSNavSensor*  m_pRespondingSensor;
        CMOOSNavSensor*  m_pInterrogateSensor;
    };
    typedef list<CVantagePoint> VANTAGEPOINT_LIST;

    CMOOSNavTopDownCalEngine();
    virtual ~CMOOSNavTopDownCalEngine();
    virtual bool Initialise(STRING_LIST  sParams);
    virtual bool AddData(const CMOOSMsg &Msg);
    virtual bool Iterate(double dfTimeNow);


protected:
    double GetRequiredPathLength();
    bool Calculate(double dfTimeNow);
    bool SeedSolution();
    bool IndicateGatherProgress();
    bool OnStart();
    string GetStateAsString(State eState);
    bool SetState(State eState);
    bool OnRxTopDownControl(string sInstruction);
    State  m_eState;
    bool Clean();
    bool OnIterateDone();
    bool MakeObservations();
    bool MakePseudoBeacons();
    bool MakeVantagePoints();
    bool GetXYZ(double  & dfX, double & dfY, double & dfZ, double dfTime, double dfTolerance);
    virtual bool OnSolved();
    double GetPathLength();
    map<int,int> m_GuessedDepths;

    string m_sJobName;

    int m_nNoConvergenceCounter;
    double m_dfCalPathLength;
    int m_nSelectedChan;

    //control of search for solution
    double m_dfLastSolveAttempt;
    double m_dfSpacing;
    double m_dfTrialRate;

    VANTAGEPOINT_LIST m_VantagePoints;
    double m_dfTide;
    int m_nActiveChannel; 
};

#endif // !defined(AFX_MOOSNAVTOPDOWNCALENGINE_H__DCD111D4_671C_47EC_B704_680F9BAE8C14__INCLUDED_)
