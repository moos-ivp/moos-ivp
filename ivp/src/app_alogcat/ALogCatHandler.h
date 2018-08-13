/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogCatHandler.h                                     */
/*    DATE: Aug 13th, 2018                                       */
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

#ifndef ALOG_CAT_HANDLER_HEADER
#define ALOG_CAT_HANDLER_HEADER

#include <string>
#include <vector>

class ALogCatHandler
{
 public:
  ALogCatHandler();
  ~ALogCatHandler() {}

  void      setForceOverwrite()      {m_force_overwrite=true;}
  void      setVerbose()             {m_verbose=true;}
  bool      addALogFile(std::string s);

  bool      process();

 protected:
  bool      preCheck();
  bool      processFile(std::string);

 protected:
  bool        m_force_overwrite;
  bool        m_verbose;

 private:

  std::vector<std::string> m_alog_files;
  std::string  m_outfile;
};

#endif 




