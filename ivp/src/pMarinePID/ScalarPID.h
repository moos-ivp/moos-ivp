/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ScalarPID.h                                          */
/*    DATE: Oct 1 2005                                           */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

/******************************************************************/
/* This file is a slightly augmented version of P.Newman's        */
/* ScalarPID class found in MOOSGenLib.                           */
/*                                                                */
/* It is augmented w/ a copy constructor and assignment operator. */
/*                                                                */
/* All variable names remain the same, some comments are added.   */
/*                                                                */
/* Name of the class is changed from "CScalarPID" to "ScalarPID"  */
/******************************************************************/


#ifndef MOD_SCALARPID_HEADER
#define MOD_SCALARPID_HEADER

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>
#include <fstream>

class ScalarPID  
{
public:
  ScalarPID();
  ScalarPID(double dfKp, double dfKd,
	     double dfKi, double dfIntegralLimit,
	     double dfOutputLimit);
  ScalarPID(const ScalarPID&);    // **new **
  virtual ~ScalarPID();

  const ScalarPID &operator=(const ScalarPID&);  // **new**

  void SetGains(double dfKp,double dfKd,double dfKi);
  void SetLimits(double dfIntegralLimit, double dfOutputLimit);

  void SetGoal(double dfGoal);
  void SetLogPath(std::string & sPath);
  void SetLog(bool bLog);
  void SetName(std::string sName);
  bool Run(double dfeIn, double dfErrorTime, double& dfOut);

protected:  // Core parameters
  double m_dfKi;
  double m_dfKd;
  double m_dfKp;
  double m_dfIntegralLimit;
  double m_dfOutputLimit;

protected: // Data persistent between runs
  double m_dfeOld;
  double m_dfOldTime;
  double m_dfOut;
  unsigned int m_nHistorySize;
  std::list<double> m_DiffHistory;
  
  double m_dfe;
  double m_dfeSum;
  double m_dfeDiff;
  double m_dfDT;

  
protected:
  bool Log();
  
  //note this is just for logging purposes...
  double  m_dfGoal;
  int     m_nIterations;

  bool          m_bLog;
  std::string   m_sName;
  std::string   m_sLogPath;
  std::ofstream m_LogFile;
};

#endif









