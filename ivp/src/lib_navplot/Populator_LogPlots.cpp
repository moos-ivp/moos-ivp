/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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

#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "Populator_LogPlots.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: setFileALog
//   Purpose: Just grab the raw data from the alog files and build
//            four vectors - one for each alog column. No further
//            processing.

bool Populator_LogPlots::setFileALog(string filestr)
{
  FILE *f = fopen(filestr.c_str(), "r");
  if(!f)
    return(false);
  fclose(f);

  vector<string> lines = fileBuffer(filestr);
  unsigned int i, line_count = lines.size();
  for(i=0; i<line_count; i++) {
    lines[i] = findReplace(lines[i], '\t', ' ');
    lines[i] = compactConsecutive(lines[i], ' ');
    lines[i] = stripBlankEnds(lines[i]);
    
    string time, variable, source, value;
    
    vector<string> vector_a = chompString(lines[i], ' ');
    time = vector_a[0];
    if(vector_a.size() == 2) {
      vector<string> vector_b = chompString(vector_a[1], ' ');
      variable = vector_b[0];
      if(vector_b.size() == 2) {
	vector<string> vector_c = chompString(vector_b[1], ' ');
	source = vector_c[0];
	if(vector_c.size() == 2)
	  value = vector_c[1];
      }
    }
    
    if((time!="")&&(variable!="")&&(source!="")&&(value!="")) {
      m_alog_entry_time.push_back(time);
      m_alog_entry_var.push_back(variable);
      m_alog_entry_src.push_back(source);
      m_alog_entry_val.push_back(value);
    }
  }
  return(true);
}


//---------------------------------------------------------------
// Procedure: populateFromALog

bool Populator_LogPlots::populateFromALog()
{
  unsigned int i, vsize = m_alog_entry_time.size();
  
  for(i=0; i<vsize; i++) {
    string var_name  = m_alog_entry_var[i];
    string var_value = m_alog_entry_val[i];
    double itime     = atof(m_alog_entry_time[i].c_str());
    // First determine if a LogPlot already exists for the variable
    map<string, unsigned int>::iterator p;
    p = m_logplot_var_map.find(var_name);
    if(p == m_logplot_var_map.end()) {
      if(isNumber(var_value)) { 
	double dvalue = atof(var_value.c_str());
	LogPlot new_logplot;
	new_logplot.set_varname(var_name);
	new_logplot.set_vehi_name(m_vname);
	new_logplot.set_value(itime, dvalue);
	m_logplots.push_back(new_logplot);
	int new_index = m_logplots.size()-1;
	m_logplot_var_map[var_name] = new_index; 
      }
    }
    else {
      double dvalue = atof(var_value.c_str());
      int found_index = p->second;
      m_logplots[found_index].set_value(itime, dvalue);
    }

    if((var_name == "NODE_REPORT_LOCAL") || (var_name == "AIS_REPORT_LOCAL"))
      m_node_reports.push_back(var_value);
  }

  // Handle node reports just to get the alog file vehicle name
  handleNodeReports();

  // Once the vehicle name has been determined, associate the 
  // vehicle name with each log plot.
  vsize = m_logplots.size();
  for(i=0; i<vsize; i++)
    m_logplots[i].set_vehi_name(m_vname);

  cout << "Total LogPlots: " << m_logplots.size() << endl;

  return(true);
}

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

    if((var_name=="NODE_REPORT_LOCAL") || (var_name=="AIS_REPORT_LOCAL")) {
      string svalue = entries[i].getStringVal();
      m_node_reports.push_back(svalue);
    }
  }
  
  // Handle node reports just to get the alog file vehicle name
  handleNodeReports();

  // Once the vehicle name has been determined, associate the 
  // vehicle name with each log plot.
  vsize = m_logplots.size();
  for(i=0; i<vsize; i++)
    m_logplots[i].set_vehi_name(m_vname);

  cout << "Total LogPlots: " << m_logplots.size() << endl;

  return(true);
}

//---------------------------------------------------------------
// Procedure: getLogPlot(int index)

LogPlot Populator_LogPlots::getLogPlot(unsigned int ix)
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

LogPlot Populator_LogPlots::getLogPlot(string varname)
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
// Procedure: handleNodeReports

bool Populator_LogPlots::handleNodeReports()
{
  string vname;

  unsigned int i, vsize = m_node_reports.size();
  for(i=0; i<vsize; i++) {
    vector<string> kvector = parseString(m_node_reports[i], ',');
    unsigned int k, ksize = kvector.size();
    for(k=0; k<ksize; k++) {
      string left  = stripBlankEnds(tolower(biteString(kvector[k], '=')));
      string right = stripBlankEnds(kvector[k]);
      
      if(left == "name") {
	if((vname == "") || (vname == right))
	  vname = right;
	else {
	  cout << "Inconsistent vehi_name from AIS/NODE_REPORT_LOCAL entries" << endl;
	  return(false);
	}
      }
    }
  }

  if(vname != "")
    m_vname = vname;

  return(true);
}

