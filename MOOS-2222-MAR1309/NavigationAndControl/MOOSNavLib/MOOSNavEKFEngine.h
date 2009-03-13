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
// MOOSNavEKFEngine.h: interface for the CMOOSNavEKFEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVEKFENGINE_H__E0A35A81_1CA8_4A96_9B2B_F301DB02E70C__INCLUDED_)
#define AFX_MOOSNAVEKFENGINE_H__E0A35A81_1CA8_4A96_9B2B_F301DB02E70C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MOOSNavEngine.h"

class CMOOSNavEKFEngine : public CMOOSNavEngine  
{
public:
    virtual bool Reset();
    bool IsBooted();
    virtual bool Iterate(double dfTimeNow);
    virtual bool Initialise(STRING_LIST  sParams);
    bool    AddData(const CMOOSMsg &Msg);
    CMOOSNavEKFEngine();
    virtual ~CMOOSNavEKFEngine();

protected:

    bool LimitVelocityStates();
    bool MakeSymmetric();
    bool PredictForward(double dfStop,double dfTimeNow);
    bool PublishResults();
    bool OnIterateDone(double dfTimeNow);
    bool PreparePredictionMatrices();
    bool FillGlobalParamModelMatrices(double dfDeltaT);
    bool InitialiseEstimates();
    bool Boot();
    bool DoPredict(double dfDeltaT);

    //fancy data rejection
    int  HyperDimSelect(Matrix & Innov,Matrix & Cov,Matrix & InvCov);
    bool IsInside(Matrix &v, Matrix &Ellipse);
    bool ApplyHalfBakedHeuristics(double dfInnov,double dfInnovStd, int i);

    Matrix m_jF;
    Matrix m_jQ;
    Matrix m_S;

    Matrix m_XTmp;
    Matrix m_PTmp;

    double m_dfXYDynamics;
    double m_dfZDynamics;
    double m_dfYawDynamics;

    double m_dfPxx0;
    double m_dfPyy0;
    double m_dfPzz0;
    double m_dfPhh0;
    double m_dfPTide0;

    double m_dfX0;
    double m_dfY0;
    double m_dfZ0;
    double m_dfH0;
    double m_dfTide0;
    
    bool    m_bBooted;
    double m_dfLastIterated;
    double m_dfLag;
    double m_dfYawBiasStd;
    int     m_nUpdates;

    double m_dfMaxZVel;

};

#endif // !defined(AFX_MOOSNAVEKFENGINE_H__E0A35A81_1CA8_4A96_9B2B_F301DB02E70C__INCLUDED_)
