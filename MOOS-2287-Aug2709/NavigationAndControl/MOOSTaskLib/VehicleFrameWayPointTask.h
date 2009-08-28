/*
 *  VehicleFrameWayPointTask.h
 *  MOOS
 *
 *  Created by pnewman on 27/03/2008.
 *  Copyright 2008 University of Oxford. All rights reserved.
 *
 */
#include <MOOSGenLib/MOOSGenLib.h>
#include "MOOSBehaviour.h"
#include <MOOSUtilityLib/InterpBuffer.h>



class CVehicleFrameWayPointTask : public CMOOSBehaviour
{
public:
    CVehicleFrameWayPointTask();
    virtual ~CVehicleFrameWayPointTask();
    
protected:
    virtual bool SetParam(string sParam, string sVal);
    virtual bool GetRegistrations(STRING_LIST &List);
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Run(CPathAction &DesiredAction);
    virtual bool RegularMailDelivery(double dfTimeNow);
    bool OnNewTargetWayPoint(const std::string & sVal ,double dfMsgTime);
    bool ValidData();
    double GetDistanceToGo();
    
    class CSE2Pose
    {
	public:
    	double X;
        double Y;
        double A;
    };
    
    
    CSE2Pose Compose(const CSE2Pose & Xab,const CSE2Pose & Xbc)
    {
        CSE2Pose Xac;
        
        double dfCab = cos(Xab.A);
        double dfSab = sin(Xab.A);
        
        Xac.X = Xab.X+Xbc.X*dfCab-Xbc.Y*dfSab;
        Xac.Y = Xab.Y+Xbc.X*dfSab+Xbc.Y*dfCab;
        Xac.A = MOOS_ANGLE_WRAP(Xab.A+Xbc.A);
        
        return Xac;    
        
    }
    CSE2Pose Inverse(const CSE2Pose &Xab)
    {
        CSE2Pose Xba;
        double ct = cos(Xab.A);
        double st = sin(Xab.A);
        Xba.X     =  -ct*Xab.X - st*Xab.Y;
        Xba.Y     = +st*Xab.X - ct*Xab.Y;
        Xba.A     = -Xab.A;
        return Xba;
    }
    
    
    class CSE2PoseInterpolator
    {
        typedef std::pair<double,CSE2Pose> Pair;
    public:
        CSE2Pose operator()(const Pair &loPair, const Pair &hiPair, double dfInterpTime) const
        {   
            const CSE2Pose &lo = loPair.second;
            const CSE2Pose &hi = hiPair.second;
            CSE2Pose NewPose;  
            
            double dt = (hiPair.first - loPair.first);    
            double alpha = 0.0;    
            
            if ( dt != 0.0 ) 
                alpha = (dfInterpTime - loPair.first) / dt; 
            
            NewPose.X = alpha*hi.X + (1-alpha)*lo.X; 
            NewPose.Y = alpha*hi.Y + (1-alpha)*lo.Y;
            NewPose.A = alpha*hi.A + (1-alpha)* lo.A;
            
            double dfAngDiff = MOOS_ANGLE_WRAP(hi.A - lo.A);
            NewPose.A = lo.A + alpha*dfAngDiff; 
            
            return NewPose;
        }
    };
    
    
    TInterpBuffer<double, CSE2Pose,CSE2PoseInterpolator > m_PoseHistory;
    
    //name of message indicating current pose
    std::string m_sPoseSource;
    //name of message indicating  current goal
    std::string m_sTargetWayPoint;
    //name of message indicating desired thrust
    std::string m_sDesiredThrustSource;
    
    double m_dfVicinityRadius;
    
    double m_dfDesiredThrust;
    
    bool m_bThrustSet;
    bool m_bWayPointSet;
    ControlledDOF m_YawDOF;
    ControlledDOF m_XDOF;
    ControlledDOF m_YDOF;
    
    
};

