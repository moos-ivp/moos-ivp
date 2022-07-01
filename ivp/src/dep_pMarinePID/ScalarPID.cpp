/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ScalarPID.cpp                                        */
/*    DATE: Oct 1 2005                                           */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
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

#include "MOOS/libMOOS/MOOSLib.h"
#include "ScalarPID.h"
#include <math.h>
#include <iostream>
#include <iomanip>
#include "MBUtils.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ScalarPID::ScalarPID()
{
  m_dfKp  =   0;
  m_dfKd  =   0;
  m_dfKi  =   0;
  m_dfIntegralLimit = 0;
  m_dfOutputLimit   = 0;

  m_dfeOld      = 0;
  m_dfOldTime   = 0;
  m_dfOut       = 0;
  m_nIterations = 0;

  m_dfe     = 0;
  m_dfeSum  = 0;
  m_dfeDiff = 0;
  m_dfDT    = 0;
  m_nHistorySize = 10;
  m_dfGoal  = 0;

  m_sLogPath = "";
  m_bLog     = false;
  m_debug    = false;
  m_max_sat  = false;
  m_sName    = "PID_LOGFILE";

}


ScalarPID::ScalarPID(double dfKp, double dfKd, double dfKi,
		     double dfIntegralLimit, double dfOutputLimit)
{
  m_dfKp      = dfKp;
  m_dfKd      = dfKd;
  m_dfKi      = dfKi;
  m_dfeSum    = 0;
  m_dfeOld    = 0;
  m_dfeDiff   = 0;
  m_dfDT      = 0;
  m_dfOldTime = 0;
  m_dfe       = 0;
  m_dfOut     = 0;
  
  m_dfIntegralLimit = dfIntegralLimit;
  m_dfOutputLimit   = dfOutputLimit;
  m_nIterations     = 0;
  m_nHistorySize    = 10;
  m_bLog            = false;
  m_debug           = false;
  m_max_sat         = false;
}

//-------------------------------------------------------------------
// Copy Constructor

ScalarPID::ScalarPID(const ScalarPID& right)
{
  m_dfKp            = right.m_dfKp;
  m_dfKd            = right.m_dfKd;
  m_dfKi            = right.m_dfKi;
  m_dfIntegralLimit = right.m_dfIntegralLimit;
  m_dfOutputLimit   = right.m_dfOutputLimit;

  m_dfeOld      = 0;
  m_dfOldTime   = 0;
  m_dfOut       = 0;
  m_nIterations = 0;

  m_dfe         = 0;
  m_dfeSum      = 0;
  m_dfeDiff     = 0;
  m_dfDT        = 0;
  m_nHistorySize = 10;
  m_dfGoal       = 0;

  m_debug   = false;
  m_max_sat = false;
}

//-------------------------------------------------------------------
// Assignment Operator

const ScalarPID &ScalarPID::operator=(const ScalarPID& right)
{
  if(&right != this) {
    m_dfKp            = right.m_dfKp;
    m_dfKd            = right.m_dfKd;
    m_dfKi            = right.m_dfKi;
    m_dfIntegralLimit = right.m_dfIntegralLimit;
    m_dfOutputLimit   = right.m_dfOutputLimit;
    
    m_dfeOld      = 0;
    m_dfOldTime   = 0;
    m_dfOut       = 0;
    m_nIterations = 0;
    
    m_dfeSum      = 0;
    m_dfeDiff     = 0;
    m_dfDT        = 0;
    m_dfe         = 0;
    m_debug       = false;
    m_max_sat     = false;
  }
  return(*this);
}


//-------------------------------------------------------------------
ScalarPID::~ScalarPID()
{
}

