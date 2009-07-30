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
//   This file is part of a  MOOS Core Component. 
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
// ScalarPID.h: interface for the CScalarPID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCALARPID_H__0DC4321A_A987_4499_8902_AAE9F515E921__INCLUDED_)
#define AFX_SCALARPID_H__0DC4321A_A987_4499_8902_AAE9F515E921__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>
#include <fstream>
//using namespace std;

class CScalarPID  
{
public:
    bool SetGoal(double dfGoal);
    bool SetLogPath(std::string & sPath);
    bool SetLog(bool bLog);
    bool SetName(std::string sName);
    void SetLimits(double dfIntegralLimit, double dfOutputLimit);
    bool Run(double dfeIn,double dfErrorTime,double & dfOut);
    unsigned int m_nHistorySize;

    std::list<double> m_DiffHistory;
    CScalarPID();
    CScalarPID( double dfKp,
                double dfKd,
                double dfKi,
                double dfIntegralLimit,
                double dfOutputLimit);

    virtual ~CScalarPID();

    void SetGains(double dfKp,double dfKd,double dfKi);

protected:

    double m_dfKi;
    double m_dfKd;
    double m_dfKp;
    double m_dfe;
    double m_dfeSum;
    double m_dfeOld;
    double m_dfeDiff;
    double m_dfDT;
    double m_dfOldTime;
    double m_dfOut;
    double m_dfIntegralLimit;
    double m_dfOutputLimit;


protected:
    bool Log();
    std::string m_sName;
    std::string      m_sLogPath;

    //note this is just for loggin purposes...
    double m_dfGoal;
    int m_nIterations;
    bool m_bLog;
    std::ofstream m_LogFile;
};

#endif // !defined(AFX_SCALARPID_H__0DC4321A_A987_4499_8902_AAE9F515E921__INCLUDED_)
