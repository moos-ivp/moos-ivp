/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_MemoryTurnLimit.h                                */
/*    DATE: Aug 7th 2006                                         */
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
 
#ifndef BHV_MEMORY_TURN_LIMIT_HEADER
#define BHV_MEMORY_TURN_LIMIT_HEADER

#include <list>
#include "IvPBehavior.h"

class BHV_MemoryTurnLimit : public IvPBehavior {
public:
  BHV_MemoryTurnLimit(IvPDomain);
  ~BHV_MemoryTurnLimit() {}
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  bool         isConstraint() {return(true);}

protected:
  void  addHeading(double, double);
  bool  getHeadingAvg(double&);
  bool  getHeadingAvg2(double&);

private: // Configuration Parameters

  double m_memory_time;
  double m_turn_range;


private: // State Variables

  std::list<double> m_heading_val;
  std::list<double> m_heading_time;

};
#endif

