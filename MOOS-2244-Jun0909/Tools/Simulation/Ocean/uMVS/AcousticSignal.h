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


// AcousticSignal.h: interface for the CAcousticSignal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACOUSTICSIGNAL_H__C9D1AD0E_042D_4D0F_93AF_39D8C18E0E8B__INCLUDED_)
#define AFX_ACOUSTICSIGNAL_H__C9D1AD0E_042D_4D0F_93AF_39D8C18E0E8B__INCLUDED_

#include "AcousticDefs.h"    // Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <newmat.h>

using namespace NEWMAT;

#include <MOOSGenLib/MOOSGenLib.h>

#include "SimBase.h"

class CAcousticSignal  : public CSimBase
{
public:
    std::string GetSrcName();
    bool SetSrcName(const std::string & sSrcName);
    static AcousticChannel ChannelFromString(std::string & sStr);
    double GetStartTime();
    AcousticChannel GetChannel();
    bool SetChannel(AcousticChannel eChannel);
    bool HasDecayed(double dfTimeNow);
    double Age(double dfTimeNow);
    double GetExpectedIntersectionTime(Matrix & Location);
    
    CAcousticSignal();
    CAcousticSignal(double dfX,double dfY,double dfZ,double dfStartTime);
    virtual ~CAcousticSignal();

protected:
    Matrix          m_SrcPos;
    std::string          m_sSrcName;
    double          m_dfSV;
    double          m_dfStartTime;
    double          m_dfMaximumRange;
    AcousticChannel m_eChannel;

    
};

#endif // !defined(AFX_ACOUSTICSIGNAL_H__C9D1AD0E_042D_4D0F_93AF_39D8C18E0E8B__INCLUDED_)
