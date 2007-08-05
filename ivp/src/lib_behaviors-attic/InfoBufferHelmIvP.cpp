/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: InfoBufferHelmIvP.cpp                                */
/*    DATE: Oct 12th 2004 Thanksgiving in Waterloo               */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <map>
#include <set>
#include <stdio.h>
#include <iostream>
#include "InfoBufferHelmIvP.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: dQuery

double InfoBufferHelmIvP::dQuery(string var,
				 bool *result) const
{
  map<string, double>::const_iterator p2;
  p2 = dmap.find(var);
  if(p2 != dmap.end()) {
    if(result) *result = true;
    return(p2->second);
  }

  // If all fails, return ZERO and indicate failure.  
  if(result) *result = false;
  return(0.0);
}

//-----------------------------------------------------------
// Procedure: tQuery
//      Note: Returns the time since the given variable was
//            last updated.

double InfoBufferHelmIvP::tQuery(const string& var) const
{
  map<string, double>::const_iterator p2;
  p2 = tmap.find(var);
  if(p2 != tmap.end()) {
    return(curr_time - p2->second);
  }
  else
    return(-1);
}

//-----------------------------------------------------------
// Procedure: sQuery

string InfoBufferHelmIvP::sQuery(const string& var,
				 bool *result) const
{
  map<string, string>::const_iterator p2;
  p2 = smap.find(var);
  if(p2 != smap.end()) {
    if(result) *result = true;
    return(p2->second);
  }

  // If all fails, return empty string and indicate failure.
  if(result) *result = false;
  return("");
}

//-----------------------------------------------------------
// Procedure: svQuery

vector<string> InfoBufferHelmIvP::svQuery(const string& var,
					  bool *result) const
{
  // Have an empty vector handy for returning any kind of failure
  vector<string> empty_vector;
  
  // Find the vector associated with the given variable name
  map<string, vector<string> >::const_iterator p2;
  p2 = vsmap.find(var);
  if(p2 != vsmap.end()) {
    if(result) *result = true;
    return(p2->second);
  }

  // If all fails, return empty vector and indicate failure.
  if(result) *result = false;
  return(empty_vector);
}

//-----------------------------------------------------------
// Procedure: dvQuery

vector<double> InfoBufferHelmIvP::dvQuery(const string& var,
					  bool *result) const
{
  // Have an empty vector handy for returning any kind of failure
  vector<double> empty_vector;
  
  // Find the vector associated with the given variable name
  map<string, vector<double> >::const_iterator p2;
  p2 = vdmap.find(var);
  if(p2 != vdmap.end()) {
    if(result) *result = true;
    return(p2->second);
  }
  
  // If all fails, return empty vector and indicate failure.
  if(result) *result = false;
  return(empty_vector);
}

//-----------------------------------------------------------
// Procedure: clearDeltaVectors

void InfoBufferHelmIvP::clearDeltaVectors()
{
  vsmap.clear();
  vdmap.clear();
}

//-----------------------------------------------------------
// Procedure: set_val
//      Note: 

bool InfoBufferHelmIvP::set_val(const string& var, double val)
{
  dmap[var] = val;
  tmap[var] = curr_time;

  vdmap[var].push_back(val);
  vdmap[var].push_back(val);

  return(true);
}

//-----------------------------------------------------------
// Procedure: set_val
//      Note: Handles the following types of inputs:
//            HELM_SUMMARY
//

bool InfoBufferHelmIvP::set_val(const string& var, const string& val)
{
  smap[var] = val;
  tmap[var] = curr_time;
  smap[var] = val;
  tmap[var] = curr_time;

  vsmap[var].push_back(val);
  vsmap[var].push_back(val);

  return(true);
}

//-----------------------------------------------------------
// Procedure: print

void InfoBufferHelmIvP::print() const
{
  cout << "InfoBuffer: " << endl;
  cout << " curr_time:" << curr_time << endl;
  
  map<string, string>::const_iterator p0;
  map<string, double>::const_iterator p1;
  cout << "-----------------------------------------------" << endl; 
  cout << " String Data: " << endl;
  for(p0=smap.begin(); p0!=smap.end(); p0++) 
    cout << "  " << p0->first << ": " << p0->second << endl;
  
  cout << "-----------------------------------------------" << endl; 
  cout << " Numerical Data: " << endl;
  for(p1=dmap.begin(); p1!=dmap.end(); p1++)
    cout << "  " << p1->first << ": " << p1->second << endl;

  cout << "-----------------------------------------------" << endl; 
  cout << " Time Data: " << endl;
  for(p1=tmap.begin(); p1!=tmap.end(); p1++) {
    cout << "  " << p1->first << ": " << p1->second << endl;
    cout << "  " << p1->first << ": " << curr_time - p1->second << endl;
  }
}
















