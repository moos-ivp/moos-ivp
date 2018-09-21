/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPFViewUtils.cpp                                     */
/*    DATE: Apr 25th, 2011                                       */
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

#include <iostream>
#include "IPFViewUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: expandHdgSpdIPF
//      Note: The ivp_function domain is used to help expand 
//            incoming function.

IvPFunction *expandHdgSpdIPF(IvPFunction *ivp_function,
			     IvPDomain ivp_domain)
{
  if(!ivp_function)
    return(0);
    
  IvPDomain domain = ivp_function->getPDMap()->getDomain();

  double original_pwt = ivp_function->getPWT();
  
  // Case where ipf defined only over COURSE
  if(domain.hasDomain("course") && !domain.hasDomain("speed")) {
    IvPDomain spd_domain = subDomain(ivp_domain, "speed");
    if(spd_domain.size() > 0) {
      double low_speed = ivp_domain.getVarLow("speed");
      double hgh_speed = ivp_domain.getVarHigh("speed");
      double rng_speed = (hgh_speed - low_speed);
      double avg_speed = (low_speed + hgh_speed) / 2.0;
      
      ZAIC_PEAK spd_zaic(spd_domain, "speed");
      spd_zaic.setSummit(avg_speed);
      spd_zaic.setPeakWidth(rng_speed+1);
      
      IvPFunction *spd_of = spd_zaic.extractOF();
      OF_Coupler   coupler;
      IvPFunction *new_ipf = coupler.couple(ivp_function, spd_of);
      ivp_function = new_ipf;
      ivp_function->setPWT(original_pwt);
    }
  }
  
  // Case where ipf defined only over SPEED
  else if(domain.hasDomain("speed") && !domain.hasDomain("course")) {
    IvPDomain crs_domain = subDomain(ivp_domain, "course");
    if(crs_domain.size() > 0) {
      ZAIC_PEAK crs_zaic(crs_domain, "course");
      crs_zaic.setSummit(180);
      crs_zaic.setPeakWidth(360);
      IvPFunction *crs_of = crs_zaic.extractOF();
      OF_Coupler   coupler;
      ivp_function = coupler.couple(ivp_function, crs_of);
      ivp_function->setPWT(original_pwt);
    }
  }

  return(ivp_function);
}
  








