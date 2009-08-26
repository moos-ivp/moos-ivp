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

#if !defined(AFX_MOOSNAVIGATOR_H__E3474E24_E05C_4DEA_B39F_160D7BE246E6__INCLUDED_)
#define AFX_MOOSNAVIGATOR_H__E3474E24_E05C_4DEA_B39F_160D7BE246E6__INCLUDED_


#include "MOOSPriorityInput.h"    // Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>

class CMOOSNavEngine;

class CMOOSNavEKFEngine;

class CMOOSNavLSQEngine;

typedef std::list<CMOOSNavEngine * > NAVENGINE_LIST;
typedef    std::list<CMOOSPriorityInput *> PRIORITYINPUT_LIST;

class CMOOSNavigator : public CMOOSApp  
{
public:

    class CFilterSafety
    {
    public:
        double GetMedianLSQShift();
        bool IsLSQNoisey();
        bool SetLSQSolution(double dfX,double dfY,double dfZ,double dfH,double dfTime);
        bool Initialise();
        CFilterSafety();
        double m_dfLSQTimeOut;
        double m_dfMaxEKFLSQDeviation;
        double m_dfMaxEKFPositionUncertainty;

        //EKF iteration at which last disagreement between
        //EKF and LSQ was detected
        int m_nLastEKFDisagreeIteration;

        //LSQ iteration at which last disagreement between
        //EKF and LSQ was detected
        int m_nLastLSQDisagreeIteration;

        //total number of disagreements since last agreement
        int m_nEKFLSQDisagreements;

        //the number of disagreements that can be tolerated before 
        //the LSQ reboots the EKF
        int m_nForceEKFAfterNDisagreements;

        //the number of LSQ position fixes to average over..
        int m_nLSQSampleSize;

        //the time of the last LSQ update we know about..
        double m_dfLastLSQUpdate;

        std::list<double> m_DeltaLSQHistory;

        //maximum median shift over history of LSQ poses
        double m_dfMaxMedianLSQShift;

        double m_dfLastLSQX;
        double m_dfLastLSQY;
        double m_dfLastLSQZ;
        double m_dfLastLSQH;

    };

    bool MonitorFilters();


    CMOOSNavigator();
    virtual ~CMOOSNavigator();


    /** virtual overide of base class CMOOSApp member. Here we do all the processing and IO*/
    bool Iterate();

    /** virtual overide of base class CMOOSApp member. Here we register for data we wish be
    informed about*/
    bool OnConnectToServer();

    bool OnStartUp();

    bool OnNewMail(MOOSMSG_LIST &NewMail);






protected:
    bool Initialise();
    bool MakeSubscriptions();
    double GetTimeNow();
    bool OnNavFailure(const std::string & sReason);
    bool Clean();
    bool OnNavRestart();
    bool HandlePersonalMail(MOOSMSG_LIST & NewMail);
    bool AddFixedObservations();
    bool AddAcousticsToEngines();
    bool MakeNavEngines();
    bool AddSensorsToEngines();
    bool SetUpNavEngines();
    bool ManageInputs();
    bool PublishData();
    CMOOSPriorityInput m_XInput;
    CMOOSPriorityInput m_YInput;
    CMOOSPriorityInput m_ZInput;
    CMOOSPriorityInput m_YawInput;
    CMOOSPriorityInput m_DepthInput;
    CMOOSPriorityInput m_AltitudeInput;
    CMOOSPriorityInput m_PitchInput;
    CMOOSPriorityInput m_SpeedInput;
    CMOOSPriorityInput m_HeadingInput;
    CMOOSPriorityInput m_OdometryInput;
    CMOOSPriorityInput m_PoseInput;
    



    PRIORITYINPUT_LIST m_InputsList;

    STRING_LIST m_SubScriptions;

    NAVENGINE_LIST m_NavEngines;

    CMOOSNavEKFEngine * m_pEKF;

    CMOOSNavLSQEngine * m_pLSQ;

    CFilterSafety m_FilterSafety;

};

#endif // !defined(AFX_MOOSNAVIGATOR_H__E3474E24_E05C_4DEA_B39F_160D7BE246E6__INCLUDED_)
