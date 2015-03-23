/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
  virtual ~LogViewLauncher() {};
  
  REPLAY_GUI *launch(int argc, char **argv);

  bool addPlotRequest(std::string);

protected:
  void checkForMinMaxTime(int argc, char **argv);
  void setBackground(int argc, char **argv);
  void setSizeOfGUI(int argc, char **argv);
  void setALogFiles(int argc, char **argv);

  bool buildLogPlots();
  void buildGraphical();

private:
  // geometry specifications for the viewer
  double  m_gui_height;
  double  m_gui_width;

  // Background image specification
  std::string  m_tiff_file;

  std::vector<std::string> m_plot_request_var;
  std::vector<std::string> m_plot_request_fld;

  ALogDataBroker m_dbroker;

  REPLAY_GUI *m_gui;
};

#endif 
