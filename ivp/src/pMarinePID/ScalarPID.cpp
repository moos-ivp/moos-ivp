/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ScalarPID.cpp                                        */
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

/*****************************************************************/
/* This file is a modified version of P.Newman's ScalarPID class */
/* found in MOOSGenLib.                                          */
/*****************************************************************/

#include "MOOSGenLibGlobalHelper.h"
#include "ScalarPID.h"
#include <math.h>

using namespace std;

//-----------------------------------------------------------------
// Constructor with parameters

ScalarPID::ScalarPID(double dfKp, double dfKd, double dfKi,
		     double integral_limit, double output_limit)
{
  reset();
  m_dfKp      = dfKp;
  m_dfKd      = dfKd;
  m_dfKi      = dfKi;
  m_integral_limit = integral_limit;
  m_output_limit   = output_limit;
}

//-----------------------------------------------------------------
// Procedure: reset

void ScalarPID::reset()
{
  m_dfKp  =   0;
  m_dfKd  =   0;
  m_dfKi  =   0;
  m_integral_limit = 0;
  m_output_limit   = 0;

  m_dfeOld      = 0;
  m_dfOldTime   = 0;
  m_dfOut       = 0;
  m_iterations  = 0;

  m_dfe     = 0;
  m_dfeSum  = 0;
  m_dfeDiff = 0;
  m_dfDT    = 0;
  m_nHistorySize = 10;
}


//-------------------------------------------------------------------
// Procedure: Run

bool ScalarPID::Run(double dfeIn, double dfErrorTime, double &dfOut)
{
  m_dfe = dfeIn;

  m_iterations++;
  
  // Figure out time increment...
  if(m_iterations != 0) {

    m_dfDT = dfErrorTime - m_dfOldTime;

    if(m_dfDT < 0) {
      MOOSTrace("ScalarPID::Run() : negative sample time \n");
      return(false);
    }
    else if(m_dfDT == 0) {
      //nothing to do...
      dfOut = m_dfOut;
      return(true);
    }
    
    //figure out differntial
    double dfDiffNow = (dfeIn-m_dfeOld)/m_dfDT;
    m_DiffHistory.push_front(dfDiffNow);
    while(m_DiffHistory.size() >= m_nHistorySize)
      m_DiffHistory.pop_back();
    
    m_dfeDiff = 0;
    list<double>::iterator p;
    for(p=m_DiffHistory.begin(); p!=m_DiffHistory.end(); p++)
      m_dfeDiff  += *p;   
    
    m_dfeDiff /= m_DiffHistory.size();
  }
  else {
    //this is our first time through
    m_dfeDiff = 0;
  }
  
  
  if(m_dfKi>0) {
    //calculate integral term  
    m_dfeSum  += m_dfKi * m_dfe * m_dfDT;
    
    //prevent integral wind up...
    if(fabs(m_dfeSum)>=fabs(m_integral_limit)) {
      int nSign = (int)(fabs(m_dfeSum)/m_dfeSum);
      m_dfeSum = nSign*fabs(m_integral_limit);
    }
  }
  else
    m_dfeSum = 0;


  //do pid control
  m_dfOut = (m_dfKp*m_dfe) + (m_dfKd*m_dfeDiff) + m_dfeSum; 
  //note Ki is already in dfeSum
  
  //prevent saturation..
  if(m_dfOut < -m_output_limit)
    m_dfOut = -m_output_limit;
  else if(m_dfOut > m_output_limit)
    m_dfOut = m_output_limit;
  
  //save old value..
  m_dfeOld    = m_dfe;
  m_dfOldTime = dfErrorTime;
  
  dfOut = m_dfOut;
  
  return(true);
}

//-------------------------------------------------------------------
// Procedure: SetGains

void ScalarPID::SetGains(double dfKp, double dfKd, double dfKi)
{
  m_dfKp = dfKp;
  m_dfKd = dfKd;
  m_dfKi = dfKi;
}

//-------------------------------------------------------------------
// Procedure: SetLimits

void ScalarPID::SetLimits(double integral_limit, double output_limit)
{
  m_integral_limit = integral_limit;
  m_output_limit    = output_limit;
  if(m_output_limit < 0)
    m_output_limit *= -1;
}
