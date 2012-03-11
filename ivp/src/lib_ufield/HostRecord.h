/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HostRecord.h                                         */
/*    DATE: Dec 21st 2011                                        */
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

#ifndef HOST_RECORD_HEADER
#define HOST_RECORD_HEADER

#include <string>

class HostRecord
{
 public:
  HostRecord() {};
  ~HostRecord() {};

  void set(const std::string&, const std::string&,
	   const std::string&, const std::string& udp="",
	   const std::string& warp="");
  
  void setCommunity(const std::string& s) {m_community=s;};
  void setHostIP(const std::string& s)    {m_hostip=s;};
  void setPortDB(const std::string& s)    {m_port_db=s;};
  void setPortUDP(const std::string& s)   {m_port_udp=s;};
  void setKeyword(const std::string& s)   {m_keyword=s;};
  void setTimeWarp(const std::string& s)  {m_timewarp=s;};
  void setStatus(const std::string& s)    {m_status=s;};
  void setTimeStamp(const std::string& s) {m_timestamp=s;};

  std::string getCommunity() {return(m_community);};
  std::string getHostIP()    {return(m_hostip);};
  std::string getPortDB()    {return(m_port_db);};
  std::string getPortUDP()   {return(m_port_udp);};
  std::string getKeyword()   {return(m_keyword);};
  std::string getTimeWarp()  {return(m_timewarp);};
  std::string getStatus()    {return(m_status);};
  std::string getTimeStamp() {return(m_timestamp);};
    
  bool valid(const std::string& s="") const;

  std::string getSpec() const;
  std::string getSpecTerse() const;

 protected: 
  std::string  m_community;
  std::string  m_hostip;
  std::string  m_port_db;
  std::string  m_port_udp;
  std::string  m_keyword;
  std::string  m_timewarp;
  std::string  m_status;
  std::string  m_timestamp;
};

#endif 

