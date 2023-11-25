/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PipeWay.h                                            */
/*    DATE: Dec 21st 2020                                        */
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

#ifndef PIPEWAY_HEADER
#define PIPEWAY_HEADER

#include <string>
#include <map>
#include <set>

class PipeWay
{
 public:
  PipeWay();
  ~PipeWay() {}

  // Setters
  void setShowSource(bool v=true)        {m_show_source=v;}
  void setShowCommunity(bool v=true)     {m_show_community=v;}
  void setShowSubscriptions(bool v=true) {m_show_subscriptions=v;}
  void setShowMasked(bool v=true)        {m_show_masked=v;}
  void setWrapContent(bool v=false)      {m_wrap_content=v;}
  void setTruncContent(bool v=false)     {m_trunc_content=v;}
  void setTimeFormatUTC(bool v=false)    {m_time_format_utc=v;}
  void setForceRefresh(bool v=false)     {m_force_refresh=v;}

  void setClient(std::string s)   {m_client=s;}
  void setChannel(std::string s)  {m_channel=s;}
  void addVar(std::string var)    {m_vars.insert(var);}
  void setDuration(double v)      {m_duration=v;}
  void setExpireTime(double v)    {m_expire_time=v;}

  void setModified()              {m_modified=true;}

  void setStartExpTime(double start_time, double warp=1);
  
  // Getters
  bool showSource() const         {return(m_show_source);}
  bool showCommunity() const      {return(m_show_community);}
  bool showSubscriptions() const  {return(m_show_subscriptions);}
  bool showMasked() const         {return(m_show_masked);}
  bool wrapContent() const        {return(m_wrap_content);}
  bool truncContent() const       {return(m_trunc_content);}
  bool timeFormatUTC() const      {return(m_time_format_utc);}
  bool forceRefresh() const       {return(m_force_refresh);}
  
  double      getDuration() const {return(m_duration);}
  std::string getClient() const   {return(m_client);}
  std::string getChannel() const  {return(m_channel);}

  std::set<std::string> getVars() const {return(m_vars);}

  // Analyzers
  std::string getContentDescriptor(unsigned int trunclen=40) const;
  
  bool isChanneled() {return(m_channel!="");}

  double timeUntilExpire(double curr_time) const;
  bool   modified();
  
  bool valid() const;

  bool operator==(const PipeWay&);
  bool operator!=(const PipeWay&);
  
private: // Config Vars
  std::string m_client;
  
  bool  m_show_source;
  bool  m_show_community;
  bool  m_wrap_content;

  bool  m_show_subscriptions;
  bool  m_show_masked;
  bool  m_trunc_content;
  bool  m_time_format_utc;

  bool  m_force_refresh;
  double      m_duration;
  std::string m_channel;

  std::set<std::string> m_vars;

private: // State Vars
  bool    m_modified;
  double  m_expire_time;
};

PipeWay string2PipeWay(std::string);

#endif 

