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
// MOOSObservation.h: interface for the CMOOSObservation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSOBSERVATION_H__76EA853F_4C56_49E6_A856_D98EEACADB0A__INCLUDED_)
#define AFX_MOOSOBSERVATION_H__76EA853F_4C56_49E6_A856_D98EEACADB0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <newmat.h>
using namespace NEWMAT;

#include "MOOSNavBase.h"
#include "LBLMaths.h"

class CMOOSNavVehicle;
class CMOOSNavBeacon;
class CMOOSNavSensor;

class CMOOSObservation : public CMOOSNavBase 
{

public:        
    enum Type
    {
        UNKNOWN_TYPE,
        X,
        Y,
        YAW,
        LBL_BEACON_2WR,
        DEPTH,
        ALTITUDE,
        SPEED,
        THRUST,
        BODY_VEL_Y,
        BODY_VEL_X,
        BODY_VEL_Z,
        TIDE,
        RUDDER,
        ELEVATOR,
        HEADING_BIAS
    };


    CLBLMaths m_LBLMaths;



public:
    void UsingHeadingBias(bool bUsing);
    bool SetGoodDA(bool bGoodDA);
    bool m_bGoodDA;
    bool Ignore(bool bIgnore);
    bool IsType(CMOOSObservation::Type eType);
    bool SetFixed(bool bFixed);
    bool IsFixed();
    string GetName();
    virtual void Trace();
    int GetDimension();

    bool operator > (const CMOOSObservation & Obs) const
    {
        return m_dfTime>Obs.m_dfTime;
    };
    static bool JacCallBack(    Matrix & XOut,
                                Matrix & XIn,
                                void * pParam);

    bool JacEvaluate(Matrix & XIn,Matrix & XOut);

    bool MakeMatrices(Matrix & Innov,Matrix & jH, Matrix & jR,Matrix & Xhat);
    CMOOSObservation();
    virtual ~CMOOSObservation();

    Type m_eType;

    double m_dfTime;

    ///sound velocity..
    double m_dfSV;
    
    //the actual data in the observation
    double m_dfData;

    //the actual data in the observation (part 2)
    double m_dfData2;


    //teh observation variance..
    double m_dfDataStd;

    //teh observation variance..(part 2)
    double m_dfDataStd2;


    //the channel of the acoustic observation if applicable
    int m_nChan;


    //has the observation been used?
    bool    m_bUsed;

    //flag set to say ignore this observation
    bool m_bIgnore;

    //do we use numerical evaluation of jacobians"
    bool m_bNumericalJacobians;

    /**sensors involved in LBL two way range
    likely to be on different vehicles!*/

    CMOOSNavSensor*  m_pRespondingSensor;
    CMOOSNavSensor*  m_pInterrogateSensor;

    Matrix * m_pXEvaluate;

    //row at which this obs can be found in matrices
    int    m_nRow;

    //dimension of observation
    int m_nDim;

    //whether to use heading bias state or not
    bool m_bUseHeadingBias;


    //data assoication data
    double m_dfInnov;
    double m_dfInnovStd;
protected:
    bool m_bFixed;
    void DoDebug();
    bool DoExplicitLBLJacobians();
    


    bool    MakeBeacon2WRMatrices(Matrix & Innov,
                                Matrix & jH,
                                Matrix & jR);

    bool    MakeXYMatrices(Matrix & Innov,
                                Matrix & jH,
                                Matrix & jR);


    bool    MakeYawMatrices(Matrix & Innov,
                                Matrix & jH,
                                Matrix & jR);

    bool    MakeHeadingBiasMatrices(Matrix & Innov,
                                Matrix & jH,
                                Matrix & jR);

    bool    MakeTideMatrices(Matrix & Innov,
                                Matrix & jH,
                                Matrix & jR);

    bool    MakeDepthMatrices(Matrix & Innov,
                                Matrix & jH,
                                Matrix & jR);

    bool    MakeBodyVelMatrices(Matrix & Innov,
                            Matrix & jH,
                            Matrix & jR);

};

#endif // !defined(AFX_MOOSOBSERVATION_H__76EA853F_4C56_49E6_A856_D98EEACADB0A__INCLUDED_)
