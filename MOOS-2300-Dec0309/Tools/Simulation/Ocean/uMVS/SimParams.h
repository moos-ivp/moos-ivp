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


// SimParams.h: interface for the CSimParams class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPARAMS_H__37251BDC_0220_4EC4_A2C3_066D7735B978__INCLUDED_)
#define AFX_SIMPARAMS_H__37251BDC_0220_4EC4_A2C3_066D7735B978__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MOOSGenLib/MOOSGenLib.h>


#include <string>

using namespace std;

class CSimParams  
{
public:
    bool Load(STRING_LIST &Params);

    CSimParams();
    virtual ~CSimParams();


    string m_sLogFileName;
    bool    m_bImmediateAcousticLog;
    double  m_dfTOFStd;
    double  m_dfProbMultiPath;
    double  m_dfXYStd;
    double  m_dfZStd;
    double  m_dfYawStd;
    double  m_dfXYVelStd;
    double  m_dfYawBias;
    bool    m_bAddNoise;

private:
    bool GetTokenValPair(string sLine, string &sTok, string &sVal);
};

#endif // !defined(AFX_SIMPARAMS_H__37251BDC_0220_4EC4_A2C3_066D7735B978__INCLUDED_)
