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
#include <stdlib.h>
#include <iostream>
#include "Populator_LogPlots.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

Populator_LogPlots::Populator_LogPlots()
{
  m_skew = 0;
}


//---------------------------------------------------------------
// Procedure: populate

bool Populator_LogPlots::populateFromSLog()
{
  scanColumns();
  for(int i=1; i<m_columns.size(); i++)
    populateLogPlot(i);

  return(true);
}

//---------------------------------------------------------------
// Procedure: populate

bool Populator_LogPlots::populateFromALog()
{
  int i, vsize = m_alog_entry_time.size();
  
  map<string, int>::iterator p;

  // First determine if a LogPlot already exists for the variable
  // in question.
  
  for(i=0; i<vsize; i++) {
    string var_name  = m_alog_entry_var[i];
    string var_value = m_alog_entry_val[i];
    double itime     = atof(m_alog_entry_time[i].c_str());
    p = m_logplot_var_map.find(var_name);
    if(p == m_logplot_var_map.end()) {
      if(isNumber(var_value)) { 
	double dvalue = atof(var_value.c_str());
	LogPlot new_logplot;
	new_logplot.set_varname(var_name);
	new_logplot.set_vehicle(m_vname);
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
  }

  cout << "Total LogPlots: " << m_logplots.size() << endl;
  m_logplots[0].print();



  return(true);
}

//---------------------------------------------------------------
// Procedure: getLogPlot(int index)

LogPlot Populator_LogPlots::getLogPlot(int ix)
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
// Procedure: scanColumns()
//      Note: The end result is filling in the values for m_columns
//            which should list the variable associated with each
//            column in the slog file.

void Populator_LogPlots::scanColumns()
{
  int index = 0;
  int lsize = m_lines.size();

  bool done = false;
  while(!done) {
    string search_tag = "(" + intToString(index+1) + ")";

    bool found = false;
    for(int i=0; ((i<lsize) && !found); i++) {
      vector<string> svector = m_plines[i];
      if(svector.size() >= 3)
	if(svector[1] == search_tag) {
	  found = true;
	  m_columns.push_back(svector[2]);
	  index++;
	  if((svector[2] == "TIME") && (svector.size() == 4))
	    m_skew += atof(svector[3].c_str());
	}
    }
    if(!found)
      done = true;
  }
}


//---------------------------------------------------------------
// Procedure: populateLogPlot

void Populator_LogPlots::populateLogPlot(int ix)
{
  LogPlot lplot;

  int lsize = m_lines.size();
  
  for(int i=0; i<lsize; i++) {
    if(m_lines[i][0] != '%') {
      vector<string> svector = m_plines[i];
      if(svector.size() >= ix) {
	if(svector[ix] != "NaN") {
	  double tval = atof(svector[0].c_str());
	  double cval = atof(svector[ix].c_str());
	  lplot.set_value(tval + m_skew, cval);
	}
      }
    }
  }

  lplot.set_varname(m_columns[ix]);
  lplot.set_vehicle(m_vname);
  m_logplots.push_back(lplot);
}

//---------------------------------------------------------------
// Procedure: setFileSLog

bool Populator_LogPlots::setFileSLog(string filestr)
{
  FILE *f = fopen(filestr.c_str(), "r");

  if(!f)
    return(false);

  fclose(f);
  m_file = filestr;
  m_lines = fileBuffer(filestr);

  for(int i=0; i<m_lines.size(); i++) {
    m_lines[i] = findReplace(m_lines[i], '\t', ' ');
    m_lines[i] = compactConsecutive(m_lines[i], ' ');
    m_lines[i] = stripBlankEnds(m_lines[i]);
    vector<string> svector = parseString(m_lines[i], ' ');
    m_plines.push_back(svector);
  }

  return(true);
}


//---------------------------------------------------------------
// Procedure: setFileALog

bool Populator_LogPlots::setFileALog(string filestr)
{
  FILE *f = fopen(filestr.c_str(), "r");

  if(!f)
    return(false);

  fclose(f);
  m_file = filestr;
  m_lines = fileBuffer(filestr);

  for(int i=0; i<m_lines.size(); i++) {
    m_lines[i] = findReplace(m_lines[i], '\t', ' ');
    m_lines[i] = compactConsecutive(m_lines[i], ' ');
    m_lines[i] = stripBlankEnds(m_lines[i]);
    
    string time, variable, source, value;
    
    vector<string> vector_a = chompString(m_lines[i], ' ');
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
// Procedure: print()

void Populator_LogPlots::print()
{
  for(int i=0; i<m_columns.size(); i++) {
    cout << "column [" << i << "]: " << m_columns[i] << endl;
  }
}