//-------------------------------------------------------------------
bool ScalarPID::Run(double dfeIn, double dfErrorTime, bool reset_I, double &dfOut)
{
  // Reset max_sat flag on every interation
  m_max_sat = false;
  
  if(m_debug) {
    m_debug_str =  "dfeIn=" + doubleToString(dfeIn);
    m_debug_str += ", dfErrorTime=" + doubleToString(dfErrorTime);
  }
    
  m_dfe  = dfeIn;
  
  //figure out time increment...
  if(m_nIterations++!=0) {
        
    m_dfDT = dfErrorTime-m_dfOldTime;

    if(m_debug)
      m_debug_str += ", m_dfDT=" + doubleToString(m_dfDT);
      
    
    if(m_dfDT<0) {
      MOOSTrace("ScalarPID::Run() : negative or zero sample time\n");
      return(false);
    }
    else if(m_dfDT ==0) {
      //nothing to do...
      dfOut = m_dfOut;
      Log();
      return(true);
    }
    
    //figure out differntial
    double dfDiffNow = (dfeIn-m_dfeOld)/m_dfDT;
    m_DiffHistory.push_front(dfDiffNow);
    while(m_DiffHistory.size() >= m_nHistorySize) {
      m_DiffHistory.pop_back();
    }
    if(m_debug) {
      m_debug_str += ", dfDiffNow=" + doubleToString(dfDiffNow);
      m_debug_str += ", DiffHistSize=" + uintToString(m_DiffHistory.size());
    }
      
    m_dfeDiff = 0;
    list<double>::iterator p;
    for(p = m_DiffHistory.begin();p!=m_DiffHistory.end();p++) {
      m_dfeDiff   += *p;   
    }
    if(m_debug) 
      m_debug_str += ", mdfeDiff(1)=" + doubleToString(m_dfeDiff);

    m_dfeDiff/=m_DiffHistory.size();
    if(m_debug) 
      m_debug_str += ", mdfeDiff(2)=" + doubleToString(m_dfeDiff);
  }
  else {
    //this is our first time through
    m_dfeDiff = 0;
  }
  
  
  if(m_dfKi>0) {
    // Added May 2022 to reset integral when crossing desired command
    // and/or when desired command changes.
    if (reset_I){
      cout << "Resetting integral sum of " << m_dfeSum << "to zero." << endl;
      m_dfeSum = 0.0;
    }

    //calculate integral term  
    m_dfeSum    +=  m_dfKi*m_dfe*m_dfDT;

    if(m_debug) {
      m_debug_str += ", m_dfKi" + doubleToString(m_dfKi);
      m_debug_str += ", m_dfeSum(1)" + doubleToString(m_dfeSum);
      m_debug_str += ", m_dfIntegralLimit" + doubleToString(m_dfIntegralLimit);
    }
    
    //prevent integral wind up...
    if(fabs(m_dfeSum)>=fabs(m_dfIntegralLimit)) {
      int nSign = (int)(fabs(m_dfeSum)/m_dfeSum);
      m_dfeSum = nSign*fabs(m_dfIntegralLimit);
      if(m_debug) 
	m_debug_str += ", m_dfeSum(2)" + doubleToString(m_dfeSum);
    }
  }
  else {
    m_dfeSum = 0;
  }

  // m_dfOut = (m_dfKp*m_dfe) + (m_dfKd*m_dfeDiff) + m_dfeSum; 
  //        m_dfKp =
  //        m_dfe  =
  //     (m_dfKp * m_dfe) =
  //        m_dfKd = 
  //     m_dfeDiff = 0.10223 
  //     (m_dfKd * m_dfeDiff) =
  //     m_dfeSum  = 

  // dfeIn=145.88560, dfErrorTime=1556750915.09088, m_dfDT=0.10035, dfDiffNow=0.09397, DiffHistSize=9,
  // mdfeDiff(1)=0.92005, mdfeDiff(2)=0.10223, m_dfOut(1)218.84885, m_dfOut(1)100 

  
  
  //do pid control
  m_dfOut = (m_dfKp*m_dfe) + (m_dfKd*m_dfeDiff) + m_dfeSum; 
  if(m_debug) 
    m_debug_str += ", m_dfOut(1)" + doubleToStringX(m_dfOut);
  //note Ki is already in dfeSum
  
  //prevent saturation..
  if(fabs(m_dfOut)>=fabs(m_dfOutputLimit) ) {        
    int nSign =(int)( fabs(m_dfOut)/m_dfOut);
    m_dfOut = nSign*fabs(m_dfOutputLimit);
    if(m_debug) 
      m_debug_str += ", m_dfOut(1)" + doubleToStringX(m_dfOut);
    m_max_sat = true;
  }
  
  //save old value..
  m_dfeOld    = m_dfe;
  m_dfOldTime = dfErrorTime;
  
  dfOut = m_dfOut;
  
  //do logging..
  Log();
  
  return(true);
}

