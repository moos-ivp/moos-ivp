/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_NavPlot.cpp                                */
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
#include "Populator_NavPlot.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

Populator_NavPlot::Populator_NavPlot()
{
  skew    = 0;
}


//---------------------------------------------------------------
// Procedure: populate

bool Populator_NavPlot::populate()
{
  string x_label   = "NAV_X";
  string y_label   = "NAV_Y";
  string hdg_label = "NAV_HEADING";

  scan_columns();

  int ix1 = -1;
  int ix2 = -1;
  int ix3 = -1;

  for(int i=1; i<columns.size(); i++) {
    if(columns[i] == x_label)
      ix1 = i;
    if(columns[i] == y_label)
      ix2 = i;
    if(columns[i] == hdg_label)
      ix3 = i;
  } 
  if((ix1 != ix2) && (ix1 != -1) && (ix2 != -1) && (ix3 != -1))
    populate_navplot(ix1, ix2, ix3);
  else if((ix1 != ix2) && (ix1 != -1) && (ix2 != -1))
    populate_navplot(ix1, ix2);

  return(true);
}

//---------------------------------------------------------------
// Procedure: scan_columns()

void Populator_NavPlot::scan_columns()
{
  int index = 1;
  int lsize = lines.size();

  bool done = false;
  while(!done) {
    string search_tag = "(" + intToString(index) + ")";

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
// Procedure: populate_navplot

void Populator_NavPlot::populate_navplot(int ix1, int ix2, int ix3)
{
  NavPlot new_navplot;
  int lsize = lines.size();

  for(int i=0; i<lsize; i++) {
    if(lines[i][0] != '%') {
      vector<string> svector = plines[i];
      if((svector.size() >= ix1) && (svector.size() >= ix2)) {
	if((svector[ix1] != "NaN") && (svector[ix2] != "NaN")) {
	  double t_val = atof(svector[0].c_str());
	  double x_val = atof(svector[ix1].c_str());
	  double y_val = atof(svector[ix2].c_str());
	  double hdg_val = 999;
	  if(ix3 != -1)
	    hdg_val = atof(svector[ix3].c_str());
	  new_navplot.set_value(t_val+skew, x_val, y_val, hdg_val);
	}
      }
    }
  }
  navplot = new_navplot;
}

//---------------------------------------------------------------
// Procedure: set_file

bool Populator_NavPlot::set_file(string filestr)
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









