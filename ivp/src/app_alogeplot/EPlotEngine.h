/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: EPlotEngine.h                                        */
/*    DATE: January 16th, 2016                                   */
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

#ifndef EPLOT_ENGINE_HEADER
#define EPLOT_ENGINE_HEADER

#include <cstdio>
#include <string>
#include <vector>
#include "CPAEvent.h"

class EPlotEngine
{
 public:
  EPlotEngine();
  ~EPlotEngine() {}

  bool addALogFile(std::string);
  
  void setVerbose(bool v) {m_verbose=v;}
  void generate();

 protected:
  bool handleALogFile(std::string);
  void writeLine(FILE*, const std::string&) const;

 protected:
  std::vector<std::string> m_alog_files;
  std::vector<CPAEvent>    m_cpa_events;

  std::string m_community_name;
  
  bool   m_verbose;
  double m_collision_range;
  double m_near_miss_range;
  double m_encounter_range;
};

#endif
