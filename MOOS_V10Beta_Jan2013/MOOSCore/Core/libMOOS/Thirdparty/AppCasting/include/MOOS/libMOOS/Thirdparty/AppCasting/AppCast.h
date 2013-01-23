/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCast.h                                            */
/*    DATE: June 3rd 2012                                        */
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

#ifndef APP_CAST_BROADCAST_HEADER
#define APP_CAST_BROADCAST_HEADER

#include <vector>
#include <list>
#include <map>
#include <string>

class AppCast
{
 public:
  AppCast();
  virtual ~AppCast() {};

  void  msg(const std::string& str)          {m_messages = str;};
  void  event(std::string, double=-1);
  void  runWarning(const std::string&);
  bool  retractRunWarning(const std::string&);
  void  cfgWarning(const std::string&);
  void  setProcName(std::string s)           {m_proc_name=s;}; 
  void  setNodeName(std::string s)           {m_node_name=s;}; 
  void  setIteration(unsigned int v)         {m_iteration=v;};
  void  setMaxEvents(unsigned int v)         {m_max_events=v;};
  void  setMaxRunWarnings(unsigned int v)    {m_max_run_warnings=v;};

  unsigned int getIteration() const          {return(m_iteration);};
  unsigned int size() const                  {return(m_messages.size());};
  unsigned int getRunWarningCount() const    {return(m_cnt_run_warnings);};
  unsigned int getCfgWarningCount() const    {return(m_config_warnings.size());};
  std::string  getProcName() const           {return(m_proc_name);};
  std::string  getNodeName() const           {return(m_node_name);};

  std::string  getAppCastString() const;
  std::string  getFormattedString(bool with_header=true) const;

 public: // Used for rebuilding an AppCast from String
  void  setRunWarnings(const std::string&, unsigned int);
  void  setRunWarningCount(unsigned int v)  {m_cnt_run_warnings=v;};

 protected: // Configuration vars
  std::string              m_proc_name;
  std::string              m_node_name;
  unsigned int             m_max_events;
  unsigned int             m_max_run_warnings;
  unsigned int             m_max_config_warnings;

 protected: // State vars
  unsigned int             m_iteration;

  // AppCast holds all (unlimited) messages, and config warnings
  std::string              m_messages;
  std::vector<std::string> m_config_warnings;

  // AppCast holds limited number of runtime warnings to prevent 
  // runaway size. Keeps count of warnings of identical content.
  std::map<std::string, unsigned int> m_map_run_warnings;
  unsigned int                        m_cnt_run_warnings;

  // AppCast holds limited number of event messages.
  std::list<std::string> m_events;
};

AppCast string2AppCast(const std::string&);

#endif
