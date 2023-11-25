/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogBinHandler.h                                      */
/*    DATE: August 26th, 2020                                    */
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

#ifndef LOG_BIN_HANDLER_HEADER
#define LOG_BIN_HANDLER_HEADER

#include <string>
#include <map>
#include "ExpEntry.h"

class AvgHandler
{
 public:
  AvgHandler();
  ~AvgHandler() {}

  bool handle();
  
  bool setLogFile(std::string);
  void setVerbose() {m_verbose=true;}
  void setFormatAligned(bool v) {m_format_aligned=v;}
  void setFormatNegPos()        {m_format_negpos=true;}
  
 protected: // Config vars

  bool   m_verbose;
  bool   m_format_aligned;
  bool   m_format_negpos;
  
 protected: // State vars

  FILE *m_file_in;

  std::map<double, ExpEntry> m_entries;
};

#endif

