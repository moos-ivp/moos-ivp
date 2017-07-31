/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogClipHandler.h                                    */
/*    DATE: June 11th, 2015                                      */
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

#ifndef ALOG_CLIP_HANDLER_HEADER
#define ALOG_CLIP_HANDLER_HEADER

#include <string>
#include <vector>

class ALogClipHandler
{
 public:
  ALogClipHandler();
  ~ALogClipHandler() {}

  void      setForceOverwrite()      {m_force_overwrite=true;}
  void      setVerbose()             {m_verbose=true;}
  void      setBatch()               {m_batch=true;}
  bool      setSuffix(std::string s);
  bool      setTimeStamp(double);
  bool      addALogFile(std::string s);

  bool      process();

 protected:
  bool      preCheck();
  bool      processBatch();
  bool      processFile(std::string in, std::string out);

  std::string addSuffixToALogFile(std::string);

 protected:
  double      m_min_time; 
  double      m_max_time;
  bool        m_min_time_set;
  bool        m_max_time_set;

  bool        m_force_overwrite;
  bool        m_verbose;
  bool        m_batch;
  std::string m_suffix;

 private:

  // For batch mode
  std::vector<std::string> m_batch_files;

  // For non-batch mode
  std::string  m_infile;
  std::string  m_outfile;
};

#endif 




