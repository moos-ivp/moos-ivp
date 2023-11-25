/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HostRepUtils.cpp                                     */
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

#include <cstdlib>
#include "HostRecordUtils.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: string2HostRecord
//   Example: community=henry,hostip=192.168.1.112,port_db=9000
//            port_udp=9200,keyword=lemon,key=1

HostRecord string2HostRecord(const string& host_record_string)
{
  HostRecord empty_record;
  HostRecord new_record;

  vector<string> svector = parseString(host_record_string, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    
    if(param == "community")
      new_record.setCommunity(value);
    else if(param == "vname")
      new_record.setCommunity(value);
    else if(param == "hostip")
      new_record.setHostIP(value);
    else if(param == "port_db")
      new_record.setPortDB(value);
    else if(param == "port_udp")
      new_record.setPortUDP(value);
    else if(param == "pshare_iroutes")
      new_record.setPShareIRoutes(value);
    else if(param == "keyword")
      new_record.setKeyword(value);
    else if(param == "key")
      new_record.setKey(value);
    else if(param == "hostip_alts")
      new_record.setHostIPAlts(value);
    else if(param == "timewarp")
      new_record.setTimeWarp(value);
    else if(param == "status")
      new_record.setStatus(value);
    else if(param == "time")
      new_record.setTimeStamp(value);
  }
  
  if(!new_record.valid())
    return(empty_record);

  return(new_record);
}









