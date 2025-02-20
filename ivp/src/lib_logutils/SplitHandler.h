/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SplitHandler.h                                       */
/*    DATE: Feb 2nd, 2015                                        */
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

#ifndef ALOG_SPLIT_HANDLER_HEADER
#define ALOG_SPLIT_HANDLER_HEADER

#include <vector>
#include <string>
#include <map>
#include <set>

class SplitHandler
{
 public:
  SplitHandler(std::string);
  ~SplitHandler() {}

  bool handle();
  bool handlePreCheckALogFile();
  void setVerbose(bool v)          {m_verbose=v;}
  void setProgress(bool v)         {m_progress=v;}
  void setDirectory(std::string s) {m_given_dir=s;}
  void setMaxFilePtrCache(unsigned int);
  bool addDetachedPair(std::string);
  bool addDetachedPair(std::string, std::string);
  
 protected:
  bool handlePreCheckSplitDir();
  bool handleMakeSplitFiles();
  bool handleMakeSplitSummary();

  bool handleSplitLine(const std::string& varname,
		       const std::string& rawline);
  
  std::string detached(std::string varname);
  std::set<std::string> detachedSet(std::string varname);
  
 protected: // Config variables
  std::string  m_alog_file;
  std::string  m_given_dir;
  bool         m_verbose;
  bool         m_progress;
  unsigned int m_max_cache;

  std::map<std::string, std::string> m_map_detached_pairs;
  std::map<std::string, std::set<std::string> > m_map_dpairs;
  
 protected: // State variables
  std::string m_basedir;
  std::string m_logstart;
  std::string m_time_min;
  std::string m_time_max;
  std::string m_vname;
  std::string m_vtype;
  std::string m_vcolor;
  std::string m_vlength;

  bool m_alog_file_confirmed;

  bool m_split_dir_prior;
  bool m_max_cache_exceeded;
  
  std::string m_curr_helm_iter;

  std::set<std::string> m_vip_cache;
  
  // Each map key is a MOOS variable name
  std::map<std::string, FILE*>       m_file_ptr;
  std::map<std::string, std::string> m_var_type;
  std::map<std::string, std::set<std::string> > m_var_srcs;

  // Keep track of all unique bhv names for summary file
  std::set<std::string> m_bhv_names;
  // Keep track of all unique apps with applogging for summary file
  std::set<std::string> m_applogging_app_names;
};

#endif

