/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_RangePulse.h                                     */
/*    DATE: Feb 7th 2011                                         */
/*    NOTE: A simple behavior for testing range sensor sims.     */
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
 
#ifndef BHV_RANGE_PULSE_HEADER
#define BHV_RANGE_PULSE_HEADER

#include "XYRangePulse.h"
#include "IvPBehavior.h"

class BHV_RangePulse : public IvPBehavior {
public:
  BHV_RangePulse(IvPDomain);
  ~BHV_RangePulse() {}
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();

protected:  // configuration parameters
  double    m_line_pct;
  XYPoint   m_bearing_pt;
  XYSegList m_seglist;
  bool      m_show_pt;
};
#endif












