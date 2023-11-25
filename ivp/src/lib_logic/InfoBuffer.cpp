/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: InfoBuffer.cpp                                       */
/*    DATE: Oct 12th 2004 Thanksgiving in Waterloo               */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include "InfoBuffer.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: dQuery()

double InfoBuffer::dQuery(string var, bool& result) const
{
  map<string, double>::const_iterator p2;
  p2 = dmap.find(var);
  if(p2 != dmap.end()) {
    result = true;
    return(p2->second);
  }

  // Added by mikerb Apr 9th, 2021.  For vars ending in _DELTA, for
  // example MARK_DELTA. If MARK_DELTA is not known to the InfoBuffer,
  // then check if MARK is known, and treat it as a UTC
  // timestamp. Then return delta time since that time stamp as the
  // value of MARK_DELTA.
  if(strEnds(var, "_DELTA") && (var.length() > 6)) {
    rbiteString(var, '_');
    // Handle case if the base variable is of type double
    map<string, double>::const_iterator p = dmap.find(var);
    if(p != dmap.end()) {
      result = true;
      double var_utc = p->second;
      double delta = m_curr_time_utc - var_utc;
      //cout << " delta: " << delta << endl;
      return(delta);
    }
    
    // Handle case if the base variable is of type string
    map<string, string>::const_iterator q = smap.find(var);
    if(q != smap.end()) {
      string sval = q->second;
      if(isNumber(sval)) {
	double var_utc = atof(sval.c_str());
	double delta = m_curr_time_utc - var_utc;
	//cout << " delta: " << delta << endl;
	result = true;
	return(delta);
      }
    }
  }
  
  // If all fails, return ZERO and indicate failure.  
  result = false;
  return(0.0);
}

//-----------------------------------------------------------
// Procedure: tQuery()
//      Note: Returns the time since the given variable was
//            last updated.

double InfoBuffer::tQuery(string var, bool elapsed) const
{
  map<string, double>::const_iterator p2;
  p2 = tmap.find(var);
  if(p2 != tmap.end()) {
    if(elapsed)
      return(m_curr_time_utc - p2->second);
    else
      return(p2->second);
  }
  else
    return(-1);
}

//-----------------------------------------------------------
// Procedure: mtQuery()
//      Note: Returns the time since the given variable was
//            last updated.

double InfoBuffer::mtQuery(string var, bool elapsed) const
{
  map<string, double>::const_iterator p;
  p = mtmap.find(var);
  if(p != mtmap.end()) {
    if(elapsed)
      return(m_curr_time_utc - p->second);
    else
      return(p->second);
  }
  else
    return(-1);
}

//-----------------------------------------------------------
// Procedure: sQuery()

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
// Procedure: sQueryDeltas()

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
// Procedure: dQueryDeltas()

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
// Procedure: isKnown()
//   Purpose: Check whether the given variable was ever posted
//            to the info buffer. Regardless of whether it was
//            posted as a string or a double, it is registered
//            in the mapping from varname to timestamp.
              
