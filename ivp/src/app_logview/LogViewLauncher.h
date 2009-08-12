/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogViewLauncher.h                                    */
/*    DATE: May 31st, 2005                                       */
/*          August 2009 - Took over most of main.cpp's role      */
/*                                                               */
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
#include "Populator_LogPlots.h"
#include "Populator_VPlugPlots.h"
#include "Populator_HelmPlots.h"
#include "ALogEntry.h"

class LogViewLauncher
{
 public:
  LogViewLauncher();
  virtual ~LogViewLauncher();
  
  int launch(int argc, char **argv);

protected:
  void setBackground(int argc, char **argv);
  void setSizeOfGUI(int argc, char **argv);


private:
  std::vector<ALogEntry> m_entries_log_plot;
  std::vector<ALogEntry> m_entries_string;
  std::vector<ALogEntry> m_entries_bhv_ipf;
  std::vector<ALogEntry> m_entries_vplug_plot;
  std::vector<ALogEntry> m_entries_helm_plot;

  std::string  m_tif_file;
  std::string  m_gui_size;
  double       m_gui_height;
  double       m_gui_width;
};

#endif 


