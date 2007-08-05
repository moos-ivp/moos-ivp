/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PID.h                                                */
/*    DATE: May 14th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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

#include "PID.h"
#include <math.h>
#include <iostream>

using namespace std;

//-------------------------------------------------------------------
// Constructor

PID::PID(double dfKp, double dfKd,
	 double dfKi, double dfIntegralLimit,
	 double dfOutputLimit)
{
  m_dfKp            = dfKp;
  m_dfKd            = dfKd;
  m_dfKi            = dfKi;
  m_dfIntegralLimit = dfIntegralLimit;
  m_dfOutputLimit   = dfOutputLimit;

  m_dfeOld      = 0;
  m_dfOldTime   = 0;
  m_dfOut       = 0;
  m_nIterations = 0;

  m_dfeSum      = 0;
  m_dfeDiff     = 0;
  m_dfDT        = 0;
  m_dfe         = 0;
}

//-------------------------------------------------------------------
// Copy Constructor

PID::PID(const PID& right)
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

  m_dfeSum      = 0;
  m_dfeDiff     = 0;
  m_dfDT        = 0;
  m_dfe         = 0;
}

//-------------------------------------------------------------------
// Assignment Operator

const PID &PID::operator=(const PID& right)
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
  }
  return(*this);
}

//--------------------------------------------------------------------
// Procedure: Run

double PID::Run(double dfeIn, double dfErrorTime)
{
  m_dfe  = dfeIn;
  
#if 0
  //figure out time increment...
  if(m_nIterations++!=0) {
    
    m_dfDT = dfErrorTime - m_dfOldTime;
    
    if(m_dfDT <= 0)  return(m_dfOut); //nothing to do...
    
    //figure out differential
    double dfDiffNow = (dfeIn-m_dfeOld) / m_dfDT;
    m_DiffHistory.push_front(dfDiffNow);
    while(m_DiffHistory.size()>=m_nHistorySize) 
      m_DiffHistory.pop_back();
    
    m_dfeDiff = 0;
    list<double>::iterator p;
    for(p = m_DiffHistory.begin(); p!=m_DiffHistory.end(); p++)
      m_dfeDiff += *p;   

    m_dfeDiff/=m_DiffHistory.size();
  }
  else //this is our first time through
    m_dfeDiff = 0;
  
  if(m_dfKi>0)	{
    //calculate integral term  
    m_dfeSum += m_dfKi * m_dfe * m_dfDT;
    
    //prevent integral wind up...
    if(fabs(m_dfeSum)>=fabs(m_dfIntegralLimit)) {
      int nSign = (int)(fabs(m_dfeSum)/m_dfeSum);
      m_dfeSum = nSign*fabs(m_dfIntegralLimit);
    }
  }
  else {
    m_dfeSum = 0;
  }
#endif

  //do pid control
  m_dfOut = (m_dfKp * m_dfe) + (m_dfKd * m_dfeDiff) + m_dfeSum; 
  //note Ki is already in dfeSum
  
  //prevent saturation..
  if(fabs(m_dfOut) >= fabs(m_dfOutputLimit) ) {        
    int nSign =(int)( fabs(m_dfOut)/m_dfOut);
    m_dfOut = nSign*fabs(m_dfOutputLimit);
  }
  
  //save old value..
  m_dfeOld    = m_dfe;
  m_dfOldTime = dfErrorTime;

  return(m_dfOut);
}

void PID::SetGains(double dfKp, double dfKd, double dfKi)
{
  m_dfKp = dfKp;
  m_dfKd = dfKd;
  m_dfKi = dfKi;
}

void PID::SetLimits(double dfIntegralLimit, double dfOutputLimit)
{
  m_dfIntegralLimit = dfIntegralLimit;
  m_dfOutputLimit   = dfOutputLimit;
}





