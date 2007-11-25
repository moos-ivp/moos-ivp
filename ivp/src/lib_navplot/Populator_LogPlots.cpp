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
  skew = 0;
}


//---------------------------------------------------------------
// Procedure: populate

bool Populator_LogPlots::populate()
{
  scan_columns();
  for(int i=1; i<columns.size(); i++)
    populate_logplot(i);

  return(true);
}

//---------------------------------------------------------------
// Procedure: get_logplot(int index)

LogPlot Populator_LogPlots::get_logplot(int ix)
{
  if((ix >= 0) && (ix < logplots.size()))
    return(logplots[ix]);
  else {
    LogPlot empty_logplot;
    return(empty_logplot);
  }
}

//---------------------------------------------------------------
// Procedure: get_logplot(string var_name)

LogPlot Populator_LogPlots::get_logplot(string varname)
{
  int vsize = logplots.size();
  for(int i=0; i<vsize; i++) {
    string i_varname = logplots[i].get_varname();
    if(i_varname == varname)
      return(logplots[i]);
  }

  LogPlot empty_logplot;
  return(empty_logplot);
}

//---------------------------------------------------------------
// Procedure: scan_columns()

void Populator_LogPlots::scan_columns()
{
  int index = 0;
  int lsize = lines.size();

  bool done = false;
  while(!done) {
    string search_tag = "(" + intToString(index+1) + ")";

    bool found = false;
    for(int i=0; ((i<lsize) && !found); i++) {
      vector<string> svector = plines[i];
      if(svector.size() >= 3)
	if(svector[1] == search_tag) {
	  found = true;
	  columns.push_back(svector[2]);
	  index++;
	  if((svector[2] == "TIME") && (svector.size() == 4))
	    skew += atof(svector[3].c_str());
	}
    }
    if(!found)
      done = true;
  }
}


//---------------------------------------------------------------
// Procedure: populate_logplot

void Populator_LogPlots::populate_logplot(int ix)
{
  LogPlot lplot;

  int lsize = lines.size();
  
  for(int i=0; i<lsize; i++) {
    if(lines[i][0] != '%') {
      vector<string> svector = plines[i];
      if(svector.size() >= ix) {
	if(svector[ix] != "NaN") {
	  double tval = atof(svector[0].c_str());
	  double cval = atof(svector[ix].c_str());
	  lplot.set_value(tval+skew, cval);
	}
      }
    }
  }

  lplot.set_varname(columns[ix]);
  lplot.set_vehicle(vname);
  logplots.push_back(lplot);
}

//---------------------------------------------------------------
// Procedure: set_file

bool Populator_LogPlots::set_file(string filestr)
{
  FILE *f = fopen(filestr.c_str(), "r");

  if(!f)
    return(false);

  fclose(f);
  file = filestr;
  lines = fileBuffer(filestr);

  for(int i=0; i<lines.size(); i++) {
    lines[i] = findReplace(lines[i], '\t', ' ');
    lines[i] = compactConsecutive(lines[i], ' ');
    lines[i] = stripBlankEnds(lines[i]);
    vector<string> svector = parseString(lines[i], ' ');
    plines.push_back(svector);
  }

  return(true);
}


//---------------------------------------------------------------
// Procedure: print()

void Populator_LogPlots::print()
{
  for(int i=0; i<columns.size(); i++) {
    cout << "column [" << i << "]: " << columns[i] << endl;
  }
}








