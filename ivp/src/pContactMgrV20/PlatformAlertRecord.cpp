/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PlatformAlertRecord.cpp                              */
/*    DATE: Feb 27th 2010                                        */
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

#include <iostream>
#include "PlatformAlertRecord.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: addAlertID
//    Step 1: add it to a list of alert_ids
//    Step 2: set m_par_alerted[vname][alert_id] = false for all
//            known vnames

void PlatformAlertRecord::addAlertID(std::string alertid)
{
  // If the "column" already exists, do nothing
  if(m_alertids.count(alertid) != 0)
    return;

  // Step 1: Update the list of columns (ids). Note there may be no rows
  // yet until a contact/vehicle has been added to this data structure
  m_alertids.insert(alertid);

  // Step 2: add the column (w/ false values) for all known vehicles
  map<string, map<string, bool> >::iterator p;
  // For the alerted matrix
  for(p=m_par_alerted.begin(); p!=m_par_alerted.end(); p++)
    p->second[alertid] = false;
}

//---------------------------------------------------------------
// Procedure: addVehicle
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.

void PlatformAlertRecord::addVehicle(string vehicle)
{
  vehicle = tolower(vehicle);

  // First check to see if the row/vehicle already exists.
  if(m_par_alerted.count(vehicle) != 0)
    return;

  // Create an "empty row". A map of alertid->false for all ids.
  map<string, bool> idmap;
  set<string>::iterator p;
  for(p=m_alertids.begin(); p!=m_alertids.end(); p++) {
    string alertid = *p;
    idmap[alertid] = false;
  }
  
  // Then assign this "empty row" to the new given vehicle.
  // For the alerted matrix
  m_par_alerted[vehicle] = idmap;
}


//---------------------------------------------------------------
// Procedure: removeVehicle()
//   Returns: true if the vehicle name existed prior to removal

bool PlatformAlertRecord::removeVehicle(string vehicle)
{
  vehicle = tolower(vehicle);

  // First check to see if the row/vehicle already exists.
  if(m_par_alerted.count(vehicle) == 0)
    return(false);

  m_par_alerted.erase(vehicle);
  return(true);
}


//---------------------------------------------------------------
// Procedure: containsVehicle
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.

bool PlatformAlertRecord::containsVehicle(string vehicle) const
{
  return(m_par_alerted.count(tolower(vehicle)) == 1);
}


//---------------------------------------------------------------
// Procedure: getAlertsTotal()

unsigned int PlatformAlertRecord::getAlertsTotal(string contact) const
{
  if(!m_map_alerts_total.count(contact))
    return(0);

  return(m_map_alerts_total.at(contact));
}

//---------------------------------------------------------------
// Procedure: getAlertsActive()

unsigned int PlatformAlertRecord::getAlertsActive(string contact) const
{
  if(!m_map_alerts_active.count(contact))
    return(0);

  return(m_map_alerts_active.at(contact));
}


//---------------------------------------------------------------
// Procedure: containsAlertID

bool PlatformAlertRecord::containsAlertID(string alertid) const
{
  return(m_alertids.count(tolower(alertid)) == 1);
}

//---------------------------------------------------------------
// Procedure: setAlertedValue
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.
//      Note: If the alertid is unknown, nothing is done.
//      Note: If the vehicle is unknown, nothing is done.

void PlatformAlertRecord::setAlertedValue(string vehicle, string alertid, 
					  bool bval)
{
  if(!containsAlertID(alertid) || !containsVehicle(vehicle))
    return;
  vehicle = tolower(vehicle);

  // If turning the alert on
  if(bval && !m_par_alerted[vehicle][alertid]) {
    m_map_alerts_total[vehicle]++;
    m_map_alerts_active[vehicle]++;
  }
  // If turning the alert off
  else if(!bval && m_par_alerted[vehicle][alertid]) {
    if(m_map_alerts_active[vehicle] > 0)
      m_map_alerts_active[vehicle]--;
  }

  m_par_alerted[vehicle][alertid] = bval;

}

//---------------------------------------------------------------
// Procedure: getAlertedValue
//            Get the value of a particular element in the matrix
//            indicating whether the alert has been made (true), or
//            if the alert is pending (false).
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.

