/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HostRecord.cpp                                       */
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

#include "HostRecord.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

void HostRecord::set(const string& community, const string& hostip,
		     const string& port_db,   const string& port_udp,
		     const string& warp)
{
  m_community = community;
  m_hostip    = hostip;
  m_port_db   = port_db;
  m_port_udp  = port_udp;
  m_timewarp  = warp;
}

//------------------------------------------------------------
// Procedure: getSpec()

string HostRecord::getSpec() const
{
  string str = getSpecTerse();
  if(m_timewarp != "")
    str += ",timewarp=" + m_timewarp;
  if(m_status != "")
    str += ",status=" + m_timewarp;
  if(m_timestamp != "")
    str += ",time=" + m_timestamp;
  if(m_key != "")
    str += ",key=" + m_key;

  return(str);
}

//------------------------------------------------------------
// Procedure: getSpecTerse()

string HostRecord::getSpecTerse() const
{
  string str = "community=" + m_community;
  if(m_hostip != "")
    str += ",hostip="   + m_hostip;
  if(m_port_db != "")
    str += ",port_db="  + m_port_db;
  if(m_port_udp != "")
    str += ",port_udp=" + m_port_udp;
  if(m_pshare_iroutes != "")
    str += ",pshare_iroutes=" + m_pshare_iroutes;
  if(m_keyword != "")
    str += ",keyword=" + m_keyword;

  if(m_hostip_alts != "") {
    string hostip_alts = m_hostip_alts;
    hostip_alts = findReplace(hostip_alts, m_hostip+",", "");
    hostip_alts = findReplace(hostip_alts, m_hostip, "");
    str += ",hostip_alts=" + hostip_alts;
  }

  return(str);
}
//---------------------------------------------------------------
// Procedure: valid
//      Note: Determines if all the required fields have been set

bool HostRecord::valid(const string& check) const
{
  if(m_community == "")
    return(false);
  if(!isValidIPAddress(m_hostip))
    return(false);
  if(!isNumber(m_port_db))
    return(false);

  if((check == "port_udp") && !isNumber(m_port_udp))
    return(false);

  if((check == "pshare_iroutes") && (m_pshare_iroutes==""))
    return(false);

  if((check == "time_warp") && !isNumber(m_timewarp))
    return(false);
  return(true);
}








