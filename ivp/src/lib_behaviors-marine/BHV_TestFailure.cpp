/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_TestFailure.cpp                                  */
/*    DATE: October 21st 2011                                    */
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

#include <cstdlib>
#include <assert.h>
#include <cstdlib>
#include "MBUtils.h"
#include "MBTimer.h"
#include "BHV_TestFailure.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_TestFailure::BHV_TestFailure(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "test_failure");

  m_failure_crash = true;
  m_failure_burn  = false;
  m_failure_burn_time = 3;  // seconds
}

//-----------------------------------------------------------
// Procedure: setParam
//   Handles: failure_type = crash
//            failure_type = burn
//            failure_type = burn,3

bool BHV_TestFailure::setParam(string param, string val) 
{
  if(param == "failure_type") {
    val = stripBlankEnds(tolower(val));
    if(val == "crash") {
      m_failure_crash = true;
      m_failure_burn  = false;
    }
    else if(strBegins(val, "burn") || strBegins(val, "hang")) {
      m_failure_crash = false;
      m_failure_burn  = true;
      string burn = biteString(val, ',');
      string time = val;
      if(isNumber(time))
	m_failure_burn_time = atof(time.c_str());
    }
  }
  else
    return(false);
  
  return(true);      
}

//-----------------------------------------------------------
// Procedure: onCompleteState

void BHV_TestFailure::onCompleteState()
{
  if(m_failure_crash)
    assert(0);
  else if(m_failure_burn) {
    MBTimer timer;
    timer.start();
    bool done = false;
    while(!done) {
      double elapsed_time = timer.get_float_wall_time();
      if(elapsed_time > m_failure_burn_time)
	done = true;
    }
    timer.stop();
  }
}









