/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_LogPlots.cpp                               */
/*    DATE: June 5th, 2005                                       */
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
    double dvalue    = entries[i].getDoubleVal();
    bool   isnum     = entries[i].isNumerical();

    if(isnum)
      augmentLogPlot(var_name, itime, dvalue);
    else {
      if(m_map_var_field_extras.count(var_name) > 0) {
	vector<string> fields = m_map_var_field_extras[var_name];
	for(unsigned int j=0; j<fields.size(); j++) {
	  bool found = entries[i].tokenField(fields[j], dvalue);
	  if(found) {
	    string full_var_name = var_name + "_" + toupper(fields[j]);
	    augmentLogPlot(full_var_name, itime, dvalue);
	  }
	}
      }
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
// Procedure: augmentLogPlot

void Populator_LogPlots::augmentLogPlot(const string& var_name, 
					double itime, double dvalue)
{
  // First determine if a LogPlot already exists for the variable
  map<string, unsigned int>::iterator p = m_logplot_var_map.find(var_name);
  if(p == m_logplot_var_map.end()) {
    LogPlot new_logplot;
    new_logplot.set_logstart_utc(m_logstart_utc);
    new_logplot.set_varname(var_name);
    new_logplot.set_vehi_name(m_vname);
    new_logplot.set_value(itime, dvalue);
    m_logplots.push_back(new_logplot);
    int new_index = m_logplots.size()-1;
    m_logplot_var_map[var_name] = new_index; 
  }
  else {
    unsigned int found_index = p->second;
    m_logplots[found_index].set_value(itime, dvalue);
  }
}

//---------------------------------------------------------------
// Procedure: getLogPlot(int index)

LogPlot Populator_LogPlots::getLogPlot(unsigned int ix) const
{
  if(ix < m_logplots.size())
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

//---------------------------------------------------------------
// Procedure: addVarFieldExtra

void Populator_LogPlots::addVarFieldExtra(string var, string field)
{
  if(m_map_var_field_extras.count(var) == 0) {
    vector<string> fields;
    m_map_var_field_extras[var] = fields;
  }

  m_map_var_field_extras[var].push_back(field);
}





