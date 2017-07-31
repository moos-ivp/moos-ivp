/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_MinAltitudeX.h                                   */
/*    DATE: Mar 3, 2007                                          */
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
 
#ifndef BHV_MIN_ALTITUDEX_HEADER
#define BHV_MIN_ALTITUDEX_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_MinAltitudeX : public IvPBehavior {
public:
  BHV_MinAltitudeX(IvPDomain);
  ~BHV_MinAltitudeX() {}
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();

protected:
  bool         updateInfoIn();
  void         postStatusReport();

private: // Behavior Configuration Parameters
  double      m_min_altitude;
  bool        m_missing_altitude_critical;

private: // Behavior state
  double      m_curr_depth;
  double      m_curr_altitude;
  double      m_curr_max_depth;
};

#endif
 









