///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Utility Component. 
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


// SixDOFAUV.h: interface for the CSixDOFAUV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIXDOFAUV_H__135C4976_AA4F_495D_AD96_0F91C579E194__INCLUDED_)
#define AFX_SIXDOFAUV_H__135C4976_AA4F_495D_AD96_0F91C579E194__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <newmat.h>
using namespace NEWMAT;

#include <MOOSGenLib/MOOSGenLib.h>
//#include "SimEntity.h"
#include "SimVehicle.h"


class CSixDOFAUV  : public CSimVehicle
{
public:
    double GetDepth();



    bool    Initialise();

    double GetX(){    return m_Pos_e(1,1);};
    double GetY(){    return m_Pos_e(2,1);};
    double GetZ(){    return m_Pos_e(3,1);};
    double GetPitch(){    return m_Pos_e(4,1);};

    double GetHeading(){    return m_dfHeading;};
    double GetAltitude(){    return m_dfAltitude;};
    double GetYaw(){return m_Pos_e(6,1);};
    double GetZVel(){return m_Vel_e(3,1);};
    double GetBodyVelY(){return m_Vel_v(2,1);};
    double GetBodyVelX(){return m_Vel_v(1,1);};
    double GetSpeed(){return m_dfSpeed;};

    void   SetX(double dfVal){m_Pos_e(1,1) = dfVal;};
    void   SetY(double dfVal){m_Pos_e(2,1) = dfVal;};
    void   SetZ(double dfVal){m_Pos_e(3,1) = dfVal;};
    void   SetYaw(double dfVal){m_Pos_e(6,1) = dfVal;};
    void   SetDepth(double dfVal){m_dfDepth = dfVal;};
    void   SetAltitude(double dfVal){m_dfAltitude = dfVal;};
    void   SetInputPrefix(const std::string & sStr){m_sInputPrefix = sStr;};
    void   SetOutputPrefix(const std::string & sStr){m_sOutputPrefix = sStr;};

    bool FillVelJacobian();
    bool Iterate(double dfTimeNow,double dfDTime);

    CSixDOFAUV();
    virtual ~CSixDOFAUV();

    double m_dfWeight;
    double m_dfMass;
    
    double m_dfBuoyancy;

    Matrix m_Vel_v;
    Matrix m_Acc_v;
    Matrix m_M;
    Matrix m_Mi;
    Matrix m_Uf;
    Matrix m_Drag;
    Matrix m_JacVel;
    Matrix m_KDrag;
    Matrix m_Hf;
    Matrix m_COG;
    Matrix m_COB;
    double m_dfAltitude;
    double m_dfDepth;
    double m_dfTideHeight;
    double m_dfSpeed;
    double m_dfHeading; //in degrees...

//    double m_dfTimeNow;

    
    double m_dfRudder;
    double m_dfElevator;
    double m_dfThrust;



protected:
    bool DoModel(double dfDT);
};

#endif // !defined(AFX_SIXDOFAUV_H__135C4976_AA4F_495D_AD96_0F91C579E194__INCLUDED_)
