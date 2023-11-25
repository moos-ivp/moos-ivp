/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogPlot.cpp                                          */
/*    DATE: May 31st, 2005                                       */
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

#include <list>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "LogPlot.h"
#include "LogUtils.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

LogPlot::LogPlot()
{
  m_min_val  = 0; 
  m_max_val  = 0; 
  m_median   = 0;
  m_median_set = false;

  m_avg_time_gap = -1;
}

//---------------------------------------------------------------
// Procedure: getAvgTimeGap()
//   Purpose: Determine average gap in time between succesive entries.

double LogPlot::getAvgTimeGap()
{
  // If this has been calculated before, just returned the cached value
  if(m_avg_time_gap != -1)
    return(m_avg_time_gap);
  
  // If there isn't at least one gap, (need 2 elements) just return -1.
  if(m_time.size() <= 1)
    return(-1);
  
  m_avg_time_gap = ((m_time[size()-1] - m_time[0]) / ((double)(m_time.size())));
  return(m_avg_time_gap);
}

//---------------------------------------------------------------
// Procedure: setValue
//      Note: Time must be in ascending order. If new pair doesn't
//            obey, no action is taken, and false is returned.

bool LogPlot::setValue(double gtime, double gval)
{
  unsigned int tsize = m_time.size();

  if((tsize == 0) || (m_time[tsize-1] <= gtime)) {
    m_time.push_back(gtime);
    m_value.push_back(gval);
    if((tsize == 0) || (gval > m_max_val))
      m_max_val = gval;
    if((tsize == 0) || (gval < m_min_val))
      m_min_val = gval;
    m_median_set = false;
    return(true);
  }
  else {
    return(false);
  }
}

//---------------------------------------------------------------
// Procedure: setValueByIndex

bool LogPlot::setValueByIndex(unsigned int ix, double gval)
{
  if((ix >= m_time.size()) || (ix >= m_value.size()))
    return(false);

  m_value[ix] = gval;
  return(true);
}

//---------------------------------------------------------------
// Procedure: applySkew

void LogPlot::applySkew(double skew)
{
  for(unsigned int i=0; i<m_time.size(); i++)
    m_time[i] += skew;
}
     
//---------------------------------------------------------------
// Procedure: modValues

void LogPlot::modValues(double modval)
{
  for(unsigned int i=0; i<m_value.size(); i++)
    m_value[i] += modval;
}
     
//---------------------------------------------------------------
// Procedure: filterOutHighest()

void LogPlot::filterOutHighest(double pct)
{
  if((pct < 0) || (pct > 1.0))
    return;

  // Part 1: sort the values from lowest to highest.
  vector<double> m_value_copy = m_value;
  std::sort(m_value_copy.begin(), m_value_copy.end());
  
  // Part 2: find out the threshold, above which we will discard.
  unsigned int thresh_ix = (unsigned int)((1-pct) * m_value.size());
  double thresh = m_value_copy[thresh_ix];

  // Part 3: Create the new vectors
  vector<double> old_time = m_time;
  vector<double> old_value = m_value;
  m_time.clear();
  m_value.clear();

  m_min_val  = 0; 
  m_max_val  = 0; 
  m_median   = 0;
  m_median_set = false;
  m_avg_time_gap = -1;

  for(unsigned int i=0; i<old_time.size(); i++) {
    if(m_value[i] < thresh)
      setValue(m_time[i], m_value[i]);
  }
}
     
//---------------------------------------------------------------
// Procedure: getValueByIndex

double LogPlot::getValueByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_value[index]);
  return(0);
}
     
//---------------------------------------------------------------
// Procedure: getTimeByIndex

double LogPlot::getTimeByIndex(unsigned int index) const
{
  if(index < m_time.size())
    return(m_time[index]);
  return(0);
}
     
//---------------------------------------------------------------
// Procedure: getLPIndexByTime()

unsigned int LogPlot::getLPIndexByTime(double gtime) const
{
  // Handle edge cases. User beware that a returned value of zero
  // may be an indication of one of these edge cases.
  if(m_time.size() == 0)
    return(0);
  if(gtime < m_time[0])
    return(0);
  if(gtime > m_time[m_time.size()-1])
    return(m_time.size()-1);
  
  return(getIndexByTime(m_time, gtime));
}
     
//---------------------------------------------------------------
// Procedure: getValueByTime
//      Note: If the argument, interp, is true, the function will
//            return an interpolated value if the given value falls
//            between two data points. The default is interp=false.

double LogPlot::getValueByTime(double gtime, bool interp) const
{
  unsigned int vsize = m_time.size();

  if(vsize == 0)
    return(0);

  if(gtime >= m_time[vsize-1])
    return(m_value[vsize-1]);

  if(gtime <= m_time[0])
    return(m_value[0]);

  unsigned int index = getIndexByTime(m_time, gtime);
  if((gtime == m_time[index]) || !interp)
    return(m_value[index]);
  
  double val1 = m_value[index];
  double val2 = m_value[index+1];
  
  double val_range  = val2 - val1;
  double time_range = m_time[index+1] - m_time[index];

  if(time_range <= 0)
    return(m_value[index]);

  double pct_time = (gtime - m_time[index]) / time_range;

  double rval = (pct_time * val_range) + val1;

  return(rval);
}
     
