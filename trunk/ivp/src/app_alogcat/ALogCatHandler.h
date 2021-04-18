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
  void      setVerbose();

  bool      addALogFile(std::string s);
  bool      setNewALogFile(std::string s) {m_outfile=s; return(true);}

  bool      process();

  unsigned int size() const {return(m_alog_files.size());}
  
 protected:
  bool      preCheck();
  bool      processFirstFile();
  bool      processOtherFiles();

 protected:
  bool        m_force_overwrite;
  bool        m_verbose;
  bool        m_debug;

 private:
  std::vector<std::string> m_alog_files;

  std::vector<double>      m_utc_log_start_times;    
  std::vector<double>      m_utc_data_start_times;   
  std::vector<double>      m_utc_data_end_times;     
  std::string  m_outfile;

  FILE *m_file_out;
};

#endif 




