/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogViewLauncher.h                                    */
/*    DATE: May 31st, 2005                                       */
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

#ifndef LOGVIEW_LAUNCHER
#define LOGVIEW_LAUNCHER

#include <vector>
#include "REPLAY_GUI.h"
#include "ALogDataBroker.h"

class LogViewLauncher
{
 public:
  LogViewLauncher();
  virtual ~LogViewLauncher() {}
  
  REPLAY_GUI *launch(int argc, char **argv);

  void setVerbose() {m_verbose=true; m_dbroker.setVerbose();}
  
protected:
  bool parseCommandArgs(int argc, char **argv);
  bool sanityCheck();

 protected:
  bool handleConfigParam(std::string);
  bool configDataBroker();
  bool configRegionInfo();
  bool configGraphical();
  
  bool handleParamsGUI(std::string);
  bool handleMinTime(std::string);
  bool handleMaxTime(std::string);
  bool handleBackground(std::string);
  bool handleGeometry(std::string);
  bool handleInitialLogPlotL(std::string);
  bool handleInitialLogPlotR(std::string);
  bool handlePanX(std::string);
  bool handlePanY(std::string);
  bool handleZoom(std::string);
  bool handleNowTime(std::string);
  bool handleGrep(std::string);
  bool handleMaxFilePtrs(std::string);
  bool handleVQual(std::string);
  
  bool handleALogViewConfig(std::string);
  bool handleBehaviorVarMapping(std::string);

 private:
  std::vector<std::string> m_alog_files;
  std::vector<std::string> m_tiff_files;
  
  std::string  m_tiff_file;
  std::string  m_alt_nav_prefix;
  std::string  m_start_veh_lft;
  std::string  m_start_var_lft;
  std::string  m_start_veh_rgt;
  std::string  m_start_var_rgt;
  double       m_start_panx;
  double       m_start_pany;
  double       m_start_zoom;
  double       m_start_time;
  bool         m_min_time_set;
  bool         m_max_time_set;
  double       m_min_time;
  double       m_max_time;
  double       m_gui_height;
  double       m_gui_width;

  bool         m_config_file_read;
  bool         m_quick_start;
  bool         m_verbose;
  
  std::string  m_grep1;
  std::string  m_grep2;
 
  ALogDataBroker m_dbroker;

  std::map<std::string, std::string> m_map_bhv_vars;

  std::vector<std::string> m_gui_params;
  std::vector<std::string> m_gui_values;
  
  REPLAY_GUI *m_gui;
};

#endif 

