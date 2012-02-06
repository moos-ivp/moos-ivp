/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GeneralLauncher.h                                    */
/*    DATE  Oct 1st, 2011                                        */
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

#ifndef GENERAL_ULOGVIEW_LAUNCHER
#define GENERAL_ULOGVIEW_LAUNCHER

#include <vector>
#include "ALogEntry.h"

class GeneralLauncher
{
 public:
  GeneralLauncher();
  virtual ~GeneralLauncher() {};
  
protected:
  void checkForMinMaxTime(int argc, char **argv);
  void setALogFiles(int argc, char **argv);
  bool setALogFileLogStarts();

protected:
  // The .alog files provided at launch time
  std::vector<std::string> m_alog_files;
  std::vector<double>      m_log_starts;

  // Keep track of time boundaries across all log files. The time
  // boundaries are of use to both the LogPlot and NavPlotViewers.
  double m_global_mintime_utc;
  double m_global_maxtime_utc;

  // Optional time clipping
  double m_min_clip_time;
  double m_max_clip_time;
  bool   m_min_clip_time_set;
  bool   m_max_clip_time_set;

  // Optional preference for starting timestamp
  double m_now_time;
  bool   m_now_time_set;
};

#endif 