//-------------------------------------------------------------------
void ScalarPID::SetGains(double dfKp, double dfKd, double dfKi)
{
  m_dfKp      =   dfKp;
  m_dfKd      =   dfKd;
  m_dfKi      =   dfKi;
}

//-------------------------------------------------------------------
void ScalarPID::SetLimits(double dfIntegralLimit, double dfOutputLimit)
{
  m_dfIntegralLimit = dfIntegralLimit;
  m_dfOutputLimit   = dfOutputLimit;
}

//-------------------------------------------------------------------
bool ScalarPID::Log()
{
  int nWidth = 17;
  
  if(m_bLog) {
    if(!m_LogFile.is_open()) {
      string sName = MOOSFormat("%s%s%s.pid",
				m_sLogPath.c_str(),
				m_sName.c_str(),
				MOOSGetTimeStampString().c_str());
      m_LogFile.open(sName.c_str());
      if(!m_LogFile.is_open()) {
	m_bLog = false;
	return false;
      }
      
      m_LogFile.setf(ios::left);

      m_LogFile<<"%% Kp = "<<m_dfKp<<endl;
      m_LogFile<<"%% Kd = "<<m_dfKd<<endl;
      m_LogFile<<"%% Ki = "<<m_dfKi<<endl;
      m_LogFile<<setw(20)<<"%T";
      m_LogFile<<setw(nWidth)<<"Kp";
      m_LogFile<<setw(nWidth)<<"Kd";
      m_LogFile<<setw(nWidth)<<"Ki";
      m_LogFile<<setw(nWidth)<<"DT";
      m_LogFile<<setw(nWidth)<<"Output";
      m_LogFile<<setw(nWidth)<<"InputError";
      m_LogFile<<setw(nWidth)<<"Kp*e";
      m_LogFile<<setw(nWidth)<<"Kd*de/dt";
      m_LogFile<<setw(nWidth)<<"Ki*int(e)";
      m_LogFile<<setw(nWidth)<<"Desired";
      m_LogFile<<setw(nWidth)<<"Actual"<<endl;
      
    }
    
    //do pid control
    //    m_dfOut = m_dfKp*m_dfe+
    //    m_dfKd*m_dfeDiff+
    //   m_dfKi*m_dfeSum;
    m_LogFile.setf(ios::left);
    m_LogFile<<setw(20)<<setprecision(12)<<m_dfOldTime<<' ';
    m_LogFile<<setprecision(5);
    m_LogFile<<setw(nWidth)<<m_dfKp<<' ';
    m_LogFile<<setw(nWidth)<<m_dfKd<<' ';
    m_LogFile<<setw(nWidth)<<m_dfKi<<' ';
    m_LogFile<<setw(nWidth)<<m_dfDT<<' ';
    m_LogFile<<setw(nWidth)<<m_dfOut<<' ';
    m_LogFile<<setw(nWidth)<<m_dfe<<' ';
    m_LogFile<<setw(nWidth)<<m_dfKp*m_dfe<<' ';
    m_LogFile<<setw(nWidth)<<m_dfKd*m_dfeDiff<<' ';
    m_LogFile<<setw(nWidth)<<m_dfeSum<<' '; //Ki is already in dfeSum
    m_LogFile<<setw(nWidth)<<m_dfGoal<<' '; 
    m_LogFile<<setw(nWidth)<<m_dfGoal-m_dfe<<' '; 
    
    m_LogFile<<endl;
    
  }
  
  m_LogFile.flush();
  
  return(true);
}

//-------------------------------------------------------------------
void ScalarPID::SetName(string sName)
{
  m_sName = sName;
}

//-------------------------------------------------------------------
void ScalarPID::SetLog(bool bLog)
{
  m_bLog = bLog;
}

//-------------------------------------------------------------------
void ScalarPID::SetLogPath(string &sPath)
{
  m_sLogPath = sPath;

  if(sPath != "")
    m_bLog = true;
  else
    m_bLog = false;
}

//-------------------------------------------------------------------
void ScalarPID::SetGoal(double dfGoal)
{
  m_dfGoal =dfGoal;
}


