bool PlatformAlertRecord::getAlertedValue(string vehicle, string alertid) const
{
  vehicle = tolower(vehicle);

  map<string, map<string,bool> >::const_iterator p=m_par_alerted.find(vehicle);
  if(p==m_par_alerted.end())
    return(false);
  else {
    map<string, bool> imap = p->second;
    map<string,bool>::const_iterator q=imap.find(alertid);
    if(q==imap.end())
      return(false);
    else
      return(q->second);
  }
}


//---------------------------------------------------------------
// Procedure: getAlertedGroup
//   Purpose: Get a string report on the alert status for the 
//            matrix, depending on whether one wants the list of 
//            pairs for which alerts have been made (alerted=true), 
//            or the opposite case where alerts are pending.
//   Returns: (henry,abc)(gilda,xyz)


string PlatformAlertRecord::getAlertedGroup(bool alerted) const
{
  string result;
  
  map<string, map<string, bool> >::const_iterator p1;
  for(p1=m_par_alerted.begin(); p1!=m_par_alerted.end(); p1++) {
    string vehicle = p1->first;
    map<string, bool> imap = p1->second;
    map<string, bool>::const_iterator p2;
    for(p2=imap.begin(); p2!=imap.end(); p2++) {
      string alertid = p2->first;
      bool bval = p2->second;
      if(alerted==bval) {
	string entry = "(" + vehicle + "," + alertid + ")";
	result += entry;
      }
    }
  }
  return(result);
}

//---------------------------------------------------------------
// Procedure: getAlertedGroupCount

unsigned int PlatformAlertRecord::getAlertedGroupCount(bool alerted) const
{
  unsigned int count = 0;
  
  map<string, map<string, bool> >::const_iterator p1;
  for(p1=m_par_alerted.begin(); p1!=m_par_alerted.end(); p1++) {
    string vehicle = p1->first;
    map<string, bool> imap = p1->second;
    map<string, bool>::const_iterator p2;
    for(p2=imap.begin(); p2!=imap.end(); p2++) {
      string alertid = p2->first;
      bool bval = p2->second;
      if(alerted==bval) {
	count++;
      }
    }
  }
  return(count);
}

//---------------------------------------------------------------
// Procedure: alertsPending
//   Purpose: Return true if any of the (vehicle,alertid) pairs in 
//            the matrix have false value. 

bool PlatformAlertRecord::alertsPending() const
{
  map<string, map<string, bool> >::const_iterator p1;
  for(p1=m_par_alerted.begin(); p1!=m_par_alerted.end(); p1++) {
    string vehicle = p1->first;
    map<string, bool> imap = p1->second;
    map<string, bool>::const_iterator p2;
    for(p2=imap.begin(); p2!=imap.end(); p2++) {
      bool bool_val = p2->second;
      if(bool_val == false)
	return(true);
    }
  }
  return(false);
}

//---------------------------------------------------------------
// Procedure: getReports
//   Purpose: Reports to be posted by the contact manager

vector<VarDataPair> PlatformAlertRecord::getReports() const
{
  vector<VarDataPair> rvector;

  return(rvector);  
}

//---------------------------------------------------------------
// Procedure: print

void PlatformAlertRecord::print() const
{
  unsigned int idcount = m_alertids.size();

  //=====================================================
  // Part 1: Print the alerted matrix
  //=====================================================
  cout << "Alerted Matrix" << endl;
  cout << "   rows:" << m_par_alerted.size() << ", cols:" << idcount << endl;
  map<string, map<string, bool> >::const_iterator p1;
  for(p1=m_par_alerted.begin(); p1!=m_par_alerted.end(); p1++) {
    string vname = p1->first;
    cout << "   " << vname << ": ";

    map<string, bool> imap = p1->second;
    map<string, bool>::const_iterator p2;
    for(p2=imap.begin(); p2!=imap.end(); p2++) {
      string alertid = p2->first;
      bool bval = p2->second;

      string str = "(" + alertid + "=" + boolToString(bval) + ")";
      cout << str << "   ";
    }
    cout << endl;
  }
}

