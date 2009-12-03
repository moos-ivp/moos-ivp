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
// MOOSSensorChannel.h: interface for the CMOOSSensorChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSSENSORCHANNEL_H__8D774E32_199D_486A_80F0_040802FDA6A7__INCLUDED_)
#define AFX_MOOSSENSORCHANNEL_H__8D774E32_199D_486A_80F0_040802FDA6A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <list>
#include <string>
using namespace std;
#include "MOOSNavBase.h"
#include "MOOSObservation.h"
#include "MOOSLinearLeastMedianFilter.h"

typedef list<CMOOSObservation> OBSLIST;
class CMOOSSensorChannel  :public CMOOSNavBase
{
public:
    bool IsBuilt();
    virtual void Trace();
    double GetPercentFull();
    bool IsOnline();
    bool SetNoiseLimit(double dfLimit);
    bool SetHistoryDepth(int nDepth);


    bool Agrees(CMOOSObservation &rObs);
    bool Add(CMOOSObservation & rObs);
    CMOOSSensorChannel();
    virtual ~CMOOSSensorChannel();
    bool AutoDiagnose();

    protected:
        bool m_bActive;
        unsigned int m_nHistoryDepth;
        double m_dfHistoryTimeSpan;
        double m_dfNoiseLimit;
        OBSLIST m_History;

        double m_dfMean;
        double m_dfStd;
        bool m_bBuilt;

        CMOOSLinearLeastMedianFilter m_MedianFilter;

};

#endif // !defined(AFX_MOOSSENSORCHANNEL_H__8D774E32_199D_486A_80F0_040802FDA6A7__INCLUDED_)
