/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_MaintainHeading.h                                */
/*    DATE: August 7th 2008                                      */
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
 
#ifndef BHV_MAINTAIN_HEADING_HEADER
#define BHV_MAINTAIN_HEADING_HEADER

#include "IvPBehavior.h"

class BHV_MaintainHeading : public IvPBehavior {
public:
  BHV_MaintainHeading(IvPDomain);
  ~BHV_MaintainHeading() {}
  
  IvPFunction* onRunState();
  void         onIdleState();
  bool         setParam(std::string, std::string);

protected:
  double  m_desired_heading;

  double  m_basewidth;
  double  m_peakwidth;
  double  m_summit_delta;

  // The m_running flag is true when in the running state and set back
  // to false upon idle. Used for setting the desired heading is 
  // determined by the prevailing heading when activated.
  bool    m_running; 

  // true if desired_heading is set when behavior becomes active
  bool    m_heading_on_active;
};
#endif



















