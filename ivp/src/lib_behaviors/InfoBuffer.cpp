/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: InfoBuffer.cpp                                       */
/*    DATE: Oct 12th 2004 Thanksgiving in Waterloo               */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
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
    return(m_curr_time_utc - p2->second);
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
// Procedure: sQueryDeltas

vector<string> InfoBuffer::sQueryDeltas(string var, bool& result) const
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
// Procedure: dQueryDeltas

vector<double> InfoBuffer::dQueryDeltas(string var, bool& result) const
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
// Procedure: setValue
//      Note: 

bool InfoBuffer::setValue(string var, double val, double force_utc)
{
  dmap[var] = val;
  if(force_utc == 0)
    tmap[var] = m_curr_time_utc;
  else
    tmap[var] = force_utc;

  vdmap[var].push_back(val);

  return(true);
}

//-----------------------------------------------------------
// Procedure: setValue
//

bool InfoBuffer::setValue(string var, string val, double force_utc)
{
  smap[var] = val;
  if(force_utc == 0)
    tmap[var] = m_curr_time_utc;
  else
    tmap[var] = force_utc;

  vsmap[var].push_back(val);

  return(true);
}

//-----------------------------------------------------------
// Procedure: print

void InfoBuffer::print() const
{
  cout << "InfoBuffer: " << endl;
  cout << " curr_time_utc:" << m_curr_time_utc << endl;
  
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
    cout << "  " << pt->first << ": " << m_curr_time_utc - pt->second << endl;
  }
}

















