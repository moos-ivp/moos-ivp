/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_MaxSpeed.h                                       */
/*    DATE: Nov 19th 2018                                        */
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
 
#ifndef BHV_MAX_SPEED_HEADER
#define BHV_MAX_SPEED_HEADER

#include "IvPBehavior.h"

class BHV_MaxSpeed : public IvPBehavior {
public:
  BHV_MaxSpeed(IvPDomain);
  ~BHV_MaxSpeed() {}
  
  void         onIdleState() {updateInfoIn();}
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  bool         isConstraint() {return(true);}

 protected:
  bool         updateInfoIn();

 protected: // Configuration variables
  double      m_max_speed;
  double      m_basewidth;
  std::string m_speed_slack_var;

 protected: // State variables
  double      m_osv;
};
#endif

