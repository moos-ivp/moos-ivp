/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ScanHandler.h                                        */
/*    DATE: June 5th, 2008                                       */
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

#ifndef SCAN_HANDLER_HEADER
#define SCAN_HANDLER_HEADER

#include "ScanReport.h"

class ScanHandler
{
 public:
  ScanHandler();
  ~ScanHandler() {}

  bool setParam(const std::string&, const std::string&);
  bool handle(const std::string& alogfile, bool rate_only=false);

  void varStatReport();
  void appStatReport();
  void dataRateReport();
  void loglistReport();
  
  std::string procColor(std::string proc_name);

 protected:

  std::string m_sort_style;

  ScanReport  m_report;
  bool        m_use_colors;
  bool        m_use_full_source;
  
  std::map<std::string, std::string> m_pcolor_map;
  std::vector<std::string>           m_pcolors;
  unsigned int                       m_next_color_ix;
};

#endif









