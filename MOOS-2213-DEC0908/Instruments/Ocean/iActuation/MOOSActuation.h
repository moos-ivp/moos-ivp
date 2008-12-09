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
//   This file is part of a  MOOS Instrument. 
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
// MOOSActuation.h: interface for the CMOOSActuation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSACTUATION_H__326BACD6_5396_4326_8991_B5B71E6C49AD__INCLUDED_)
#define AFX_MOOSACTUATION_H__326BACD6_5396_4326_8991_B5B71E6C49AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMOOSActuationDriver;
class CMOOSActuation : public CMOOSInstrument 
{
public:

    enum MotorName{RUDDER,ELEVATOR,THRUST};

    CMOOSActuation();
    virtual ~CMOOSActuation();

protected:
    bool OnActuationSet();
    bool Reset();
    bool HandleHWWatchDog();
    bool DoIO(MotorName Name);

    /** virtual overide of base class CMOOSApp member. Here we do all the processing and IO*/
    bool Iterate();

    /** virtual overide of base class CMOOSApp member. Here we register for data we wish be
    informed about*/
    bool OnConnectToServer();

    bool OnStartUp();

    bool OnNewMail(MOOSMSG_LIST &NewMail);



    class ActuatorDOF
        {
        public:
        double GetTimeSet();
        bool SetValue(double dfVal,double dfTime);
        double GetDesired();
        bool SetDesired(double dfVal,double m_dfTime);
        bool HasExpired();
        ActuatorDOF();
        bool IsUpdateRequired();
        
        protected:
        double m_dfVal;
        double m_dfDesired;
        double m_dfTimeSet;
        double m_dfRefreshPeriod;
        double m_dfTimeRequested;
        
        
        
        };


    class RollTransform:public ActuatorDOF
        {
        public:
        RollTransform();
        //method to transform the input rudder + elevator
                //to a desired rudder and elevator
        bool Transform(double &dfRudder, double &dfElevator);
        bool IsTransformRequired();
        void SetTransformRequired(bool bTForm);
        private:
        bool m_bTransform;

        };


    ActuatorDOF m_Motors[3];
    RollTransform m_RollTransform;
    
    CMOOSActuationDriver * m_pDriver;

    double m_dfLastRPMTime;        
    
    std::string m_sDriverType;



};

#endif // !defined(AFX_MOOSACTUATION_H__326BACD6_5396_4326_8991_B5B71E6C49AD__INCLUDED_)
