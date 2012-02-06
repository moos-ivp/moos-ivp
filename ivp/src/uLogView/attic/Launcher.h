/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Launcher.h                                           */
/*    DATE: May 31st, 2005                                       */
/*          Sep 25th, 2011                                       */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef LOGVIEW_LAUNCHER
#define LOGVIEW_LAUNCHER

#include <vector>
#include "Populator_LogTPlots.h"
#include "Populator_VPlugTPlots.h"
#include "ALogEntry.h"
#include "ULV_GUI.h"
#include "IvPDomain.h"

class Launcher
{
 public:
  Launcher();
  virtual ~Launcher() {};
  
  ULV_GUI *launch(int argc, char **argv);

protected:
  void checkForMinMaxTime(int argc, char **argv);
  void setBackground(int argc, char **argv);
  void setSizeOfGUI(int argc, char **argv);
  void setWindowLayout(int argc, char **argv);
  void setALogFiles(int argc, char **argv);
  bool setALogFileSkews();
  void parseALogFiles();
  bool parseALogFile(unsigned int);
  void determineVehicleNames();

  bool buildLogPlots();
  bool buildVPlugPlots();
  
  bool buildGraphical();

private:
  // The .alog files provided at launch time
  std::vector<std::string> m_alog_files;
  std::vector<double>      m_alog_files_skew;

  // geometry specifications for the viewer
  double  m_gui_height;
  double  m_gui_width;

  // Background image specification
  std::string  m_tif_file;

  // Intermediate semi-raw data from the alog files
  // Outer vector index - one per vehicle, i.e., alog file.
  std::vector<std::vector<ALogEntry> > m_entries_log_plot;
  std::vector<std::vector<ALogEntry> > m_entries_vplug_plot;
  std::vector<IvPDomain> m_entry_ivp_domain;

  std::vector<std::vector<std::string> > m_node_reports;
  std::vector<std::string> m_vehicle_name;
  std::vector<std::string> m_vehicle_type;
  std::vector<double>      m_vehicle_length;

  // The various plots created from the alog data before passing
  // to the logview gui or viewers.
  // Outer vector index - one per vehicle, i.e., alog file.
  std::vector<std::vector<LogTPlot> > m_log_plots;
  std::vector<VPlugTPlot> m_vplug_plots;
  std::vector<double>     m_log_starts;


  // Optional time clipping
  double m_min_time;
  double m_max_time;
  double m_now_time;
  bool   m_min_time_set;
  bool   m_max_time_set;
  bool   m_now_time_set;

  ULV_GUI *m_gui;
  std::string m_window_layout;
};

#endif 

