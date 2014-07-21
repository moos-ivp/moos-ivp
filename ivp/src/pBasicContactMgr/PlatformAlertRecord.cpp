/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
//    Step 2: set m_par[vname][alert_id] = false for all known vnames

void PlatformAlertRecord::addAlertID(std::string alertid)
{
  // If the "column" already exists, do nothing
  if(m_alertids.count(alertid) != 0)
    return;

  // Step 1: Update the list of columns. Note there may be no rows
  // yet until a contact/vehicle has been added to this data structure
  m_alertids.insert(alertid);

  // Step 2: add the column (w/ false values) for all known vehicles
  map<string, map<string, bool> >::iterator p;
  for(p=m_par.begin(); p!=m_par.end(); p++)
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
  if(m_par.count(vehicle) != 0)
    return;

  // Create an "empty row". A map of alertid->false for all known
  // alertids.
  map<string, bool> idmap;
  set<string>::iterator p;
  for(p=m_alertids.begin(); p!=m_alertids.end(); p++) {
    string alertid = *p;
    idmap[alertid] = false;
  }
  
  // Then assign this "empty row" to the new given vehicle.
  m_par[vehicle] = idmap;
}


//---------------------------------------------------------------
// Procedure: containsVehicle
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.

bool PlatformAlertRecord::containsVehicle(const string& vehicle) const
{
  return(m_par.count(tolower(vehicle)) == 1);
}


//---------------------------------------------------------------
// Procedure: containsAlertID

bool PlatformAlertRecord::containsAlertID(const string& alertid) const
{
  return(m_alertids.count(tolower(alertid)) == 1);
}

//---------------------------------------------------------------
// Procedure: setValue
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.
//      Note: If the alertid is unknown, nothing is done.
//      Note: If the vehicle is unknown, nothing is done.

void PlatformAlertRecord::setValue(string vehicle, string alertid, 
				   bool bval)
{
  if((alertid != "all_alerts") && !containsAlertID(alertid))
    return;
  if(!containsVehicle(vehicle))
    return;

  vehicle = tolower(vehicle);

  // If the caller specifies all_allerts, 

  if(alertid != "all_alerts")
    m_par[vehicle][alertid] = bval;
  else {
    map<string, bool>::iterator p;
    for(p=m_par[vehicle].begin(); p!=m_par[vehicle].end(); p++) 
      p->second = bval;
  }
}

//---------------------------------------------------------------
// Procedure: getValue
//            Get the value of a particular element in the matrix
//            indicating whether the alert has been made (true), or
//            if the alert is pending (false).
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.

bool PlatformAlertRecord::getValue(string vehicle, string alertid) const
{
  vehicle = tolower(vehicle);

  map<string, map<string,bool> >::const_iterator p=m_par.find(vehicle);
  if(p==m_par.end())
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

string PlatformAlertRecord::getAlertedGroup(bool alerted) const
{
  string result;
  
  map<string, map<string, bool> >::const_iterator p1;
  for(p1=m_par.begin(); p1!=m_par.end(); p1++) {
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
// Procedure: alertsPending
//   Purpose: Return true if any of the (vehicle,alertid) pairs in 
//            the matrix have false value. 

bool PlatformAlertRecord::alertsPending() const
{
  map<string, map<string, bool> >::const_iterator p1;
  for(p1=m_par.begin(); p1!=m_par.end(); p1++) {
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
// Procedure: print

void PlatformAlertRecord::print() const
{
  cout << "rows:" << m_par.size() << ", cols:" << m_alertids.size() << endl;
  map<string, map<string, bool> >::const_iterator p1;
  for(p1=m_par.begin(); p1!=m_par.end(); p1++) {
    string vehicle = p1->first;
    map<string, bool> imap = p1->second;
    map<string, bool>::const_iterator p2;
    for(p2=imap.begin(); p2!=imap.end(); p2++) {
      string alertid = p2->first;
      bool bval = p2->second;

      string str = "(" + vehicle + "," + alertid + ")=" + boolToString(bval);
      cout << str << "   ";
    }
    cout << endl;
  }
}





