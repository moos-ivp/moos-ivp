/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_LogPlots.cpp                               */
/*    DATE: June 5th, 2005                                       */
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

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "Populator_LogPlots.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populateFromEntries

bool Populator_LogPlots::populateFromEntries(const vector<ALogEntry>& entries)
{
  unsigned int i, vsize = entries.size();
  
  for(i=0; i<vsize; i++) {
    string var_name  = entries[i].getVarName();
    double itime     = entries[i].getTimeStamp();
    double dvalue = entries[i].getDoubleVal();

    // First determine if a LogPlot already exists for the variable
    map<string, unsigned int>::iterator p;
    p = m_logplot_var_map.find(var_name);
    if(p == m_logplot_var_map.end()) {
      if(entries[i].isNumerical()) {
	LogPlot new_logplot;
	new_logplot.set_logstart_utc(m_logstart_utc);
	new_logplot.set_varname(var_name);
	new_logplot.set_vehi_name(m_vname);
	new_logplot.set_value(itime, dvalue);
	m_logplots.push_back(new_logplot);
	int new_index = m_logplots.size()-1;
	m_logplot_var_map[var_name] = new_index; 
      }
    }
    else {
      unsigned int found_index = p->second;
      m_logplots[found_index].set_value(itime, dvalue);
    }
  }
  
  // Once the vehicle name has been determined, associate the 
  // vehicle name with each log plot.
  vsize = m_logplots.size();
  for(i=0; i<vsize; i++)
    m_logplots[i].set_vehi_name(m_vname);

  cout << "  Total LogPlots (" << m_vname << "): ";
  cout << m_logplots.size() << endl;

  return(true);
}

//---------------------------------------------------------------
// Procedure: getLogPlot(int index)

LogPlot Populator_LogPlots::getLogPlot(unsigned int ix) const
{
  if((ix >= 0) && (ix < m_logplots.size()))
    return(m_logplots[ix]);
  else {
    LogPlot empty_logplot;
    return(empty_logplot);
  }
}

//---------------------------------------------------------------
// Procedure: getLogPlot(string var_name)

LogPlot Populator_LogPlots::getLogPlot(const string& varname) const
{
  int vsize = m_logplots.size();
  for(int i=0; i<vsize; i++) {
    string i_varname = m_logplots[i].get_varname();
    if(i_varname == varname)
      return(m_logplots[i]);
  }

  LogPlot empty_logplot;
  return(empty_logplot);
}

