/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PlatformAlertRecord.cpp                              */
/*    DATE: Feb 27th 2010                                        */
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

#include <iostream>
#include "PlatformAlertRecord.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

PlatformAlertRecord::PlatformAlertRecord()
{
  // Create a temporary "row in waiting" with just the no_id col. 
  m_temp_row["no_id"] = false;

  m_rows = 0;
  m_cols = 0;  
}

//---------------------------------------------------------------
// Procedure: print

void PlatformAlertRecord::print()
{
  cout << "rows:" << m_rows << ", cols:" << m_cols << endl;
  map<string, map<string, bool> >::iterator p1;
  for(p1=m_par.begin(); p1!=m_par.end(); p1++) {
    string vehicle = p1->first;
    map<string, bool> imap = p1->second;
    map<string, bool>::iterator p2;
    for(p2=imap.begin(); p2!=imap.end(); p2++) {
      string alertid = p2->first;
      bool bval = p2->second;

      string str = "(" + vehicle + "," + alertid + ")=" + boolToString(bval);
      cout << str << "   ";
    }
    cout << endl;
  }
}

//---------------------------------------------------------------
// Procedure: addAlertID

void PlatformAlertRecord::addAlertID(std::string alertid)
{
  // If there are no rows, add the alertid to the "row in waiting".
  if(m_rows == 0) {
    m_temp_row[alertid] = false;
    return;
  }

  unsigned int cols = 0;
  map<string, map<string, bool> >::iterator p1;
  for(p1=m_par.begin(); p1!=m_par.end(); p1++) {
    p1->second[alertid] = false;
    cols = p1->second.size();
  }  
  m_cols = cols;
}

//---------------------------------------------------------------
// Procedure: addVehicle
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.

void PlatformAlertRecord::addVehicle(string vehicle)
{
  vehicle = tolower(vehicle);

  // First check to see if the row/vehicle already exists.
  map<string, map<string, bool> >::iterator p1;
  p1 = m_par.find(vehicle);
  if(p1 != m_par.end())
    return;

  map<string, bool> new_row;

  // If this is the first row, use the "row in waiting"
  if(m_rows == 0) {
    new_row = m_temp_row;
    m_cols = m_temp_row.size();
    m_temp_row.clear();
  }
  else
    new_row = m_par.begin()->second;

  map<string, bool>::iterator p;
  for(p=new_row.begin(); p!=new_row.end(); p++)
    p->second = false;
  m_par[vehicle] = new_row;

  m_rows = m_par.size();
}


//---------------------------------------------------------------
// Procedure: containsVehicle
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.

bool PlatformAlertRecord::containsVehicle(string vehicle)
{
  vehicle = tolower(vehicle);

  map<string, map<string, bool> >::iterator p1;
  p1 = m_par.find(vehicle);
  return(p1 != m_par.end());
}


//---------------------------------------------------------------
// Procedure: containsAlertID

bool PlatformAlertRecord::containsAlertID(string alertid)
{
  map<string, bool> some_row = m_par.begin()->second;
  
  map<string, bool>::iterator p;
  p = some_row.find(alertid);
  return(p != some_row.end());
}

//---------------------------------------------------------------
// Procedure: setValue
//      Note: The vehicles names are case insensitive. They are
//            converted and regarded thereafter all in lowercase.

void PlatformAlertRecord::setValue(string vehicle, string alertid, 
				   bool bval)
{
  vehicle = tolower(vehicle);

  if(!containsVehicle(vehicle))
    return;
  if((alertid != "all_alerts") && !containsAlertID(alertid))
    return;

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

bool PlatformAlertRecord::getValue(string vehicle, string alertid)
{
  vehicle = tolower(vehicle);

  if(!containsVehicle(vehicle))
    return(false);
  if(!containsAlertID(alertid))
    return(false);

  return(m_par[vehicle][alertid]);
}

//---------------------------------------------------------------
// Procedure: getAlertedGroup
//   Purpose: Get a string report on the alert status for the 
//            matrix, depending on whether one wants the list of 
//            pairs for which alerts have been made (alerted=true), 
//            or the opposite case where alerts are pending.

string PlatformAlertRecord::getAlertedGroup(bool alerted)
{
  string result;
  
  map<string, map<string, bool> >::iterator p1;
  for(p1=m_par.begin(); p1!=m_par.end(); p1++) {
    string vehicle = p1->first;
    map<string, bool> imap = p1->second;
    map<string, bool>::iterator p2;
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

bool PlatformAlertRecord::alertsPending()
{
  map<string, map<string, bool> >::iterator p1;
  for(p1=m_par.begin(); p1!=m_par.end(); p1++) {
    string vehicle = p1->first;
    map<string, bool> imap = p1->second;
    map<string, bool>::iterator p2;
    for(p2=imap.begin(); p2!=imap.end(); p2++) {
      bool bool_val = p2->second;
      if(bool_val == false)
	return(true);
    }
  }
  return(false);
}

