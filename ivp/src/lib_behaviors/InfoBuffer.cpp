/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: InfoBuffer.cpp                                       */
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

#include <iostream>
#include "InfoBuffer.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: dQuery

double InfoBuffer::dQuery(string var, bool& result) const
{
  map<string, double>::const_iterator p2;
  p2 = dmap.find(var);
  if(p2 != dmap.end()) {
    result = true;
    return(p2->second);
  }
  
  // If all fails, return ZERO and indicate failure.  
  result = false;
  return(0.0);
}

//-----------------------------------------------------------
// Procedure: tQuery
//      Note: Returns the time since the given variable was
//            last updated.

double InfoBuffer::tQuery(string var) const
{
  map<string, double>::const_iterator p2;
  p2 = tmap.find(var);
  if(p2 != tmap.end()) {
    return(m_curr_time - p2->second);
  }
  else
    return(-1);
}

//-----------------------------------------------------------
// Procedure: sQuery

string InfoBuffer::sQuery(string var, bool& result) const
{
  map<string, string>::const_iterator p2;
  p2 = smap.find(var);
  if(p2 != smap.end()) {
    result = true;
    return(p2->second);
  }
  
  // If all fails, return empty string and indicate failure.
  result = false;
  return("");
}

//-----------------------------------------------------------
// Procedure: svQuery

vector<string> InfoBuffer::svQuery(string var, bool& result) const
{
  // Have an empty vector handy for returning any kind of failure
  vector<string> empty_vector;
  
  // Find the vector associated with the given variable name
  map<string, vector<string> >::const_iterator p2;
  p2 = vsmap.find(var);
  if(p2 != vsmap.end()) {
    result = true;
    return(p2->second);
  }
  
  // If all fails, return empty vector and indicate failure.
  result = false;
  return(empty_vector);
}

//-----------------------------------------------------------
// Procedure: dvQuery

vector<double> InfoBuffer::dvQuery(string var, bool& result) const
{
  // Have an empty vector handy for returning any kind of failure
  vector<double> empty_vector;
  
  // Find the vector associated with the given variable name
  map<string, vector<double> >::const_iterator p2;
  p2 = vdmap.find(var);
  if(p2 != vdmap.end()) {
    result = true;
    return(p2->second);
  }
  
  // If all fails, return empty vector and indicate failure.
  result = false;
  return(empty_vector);
}

//-----------------------------------------------------------
// Procedure: clearDeltaVectors

void InfoBuffer::clearDeltaVectors()
{
  vsmap.clear();
  vdmap.clear();
}

//-----------------------------------------------------------
// Procedure: set_val
//      Note: 

bool InfoBuffer::set_val(string var, double val)
{
  dmap[var] = val;
  tmap[var] = m_curr_time;

  vdmap[var].push_back(val);
  vdmap[var].push_back(val);

  return(true);
}

//-----------------------------------------------------------
// Procedure: set_val
//      Note: Handles the following types of inputs:
//            HELM_SUMMARY
//

bool InfoBuffer::set_val(string var, string val)
{
  smap[var] = val;
  tmap[var] = m_curr_time;

  vsmap[var].push_back(val);
  vsmap[var].push_back(val);

  return(true);
}

//-----------------------------------------------------------
// Procedure: print

void InfoBuffer::print() const
{
  cout << "InfoBuffer: " << endl;
  cout << " curr_time:" << m_curr_time << endl;
  
  cout << "-----------------------------------------------" << endl; 
  cout << " String Data: " << endl;
  map<string, string>::const_iterator ps;
  for(ps=smap.begin(); ps!=smap.end(); ps++)
    cout << "  " << ps->first << ": " << ps->second << endl;
  
  cout << "-----------------------------------------------" << endl; 
  cout << " Numerical Data: " << endl;
  map<string, double>::const_iterator pd;
  for(pd=dmap.begin(); pd!=dmap.end(); pd++) 
    cout << "  " << pd->first << ": " << pd->second << endl;
  
  cout << "-----------------------------------------------" << endl; 
  cout << " Time Data: " << endl;
  map<string, double>::const_iterator pt;
  for(pt=tmap.begin(); pt!=tmap.end(); pt++) {
    cout << "  " << pt->first << ": " << pt->second << endl;
    cout << "  " << pt->first << ": " << m_curr_time - pt->second << endl;
  }
}
















