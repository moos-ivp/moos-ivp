///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txtgram is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////

/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppCast.h                                            */
/*    DATE: June 3rd 2012                                        */
/*                                                               */


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

  std::string::size_type size() const { return (m_messages.size()); };
  std::string::size_type getCfgWarningCount() const {
    return (m_config_warnings.size());
  };

  unsigned int getIteration() const          {return (m_iteration);};
  unsigned int getRunWarningCount() const    {return(m_cnt_run_warnings);};
  unsigned int getMaxEvents() const          {return(m_max_events);};
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
