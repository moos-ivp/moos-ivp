/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
  bool setSceneFile(std::string);

  void setVerbose(bool v)           {m_verbose=v;}
  bool setPlotWidCM(std::string);
  bool setPlotHgtCM(std::string);
  bool setGridWidCM(std::string);
  bool setGridHgtCM(std::string);
  bool setPointColor(std::string);
  bool setPointSize(std::string);
  
  void generate();

 protected:
  void handleALogFiles();
  bool handleALogFile(std::string);
  void writeLine(FILE*, const std::string&) const;

  void writeBaseGridFrame(FILE*) const;
  void writeBaseZones(FILE*) const;
  void writeBaseLabels(FILE*) const;
  void writeEncounters(FILE*) const;
  
 protected:
  std::vector<std::string> m_alog_files;
  std::vector<CPAEvent>    m_cpa_events;

  std::string m_community_name;
  std::string m_scene_file;
  std::string m_point_size;
  std::string m_point_color;
  
  std::string m_grid_wid_cm_str;
  std::string m_grid_hgt_cm_str;
  std::string m_plot_wid_cm_str;
  std::string m_plot_hgt_cm_str;

  double m_grid_wid_cm;
  double m_grid_hgt_cm;
  double m_plot_wid_cm;
  double m_plot_hgt_cm;

  bool   m_verbose;
  double m_collision_range;  // meters
  double m_near_miss_range;  // meters
  double m_encounter_range;  // meters
};

#endif





