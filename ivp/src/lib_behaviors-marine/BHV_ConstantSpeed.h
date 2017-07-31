/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_ConstantSpeed.h                                  */
/*    DATE: Jul 24 2005 Pianosa                                  */
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
 
#ifndef BHV_CONSTANT_SPEED_HEADER
#define BHV_CONSTANT_SPEED_HEADER

#include "IvPBehavior.h"

class BHV_ConstantSpeed : public IvPBehavior {
public:
  BHV_ConstantSpeed(IvPDomain);
  ~BHV_ConstantSpeed() {}
  
  void         onIdleState() {updateInfoIn();}
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

 protected:
  bool         updateInfoIn();

 protected: // Configuration variables
  double       m_desired_speed;
  double       m_peakwidth;
  double       m_basewidth;
  double       m_summitdelta;
  std::string  m_speed_mismatch_var;

 protected: // State variables
  double       m_os_speed;
};
#endif