bool InfoBuffer::isKnown(string varname) const
{
  map<string, double>::const_iterator p=tmap.find(varname);
  if(p != tmap.end())
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: size()
//   Purpose: Get the total size of the info_buffer
//      Note: This just counts elements, and not size of elements.
//            For example, a string counts as "1" regardless of len. 

unsigned long int InfoBuffer::size() const
{
  unsigned long int total = 0;

  total += smap.size();
  total += dmap.size();
  total += tmap.size();
  total += mtmap.size();

  map<string, vector<string> >::const_iterator p;
  for(p=vsmap.begin(); p!= vsmap.end(); p++)
    total += p->second.size();
  map<string, vector<double> >::const_iterator q;
  for(q=vdmap.begin(); q!= vdmap.end(); q++)
    total += q->second.size();

  return(total);
}

//-----------------------------------------------------------
// Procedure: sizeFull()
//   Purpose: Get the total size of the info_buffer
//      Note: This just counts elements, and not size of elements.
//            For example, a string counts as "1" regardless of len. 

unsigned long int InfoBuffer::sizeFull() const
{
  unsigned long int total = 0;

  total += smap.size();
  total += dmap.size();
  total += tmap.size();
  total += mtmap.size();

  map<string, vector<string> >::const_iterator p;
  for(p=vsmap.begin(); p!= vsmap.end(); p++)
    total += p->second.size();
  map<string, vector<double> >::const_iterator q;
  for(q=vdmap.begin(); q!= vdmap.end(); q++)
    total += q->second.size();

  return(total);
}


//-----------------------------------------------------------
// Procedure: setValue()
//      Note: msg_time is the timestamp embedded in the incoming 
//            message, vs. the time stamp of when this buffer is 
//            beig updated.

bool InfoBuffer::setValue(string var, double val, double msg_time)
{
  dmap[var] = val;
  tmap[var] = m_curr_time_utc;

  // msg_time is the timestamp perhaps embedded in the incoming message, 
  // vs. the buffer update time (the time at which the info_buffer is 
  // undergoing a round of updates. If msg_time is unspecified (0) then 
  // set it to the buffer update time.
  if(msg_time == 0)
    msg_time = m_curr_time_utc;
  mtmap[var] = msg_time;

  vdmap[var].push_back(val);

  return(true);
}

//-----------------------------------------------------------
// Procedure: setValue()

bool InfoBuffer::setValue(string var, string val, double msg_time)
{
  smap[var] = val;
  tmap[var] = m_curr_time_utc;

  // msg_time is the timestamp perhaps embedded in the incoming message, 
  // vs. the buffer update time (the time at which the info_buffer is 
  // undergoing a round of updates. If msg_time is unspecified (0) then 
  // set it to the buffer update time.
  if(msg_time == 0)
    msg_time = m_curr_time_utc;
  mtmap[var] = msg_time;

  vsmap[var].push_back(val);

  return(true);
}

//-----------------------------------------------------------
// Procedure: clearDeltaVectors()

void InfoBuffer::clearDeltaVectors()
{
  vsmap.clear();
  vdmap.clear();
}

//-----------------------------------------------------------
// Procedure: print()

void InfoBuffer::print(string vars_str) const
{
  vector<string> vars = parseString(vars_str, ',');

  cout << "Print Variables: " << endl;
  for(unsigned int i=0; i<vars.size(); i++) 
    cout << "  [" << vars[i] << "]" << endl;
  
  
  cout << "InfoBuffer: " << endl;
  cout << " curr_time_utc:" << m_curr_time_utc << endl;
  
  cout << "-----------------------------------------------" << endl; 
  cout << " String Data: " << endl;
  map<string, string>::const_iterator ps;
  for(ps=smap.begin(); ps!=smap.end(); ps++) {
    string var = ps->first;
    string val = ps->second;
    if((vars.size() == 0) || vectorContains(vars, var))
      cout << "  " << var << ": " << val << endl;
  }
  
  cout << "-----------------------------------------------" << endl; 
  cout << " Numerical Data: " << endl;
  map<string, double>::const_iterator pd;
  for(pd=dmap.begin(); pd!=dmap.end(); pd++) {
    string var = pd->first;
    double val = pd->second;
    if((vars.size() == 0) || vectorContains(vars, var))
      cout << "  " << var << ": " << val << endl;
  }

  cout << "-----------------------------------------------" << endl; 
  cout << " Time Data: " << endl;
  map<string, double>::const_iterator pt;
  for(pt=tmap.begin(); pt!=tmap.end(); pt++) {
    string var = pt->first;
    if((vars.size() == 0) || vectorContains(vars, var))
      cout << "  " << var << ": " << m_curr_time_utc - pt->second << endl;
  }
}


//-----------------------------------------------------------
// Procedure: getReport()
//   Purpose: Get an info_buffer report for all variables known
//            to the info_buffer. Uses local tmap to get list of
//            known variables and then uses this set of vars to
//            call the more general getReport() function

vector<string> InfoBuffer::getReport(bool verbose) const
{
  // Since all variables, string or double, create a tmap entry,
  // then use the tmap for an exhaustive list of all vars known.
  vector<string> vars;
  map<string,double>::const_iterator p;
  for(p=tmap.begin(); p!=tmap.end(); p++) 
    vars.push_back(p->first);

  return(getReport(vars, verbose));
}

//-----------------------------------------------------------
// Procedure: getReport()
//   Purpose: Get an info_buffer report for all given variables.
//            This list may include variables for which the
//            info_buffer may not yet know anything.

vector<string> InfoBuffer::getReport(vector<string> vars, bool verbose) const
{
  vector<string> report_lines;

  unsigned int longest_var = 0;
  for(unsigned int i=0; i<vars.size(); i++) {
    if(vars[i].length() > longest_var)
      longest_var = vars[i].length();
  }

  for(unsigned int i=0; i<vars.size(); i++) {
    string line, val;
    string var = vars[i];
    line += padString(var, longest_var, true) + "  ";
    if(dmap.count(var))
      line += doubleToStringX(dmap.at(var),2);
    else if(smap.count(var))
      line += smap.at(var);
    else
      line += "[---]";
    
    report_lines.push_back(line);
  }
  
  return(report_lines);
}

