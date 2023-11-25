/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactLedger.h                                      */
/*    DATE: Aug 31st 2022                                        */
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

#ifndef CONTACT_LEDGER_HEADER
#define CONTACT_LEDGER_HEADER

#include <string>
#include "NodeRecord.h"

class ContactLedger
{
public:
  ContactLedger();
  ~ContactLedger() {};

  void setCurrTimeUTC(double utc) {m_curr_time_utc=utc;}
  void extrapolate();
  
  bool processReport(std::string report);
  bool processRecord(NodeRecord record);

  bool   hasVName(std::string) const;

  double getX(std::string vname, bool extrap=true) const;
  double getY(std::string vname, bool extrap=true) const;
  double getSpeed(std::string vname) const;
  double getHeading(std::string vname) const;
  double getDepth(std::string vname) const;
  double getLat(std::string vname) const;
  double getLon(std::string vname) const;
  std::string getGroup(std::string vname) const;
  std::string getType(std::string vname) const;
  

protected:
  NodeRecord getRecord(std::string vname, bool extrap=true) const;
  void       extrapolate(std::string vname);
  
 protected: 
  std::map<std::string, NodeRecord> m_map_records_rep;
  std::map<std::string, NodeRecord> m_map_records_ext;

  double m_curr_time_utc;
};

#endif 

