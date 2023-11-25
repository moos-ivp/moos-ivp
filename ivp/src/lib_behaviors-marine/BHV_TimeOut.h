/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_TimeOut.h                                        */
/*    DATE: July 27th 2007 (pulled from BHV_OpRegion)            */
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
 
#ifndef BHV_TIME_OUT_HEADER
#define BHV_TIME_OUT_HEADER

#include "IvPBehavior.h"

class BHV_TimeOut : public IvPBehavior {
 public:
  BHV_TimeOut(IvPDomain);
  ~BHV_TimeOut() {}
  
  bool         setParam(std::string, std::string);
  IvPFunction* onRunState();

 protected:
  void      setTimeStamps();

protected: // Configuration Parameters (time in seconds)
  double    m_max_time;

protected: // State Variables (all time values in seconds)
  double    m_previous_time;  
  double    m_current_time;
  double    m_delta_time;
  double    m_start_time;
  double    m_elapsed_time;
  bool      m_first_time;
};
#endif




















