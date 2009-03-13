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


// SimEntity.h: interface for the CSimEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMENTITY_H__73AEBFEA_E3FA_46D9_91E9_DCA62067F15A__INCLUDED_)
#define AFX_SIMENTITY_H__73AEBFEA_E3FA_46D9_91E9_DCA62067F15A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SimBase.h"
#include "SimLoggable.h"
#include "AcousticNode.h"
#include "AcousticSignal.h"
#include "SimEnvironment.h"

#include <list>
using namespace std;

typedef list<CAcousticNode *>        ACOUSTIC_NODE_LIST;

class CSimEnvironment;

class CSimEntity  : public CSimLoggable
{
public:
    virtual bool        SetParams(CSimParams* pParams);
    bool                SolveAcoustics(double dfTime,double dfDT);
    void                SetEnvironment(CSimEnvironment * pEnv);
    virtual bool        GetNodePosition(CAcousticNode & Node, double dfDT,Matrix & Result);
    bool                GetAcousticNodes(ACOUSTIC_NODE_LIST & List);
    virtual bool        Iterate(double dfTimeNow,double dfDT);
    CSimEntity();
    virtual ~CSimEntity();

    ACOUSTIC_NODE_LIST        m_AcousticNodes;

    Matrix m_Pos_e;
    Matrix m_Vel_e;

    double m_dfLastLogState;
    double m_dfLogFrequency; 

    virtual bool LogState(double dfTimeNow);

    std::string m_sInputPrefix;
    std::string m_sOutputPrefix;


protected:
    double HeadingFromYaw(double dfYaw);
    CSimEnvironment* m_pEnvironment;
    bool IsLocalSource(std::string sSrc);



};

#endif // !defined(AFX_SIMENTITY_H__73AEBFEA_E3FA_46D9_91E9_DCA62067F15A__INCLUDED_)
