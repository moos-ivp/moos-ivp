/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PlatformAlterRecord.h                                */
/*    DATE: Apr 6th 2010                                         */
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

#ifndef PLATFORM_ALERT_RECORD_HEADER
#define PLATFORM_ALERT_RECORD_HEADER

#include <map>
#include <set>
#include <string>

class PlatformAlertRecord
{
 public:
  PlatformAlertRecord() {}
  ~PlatformAlertRecord() {}

  void addAlertID(std::string str);
  void addVehicle(std::string str);

  bool removeVehicle(std::string str);
  
  bool containsVehicle(std::string contact) const;
  bool containsAlertID(std::string alertid) const;

  void setAlertedValue(std::string contact, std::string id, bool);
  bool getAlertedValue(std::string contact, std::string id) const;

 public:
  std::string getAlertedGroup(bool alerted) const;
  unsigned int getAlertedGroupCount(bool alerted) const;

  bool alertsPending() const;

  void print() const;

 protected: 
  //     map<vehicle, map<alertid, bool>>
  //  
  //     vname   id=aa   id=bb
  //     -----   -----   -----
  //     gilda | true    false     "the gilda map"
  //     henry | false   false     "the henry map"
  //     ike   | true    false
  //     jake  | false   true


  // The first Matix indicates whether a vehicle has been alerted,
  // for all defined alert-ids.
  std::map<std::string, std::map<std::string, bool> > m_par_alerted;

  std::set<std::string> m_alertids;
};

#endif 
