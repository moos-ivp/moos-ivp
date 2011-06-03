/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PlatformAlterRecord.h                                */
/*    DATE: Apr 6th 2010                                         */
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

#ifndef PLATFORM_ALERT_RECORD_HEADER
#define PLATFORM_ALERT_RECORD_HEADER

#include <map>
#include <string>

class PlatformAlertRecord
{
 public:
  PlatformAlertRecord();
  ~PlatformAlertRecord() {};

  void print();

  void addAlertID(std::string str);
  void addVehicle(std::string str);

  void setValue(std::string vehicle, std::string alertid, bool);
  bool getValue(std::string vehicle, std::string alertid);

  bool containsVehicle(std::string vehicle);
  bool containsAlertID(std::string alertid);

  std::string getAlertedGroup(bool alerted);

  bool alertsPending();


 protected: 
  // map<vehicle, map<alertid, bool>>
  std::map<std::string, std::map<std::string, bool> > m_par;

  std::map<std::string, bool> m_temp_row;

  unsigned int m_rows;
  unsigned int m_cols;
};

#endif 

