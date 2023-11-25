/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HostRecord.h                                         */
/*    DATE: Dec 21st 2011                                        */
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

#ifndef HOST_RECORD_HEADER
#define HOST_RECORD_HEADER

#include <string>

class HostRecord
{
 public:
  HostRecord() {}
  ~HostRecord() {}

  void set(const std::string&, const std::string&,
	   const std::string&, const std::string& udp="",
	   const std::string& warp="");
  
  void setCommunity(const std::string& s)  {m_community=s;}
  void setHostIP(const std::string& s)     {m_hostip=s;}
  void setHostIPAlts(const std::string& s) {m_hostip_alts=s;}
  void setPortDB(const std::string& s)     {m_port_db=s;}
  void setPortUDP(const std::string& s)    {m_port_udp=s;}
  void setKeyword(const std::string& s)    {m_keyword=s;}
  void setKey(const std::string& s)        {m_key=s;}
  void setTimeWarp(const std::string& s)   {m_timewarp=s;}
  void setStatus(const std::string& s)     {m_status=s;}
  void setTimeStamp(const std::string& s)  {m_timestamp=s;}
  void setPShareIRoutes(std::string);

  std::string getCommunity() const  {return(m_community);}
  std::string getHostIP() const     {return(m_hostip);}
  std::string getHostIPAlts() const {return(m_hostip_alts);}
  std::string getPortDB() const     {return(m_port_db);}
  std::string getPortUDP() const    {return(m_port_udp);}
  std::string getKeyword() const    {return(m_keyword);}
  std::string getKey() const        {return(m_key);}
  std::string getTimeWarp() const   {return(m_timewarp);}
  std::string getStatus() const     {return(m_status);}
  std::string getTimeStamp() const  {return(m_timestamp);}
  std::string getPShareIRoutes() const {return(m_pshare_iroutes);}
    
  bool valid(const std::string& s="") const;

  std::string getSpec() const;
  std::string getSpecTerse() const;

 protected: 
  std::string  m_community;
  std::string  m_hostip;
  std::string  m_hostip_alts;
  std::string  m_port_db;
  std::string  m_port_udp;
  std::string  m_pshare_iroutes;
  std::string  m_keyword;
  std::string  m_key;
  std::string  m_timewarp;
  std::string  m_status;
  std::string  m_timestamp;
};

#endif 










