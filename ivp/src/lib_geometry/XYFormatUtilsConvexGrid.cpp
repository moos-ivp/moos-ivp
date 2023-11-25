/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYFormatUtilsConvexGrid.cpp                          */
/*    DATE: Dec 7th, 2011 Alon visits Reading                    */
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
   #define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream>
#include "XYFormatUtilsConvexGrid.h"
#include "XYPolygon.h"
#include "XYSquare.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2ConvexGrid 

XYConvexGrid string2ConvexGrid(string str)
{
  //cout << "string2ConvexGrid:" << str << endl;

  XYConvexGrid null_grid;
  XYConvexGrid new_grid;
  XYPolygon    new_poly;
  double       cell_size = 10;
  double       init_val  = 0;

  vector<string> cell_entries;
  vector<string> cell_vars;
  vector<double> cell_init_vals;
  vector<string> cell_mins;
  vector<string> cell_maxs;

  //==========================================================
  // PHASE 1: Initial parsing
  //==========================================================
  string rest = str;
  while(rest != "") {
    string left = biteStringX(rest, '=');
    
    if(left == "pts") {
      null_grid.set_msg("Invalid pts specification");
      string pstr = biteStringX(rest, '}');
      
      // Empty set of points is an error
      if(pstr == "")
	return(null_grid);

      // Points should begin with an open brace (but discard now)
      if(pstr[0] != '{') 
	return(null_grid);
      else
	pstr = pstr.substr(1);

      // If more components after pts={}, then it should begin w/ comma
      if(rest != "") {
	if(rest[0] != ',')
	  return(null_grid);
	else
	  rest = rest.substr(1);
      }

      vector<string> svector = parseString(pstr, ':');
      unsigned int i, vsize = svector.size();
      for(i=0; i<vsize; i++) {
	string vertex = stripBlankEnds(svector[i]);
	string xstr = biteStringX(vertex, ',');
	string ystr = biteStringX(vertex, ',');
	if(!isNumber(xstr) || !isNumber(ystr))
	  return(null_grid);
	double xval = atof(xstr.c_str());
	double yval = atof(ystr.c_str());
	new_poly.add_vertex(xval, yval);
      }
      new_poly.determine_convexity();
      if(!new_poly.is_convex())
	return(null_grid);
      null_grid.set_msg("");

    }
    else if(left == "cell_size") {
      null_grid.set_msg("Invalid cell_size specification");
      string right = biteStringX(rest, ',');
      if(!isNumber(right))
	return(null_grid);
      cell_size = atof(right.c_str());
      if(cell_size <= 0)
	return(null_grid);
      null_grid.set_msg("");
    }
    else if(left == "init_val") {
      null_grid.set_msg("Invalid init_val specification");
      string right = biteStringX(rest, ',');
      if(!isNumber(right))
	return(null_grid);
      init_val = atof(right.c_str());
      null_grid.set_msg("");
    }
    // cell_var=x:0:y:0:z:0
    else if(left == "cell_vars") {
      null_grid.set_msg("Invalid cell_vars specification");
      string rrest = biteStringX(rest, ',');
      while(rrest != "") {
	string cell_var      = biteString(rrest, ':');
	string cell_init_val = biteString(rrest, ':');
	if((cell_var != "") && isNumber(cell_init_val)) {
	  cell_vars.push_back(cell_var);
	  cell_init_vals.push_back(atof(cell_init_val.c_str()));
	}
	else
	  return(null_grid);
      }
      null_grid.set_msg("");
    }
    else if(left == "cell") {
      string right = biteStringX(rest, ',');
      cell_entries.push_back(right);
    }
    // cell_min=x:0
    else if(left == "cell_min") {
      string right = biteStringX(rest, ',');
      cell_mins.push_back(right);
    }
    // cell_max=x:0
    else if(left == "cell_max") {
      string right = biteStringX(rest, ',');
      cell_maxs.push_back(right);
    }
    else {
      string right = biteStringX(rest, ',');
      new_grid.set_param(left, right);
    }
  }

  //==========================================================
  // PHASE 2: Building the Grid
  //==========================================================

  // Phase 2A Initialize the grid elements
  // pts={0,0:100,0:100,100:0,100}, cell_size=10, cell_vars=x:0:y:0:z:0
  if(cell_vars.size() > 0) {
    new_grid.initialize(new_poly, cell_size, cell_vars, cell_init_vals);
  }
  else
    new_grid.initialize(new_poly, cell_size, init_val);
    
  unsigned int j, jsize;

  // Phase 2B: Set the cell_var min bounds
  // cell_min=x:0:y:0:z:0
  jsize = cell_mins.size();
  for(j=0; j<jsize; j++) {
    string jrest = cell_mins[j];
    while(jrest != "") {
      string var = biteStringX(jrest, ':');
      string min = biteStringX(jrest, ':');
      if(new_grid.hasCellVar(var) && isNumber(min)) {
	unsigned int cix = new_grid.getCellVarIX(var);
	double min_val = atof(min.c_str());
	new_grid.setMinLimit(min_val, cix);
      }
    }
  }

  // Phase 2C: Set the cell_var max bounds
  // cell_max=x:0:y:0:z:0
  jsize = cell_maxs.size();
  for(j=0; j<jsize; j++) {
    string jrest = cell_maxs[j];
    while(jrest != "") {
      string var = biteStringX(jrest, ':');
      string max = biteStringX(jrest, ':');
      if(new_grid.hasCellVar(var) && isNumber(max)) {
	unsigned int cix = new_grid.getCellVarIX(var);
	double max_val = atof(max.c_str());
	new_grid.setMaxLimit(max_val, cix);
      }
    }
  }


#if 1 // Bit harder to read, but perhaps a bit faster

  // Phase 2D: Apply the cell entries (usually not in initial config).
  // cell=index:cell_var:value:cell_var:value
  jsize = cell_entries.size();
  for(j=0; j<jsize; j++) {
    string index = biteString(cell_entries[j], ':');
    unsigned int ix = atoi(index.c_str());

    while(cell_entries[j] != "") {
      string cell_var = biteString(cell_entries[j], ':');
      string value = biteString(cell_entries[j], ':');
      double dval = atof(value.c_str());
      if(new_grid.hasCellVar(cell_var)) {
	unsigned int cix = new_grid.getCellVarIX(cell_var);
	new_grid.setVal(ix, dval, cix);
      }
    }
  }
#endif

#if 0
  // Phase 2D: Apply the cell entries (usually not in initial config).
  // cell=index:cell_var:value:cell_var:value
  jsize = cell_entries.size();
  for(j=0; j<jsize; j++) {
    string entry = cell_entries[j];
    string index = biteString(entry, ':');
    string rest  = entry;
    unsigned int ix = atoi(index.c_str());

    while(rest != "") {
      string cell_var = biteString(rest, ':');
      string value = biteString(rest, ':');
      double dval = atof(value.c_str());
      if(new_grid.hasCellVar(cell_var)) {
	unsigned int cix = new_grid.getCellVarIX(cell_var);
	new_grid.setVal(ix, dval, cix);
      }
    }
  }
#endif

  return(new_grid);
}