//---------------------------------------------------------------
// Procedure: getMedian
//   Purpose: Calculate the median value of all points added so far.

double LogPlot::getMedian() 
{
  // The median value is stored locally - only calculated it if it
  // has not been calculated already. Subsequent additions to the 
  // log will clear the median value;
  if(m_median_set)
    return(m_median);

  list<double> vlist;

  // First put all the log values in the list.
  unsigned int k, ksize = m_value.size();
  for(k=0; k<ksize; k++)
    vlist.push_back(m_value[k]);

  // Then sort them - ascending or descending does not matter.
  vlist.sort();

  // Increment an interator half way through the list and get
  // the value at that point as the median.
  unsigned int lsize = vlist.size();
  list<double>::iterator p = vlist.begin();
  for(k=0; k<(lsize/2); k++)
    p++;

  m_median     = *p;
  m_median_set = true;

  return(m_median);
}
     
//---------------------------------------------------------------
// Procedure: getMinTime

double LogPlot::getMinTime() const
{
  if(m_time.size() > 0)
    return(m_time[0]);
  return(0);
}

//---------------------------------------------------------------
// Procedure: getMaxTime

double LogPlot::getMaxTime() const
{
  if(m_time.size() > 0)
    return(m_time[m_time.size()-1]);
  return(0);
}

//---------------------------------------------------------------
// Procedure: getMinVal(double mintime, double maxtime)

double LogPlot::getMinVal(double mintime, double maxtime) const
{
  //cout << "LogPlot::getMinVal() Begin=====================================" << endl;
  //cout << " mintime:" << mintime << ", maxtime:" << maxtime << endl;
  bool virgin = true;
  double minval = 0;
  for(unsigned int i=0; i<m_time.size(); i++) {
    //cout << "m_time[" << i << "]: m_value:" << m_value[i] << endl;
    if((m_time[i] >= mintime) && (m_time[i] <= maxtime)) {
      //cout << "  candidate (satisfies time)" << endl;
      if(virgin || (m_value[i] < minval)) {
	//cout << "  UPDATE: new minval:" << minval << endl;
	minval = m_value[i];
	virgin = false;
      }
    }
  }
  //cout << "LogPlot::getMinVal() End =====================================" << endl;
  return(minval);
}

//---------------------------------------------------------------
// Procedure: getMaxVal(double mintime, double maxtime)

double LogPlot::getMaxVal(double mintime, double maxtime) const
{
  bool virgin = true;
  double maxval = 0;
  for(unsigned int i=0; i<m_time.size(); i++) {
    if((m_time[i] >= mintime) && (m_time[i] <= maxtime)) {
      if(virgin || (m_value[i] > maxval)) {
	maxval = m_value[i];
	virgin = false;
      }
    }
  }
  return(maxval);
}

//---------------------------------------------------------------
// Procedure: print

void LogPlot::print() const
{
  unsigned int i;
  cout << "LogPlot::print()" << endl;
  cout << " Variable Name: " << m_varname << endl;
  for(i=0; i<m_time.size(); i++) {
    cout << "time:" << m_time[i] << "  val:" << m_value[i] << endl;
  }
}

//---------------------------------------------------------------
// Procedure: getSpec()
//   Example: "vname=henry,varname=NAV_X,elememts=time:value#time:value"

string LogPlot::getSpec(unsigned int tprec, unsigned int vprec) const
{
  stringstream ss;

  // Since vname is not mandatory, check if set before serializing
  if(m_vname != "")
    ss << "vname=" << m_vname << ",";
  
  ss << "varname=" << m_varname << ",elements=";
  for(unsigned int i=0; i<m_time.size(); i++) {
    if(i > 0)
      ss << "#";
    ss << doubleToString(m_time[i],tprec) << ":";
    ss << doubleToString(m_value[i],vprec);
  }
  return(ss.str());
}


//---------------------------------------------------------------
// Procedure: setFromSpec
//   Example: "vname=henry,varname=NAV_X,elememts=time:value#time:value"

bool LogPlot::setFromSpec(string spec) 
{
  // Clear the current state
  m_vname = "";
  m_varname = "";
  m_time.clear();
  m_value.clear();
  m_min_val = 0;
  m_max_val = 0;
  m_median  = 0;
  m_median_set = false;
  
  // Set the new state
  vector<string> svector = parseString(spec, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if(param == "vname")
      m_vname = value;
    else if(param == "varname")
      m_varname = value;
    else if(param == "elements") {
      vector<string> kvector = parseString(value, '#');
      for(unsigned int k=0; k<kvector.size(); k++) {
	string time  = biteString(kvector[k], ':');
	string sval  = kvector[k];
	if(!isNumber(time) || !isNumber(sval)) {
	  cout << "FAILED NUMBER ----------------------------------" << endl;
	  return(false);
	}
	double dtime = atof(time.c_str());
	double dval  = atof(sval.c_str());
	bool ok = setValue(dtime, dval);
	if(!ok) {
	  cout << "FAILED SETVALUE ----------------------------------" << endl;
	  return(false);
	}
      }
    }
  }
  if(m_varname == "") {
    cout << "NULL VARNAME----------------------------------" << endl;
    return(false);
  }
  
  return(true);	
}





