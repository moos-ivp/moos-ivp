/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYConvexGrid.cpp                                     */
/*    DATE: Aug 27th 2005                                        */
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

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "XYConvexGrid.h"
#include "MBUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: initialize
//      Note: A convenience function. Only one cell variable is 
//            implied with an initial value as given. Calles the 
//            more generaly initialize function. 

bool XYConvexGrid::initialize(const XYPolygon& poly,
			      double cell_size, 
			      double init_value)
{
  vector<string> cell_vars;
  cell_vars.push_back("v");
  vector<double> cell_init_vals;
  cell_init_vals.push_back(init_value);

  return(initialize(poly, cell_size, cell_vars, cell_init_vals));
}


//-------------------------------------------------------------
// Procedure: initialize
//   Purpose: The general initialize function. 

bool XYConvexGrid::initialize(const XYPolygon& poly,
			      double cell_size, 
			      vector<string> cell_vars,
			      vector<double> cell_init_vals)
{
  unsigned int i, psize = poly.size();
  if(psize == 0)
    return(false);

  if(cell_vars.size() != cell_init_vals.size())
    return(false);

  // If for some reason there is no cell_vars info, use simple default
  if(cell_vars.size() == 0) {
    m_cell_vars.push_back("v");
    m_cell_init_vals.push_back(0);
  }

  XYSquare unit_square(cell_size);

  // Part One: get bounding box for the polygon and create 
  //           elements based on that square
  double xlow  = poly.get_vx(0);
  double xhigh = poly.get_vx(0);
  double ylow  = poly.get_vy(0);
  double yhigh = poly.get_vy(0);
  
  for(i=1; i<psize; i++) {
    double x = poly.get_vx(i);
    double y = poly.get_vy(i);
    if(x < xlow)    xlow  = x;
    if(x > xhigh)   xhigh = x;
    if(y < ylow)    ylow  = y;
    if(y > yhigh)   yhigh = y;
  }
  
  XYSquare outer_square(xlow, xhigh, ylow, yhigh);

  vector<XYSquare> int_elements;

  bool ok = initialize(outer_square, unit_square);
  if(!ok)
    return(false);

  unsigned int esize = m_elements.size();

  for(i=0; i<esize; i++) {
    xlow  = m_elements[i].getVal(0,0);
    xhigh = m_elements[i].getVal(0,1);
    ylow  = m_elements[i].getVal(1,0);
    yhigh = m_elements[i].getVal(1,1);
    XYPolygon spoly;
    spoly.add_vertex(xlow,  ylow);
    spoly.add_vertex(xhigh, ylow);
    spoly.add_vertex(xhigh, yhigh);
    spoly.add_vertex(xlow,  yhigh);
    if(spoly.intersects(poly))
      int_elements.push_back(m_elements[i]);
  }

  m_elements  = int_elements;

  // Store some config information for serializing the grid (get_spec)
  m_config_poly      = poly;
  m_config_cell_size = cell_size;
  
  for(i=0; i<esize; i++)
    m_cell_vals.push_back(cell_init_vals);

  m_cell_vars      = cell_vars;
  m_cell_init_vals = cell_init_vals;

  // Now that we know how many cell_vars, initialize the vectors
  // containing one entry per cell_var.
  unsigned int cix, csize = m_cell_vars.size();
  for(cix=0; cix<csize; cix++) {
    m_cell_max_limit.push_back(0);
    m_cell_min_limit.push_back(0);
    m_cell_max_limited.push_back(false);
    m_cell_min_limited.push_back(false);

    m_cell_max_sofar.push_back(0);
    m_cell_min_sofar.push_back(0);
    m_cell_minmax_noted.push_back(false);
  }

  return(true);
}


//-------------------------------------------------------------
// Procedure: ptIntersect
//   Purpose: Determine is a given point is contained within the
//            the given grid cell index.

bool XYConvexGrid::ptIntersect(unsigned int ix, double x, double y) const
{
  if(ix >= m_elements.size())
    return(false);
  return(m_elements[ix].containsPoint(x, y));
}

//-------------------------------------------------------------
// Procedure: setIntersect
//   Purpose: Determine the length of the given line segment that
//            intersects with the given grid cell.

double XYConvexGrid::segIntersect(unsigned int ix, 
				  double x1, double y1,
				  double x2, double y2)
{
  if(ix >= m_elements.size())
    return(0);
  return(m_elements[ix].segIntersectLength(x1,y1,x2,y2));
}

//-------------------------------------------------------------
// Procedure: getElement

XYSquare XYConvexGrid::getElement(unsigned int ix) const
{
  XYSquare retElement;

  if(ix < m_elements.size())
    return(m_elements[ix]);
  else
    return(retElement);
}

//-------------------------------------------------------------
// Procedure: hasCellVar()

bool XYConvexGrid::hasCellVar(const string& cell_var) const
{
  unsigned int i, vsize = m_cell_vars.size();
  for(i=0; i<vsize; i++) {
    if(m_cell_vars[i] == cell_var)
      return(true);
  }
  return(false);
}


//-------------------------------------------------------------
// Procedure: getCellVarIX()

unsigned int XYConvexGrid::getCellVarIX(const string& cell_var) const
{
  unsigned int i, vsize = m_cell_vars.size();
  for(i=0; i<vsize; i++) {
    if(m_cell_vars[i] == cell_var)
      return(i);
  }
  return(0);
}


//-------------------------------------------------------------
// Procedure: setVal

void XYConvexGrid::setVal(unsigned int ix, double val, 
			  unsigned int cix)
{
  if((ix >= m_elements.size()) || (cix >= m_cell_vars.size()))
    return;

  // Make sure new value is within bounds, if limited
  if(m_cell_max_limited[cix] && (val > m_cell_max_limit[cix]))
    val = m_cell_max_limit[cix];

  if(m_cell_min_limited[cix] && (val < m_cell_min_limit[cix]))
    val = m_cell_min_limit[cix];
    
  // Set the value of the cell IX for cell var CIX
  m_cell_vals[ix][cix] = val;

  
  // Update the minimum value so far noted for cell_var CIX
  if(!m_cell_minmax_noted[cix] || (val < m_cell_min_sofar[cix]))
    m_cell_min_sofar[cix] = val;
  
  // Update the maximum value so far noted for cell_var CIX
  if(!m_cell_minmax_noted[cix] || (val > m_cell_max_sofar[cix]))
    m_cell_max_sofar[cix] = val;

  // Now that an update has been made for cell_var CIX, note it.
  m_cell_minmax_noted[cix] = true;
}

//-------------------------------------------------------------
// Procedure: incVal

void XYConvexGrid::incVal(unsigned int ix, double val,
			  unsigned int cix)
{
  if((ix >= m_elements.size()) || (cix >= m_cell_vars.size()))
    return;
  
  double curr_val = m_cell_vals[ix][cix];
  setVal(ix, curr_val+val, cix);
}

//-------------------------------------------------------------
// Procedure: getVal

double XYConvexGrid::getVal(unsigned int ix, unsigned int cix) const
{
  if((ix >= m_elements.size()) || (cix >= m_cell_vars.size()))
    return(0);
  return(m_cell_vals[ix][cix]);
}

//-------------------------------------------------------------
// Procedure: getVar

string XYConvexGrid::getVar(unsigned int cix) const
{
  if(cix >= m_cell_vars.size())
    return("");
  return(m_cell_vars[cix]);
}

//-------------------------------------------------------------
// Procedure: getMin

double XYConvexGrid::getMin(unsigned int cix) const
{
  if(cix >= m_cell_vars.size())
    return(0);
  return(m_cell_min_sofar[cix]);
}

//-------------------------------------------------------------
// Procedure: getMax

double XYConvexGrid::getMax(unsigned int cix) const
{
  if(cix >= m_cell_vars.size())
    return(0);
  return(m_cell_max_sofar[cix]);
}

//-------------------------------------------------------------
// Procedure: getInitVal

double XYConvexGrid::getInitVal(unsigned int cix) const
{
  if(cix >= m_cell_vars.size())
    return(0);
  return(m_cell_init_vals[cix]);
}

//-------------------------------------------------------------
// Procedure: getMinLimit

double XYConvexGrid::getMinLimit(unsigned int cix) const
{
  if(cix >= m_cell_vars.size())
    return(0);
  return(m_cell_min_limit[cix]);
}

//-------------------------------------------------------------
// Procedure: cellVarMinLimited()

bool XYConvexGrid::cellVarMinLimited(unsigned int cix) const
{
  if(cix >= m_cell_vars.size())
    return(false);
  return(m_cell_min_limited[cix]);
}

//-------------------------------------------------------------
// Procedure: cellVarMaxLimited()

bool XYConvexGrid::cellVarMaxLimited(unsigned int cix) const
{
  if(cix >= m_cell_vars.size())
    return(false);
  return(m_cell_max_limited[cix]);
}

//-------------------------------------------------------------
// Procedure: getMaxLimit

double XYConvexGrid::getMaxLimit(unsigned int cix) const
{
  if(cix >= m_cell_vars.size())
    return(0);

  return(m_cell_max_limit[cix]);
}


//-------------------------------------------------------------
// Procedure: setMinLimit

void XYConvexGrid::setMinLimit(double min_limit, unsigned int cix) 
{
  if(cix >= m_cell_vars.size())
    return;
  
  m_cell_min_limit[cix]   = min_limit;
  m_cell_min_limited[cix] = true;
  
  // If there is a cell_max_limit in force
  if(m_cell_max_limited[cix])
    // And the new min_limit is actually greater(!) than max_limit
    if(min_limit > m_cell_max_limit[cix])
      // Then clip the min_limit to be no more than the max_limit
      m_cell_min_limit[cix] = m_cell_max_limit[cix];
}

//-------------------------------------------------------------
// Procedure: setMaxLimit

void XYConvexGrid::setMaxLimit(double max_limit, unsigned int cix) 
{
  if(cix >= m_cell_vars.size())
    return;
  
  m_cell_max_limit[cix]   = max_limit;
  m_cell_max_limited[cix] = true;
  
  // If there is a cell min_limit in force
  if(m_cell_min_limited[cix])
    // And the new max_limit is actually greater(!) than min_limit
    if(max_limit < m_cell_min_limit[cix])
      // Then clip the max_limit to be no more than the min_limit
      m_cell_max_limit[cix] = m_cell_min_limit[cix];  
}


//-------------------------------------------------------------
// Procedure: ptIntersect
//   Purpose: Determine is a given point is contained within the
//            the grid, i.e., within any one element

bool XYConvexGrid::ptIntersect(double x, double y) const
{
  unsigned int i, vsize = m_elements.size();
  for(i=0; i<vsize; i++) 
    if(m_elements[i].containsPoint(x, y))
      return(true);
  return(false);
}

//-------------------------------------------------------------
// Procedure: ptIntersectBound
//   Purpose: Determine is a given point is contained within the
//            bounding box of all grid squares.

bool XYConvexGrid::ptIntersectBound(double x, double y) const
{
  return(m_bounding_square.containsPoint(x, y));
}

//-------------------------------------------------------------
// Procedure: segIntersectBound
//   Purpose: Determine is a given line segment intersects the
//            bounding box of all grid squares.

bool XYConvexGrid::segIntersectBound(double x1, double y1,
			       double x2, double y2) const
{
  return(m_bounding_square.segIntersectLength(x1,y1,x2,y2) > 0);
}

//-------------------------------------------------------------
// Procedure: initialize

bool XYConvexGrid::initialize(const XYSquare& outer_square,
			      const XYSquare& unit_square)
{
  double outer_x_len = outer_square.getLengthX();
  double outer_y_len = outer_square.getLengthY();
  double unit_x_len  = unit_square.getLengthX();
  double unit_y_len  = unit_square.getLengthY();

  if(outer_y_len < unit_y_len)
    return(false);
  if(outer_x_len < unit_x_len)
    return(false);
  
  double x_whole = floor(outer_x_len / unit_x_len);
  double x_extra = (outer_x_len - (x_whole * unit_x_len));
  int    x_count = (int)(x_whole);
  if(x_extra > 0)
    x_count++;

  double y_whole = floor(outer_y_len / unit_y_len);
  double y_extra = (outer_y_len - (y_whole * unit_y_len));
  int    y_count = (int)(y_whole);
  if(y_extra > 0)
    y_count++;

  XYSquare new_square;
  for(int i=0; i<x_count; i++) {
    for(int j=0; j<y_count; j++) {
      double x_low  = (i * unit_x_len) + outer_square.getVal(0,0);
      double x_high = x_low + unit_x_len;
      double y_low  = (j * unit_y_len) + outer_square.getVal(1,0);
      double y_high = y_low + unit_y_len;
      new_square.set(x_low, x_high, y_low, y_high);
      m_elements.push_back(new_square);
    }
  }

  m_bounding_square = outer_square;
  return(true);
}


//-------------------------------------------------------------
// Procedure: reset
//   Purpose: Reset the grid (without changing the grid structure).
//            Reset all the initial values back, and reset the 
//            min_sofar and max_sofar values for each cellvar.

void XYConvexGrid::reset()
{
  // For each element (grid cell) and each cell_var, reset to the
  // cell_var's initial value.
  unsigned int ix,  esize = m_elements.size();
  unsigned int cix, csize = m_cell_vars.size();
  for(ix=0; ix<esize; ix++) {
    for(cix=0; cix<csize; cix++) {
      double init_val = m_cell_init_vals[cix];
      m_cell_vals[ix][cix] = init_val;
    }
  }

  // For each cell_var indicate no values yet noted.
  for(cix=0; ix<csize; ix++) 
    m_cell_minmax_noted[cix] = false;
}


//-------------------------------------------------------------
// Procedure: reset (for a given cell_var)
//   Purpose: Reset the grid (without changing the grid structure).
//            Reset all the initial values back, and reset the 
//            min_sofar and max_sofar values for each cellvar.

void XYConvexGrid::reset(const string& cell_var)
{
  if(!hasCellVar(cell_var))
    return;
  unsigned int cix = getCellVarIX(cell_var);

  // For each element (grid cell), for the given cell_var, reset 
  // to the cell_var's initial value.
  unsigned int ix,  esize = m_elements.size();
  for(ix=0; ix<esize; ix++) {
    double init_val = m_cell_init_vals[cix];
    m_cell_vals[ix][cix] = init_val;
  }

  // For given cell_var indicate that no values yet noted
  m_cell_minmax_noted[cix] = false;
}


//-------------------------------------------------------------
// Procedure: get_spec

string XYConvexGrid::get_spec() const
{
  string spec = getConfigStr();

  // Now build all the cell information. If a cell has no changes
  // from the defaults, for any of its cell vars, there will be no
  // info for that cell included. Format is generally like:
  //   cell=ix:var:val:var:val:var:val, or
  //   cell=23:force_angle:180:magnitude:2.3

  unsigned int ix, esize = m_elements.size();
  for(ix=0; ix<esize; ix++) {
    string cell_spec;
    unsigned int cix, csize = m_cell_vars.size();
    for(cix=0; cix<csize; cix++) {
      if(m_cell_vals[ix][cix] != m_cell_init_vals[cix]) {
	double dval = m_cell_vals[ix][cix];
	cell_spec += m_cell_vars[cix] + ":" + doubleToStringX(dval);	
      }
    }
    if(cell_spec != "") {
      cell_spec = ",cell=" + uintToString(ix) + ":" + cell_spec;
      spec += cell_spec;
    }
  }

  string obj_spec = XYObject::get_spec();
  if(obj_spec != "")
    spec += ("," + obj_spec);

  return(spec);
}



//-------------------------------------------------------------
// Procedure: getConfigStr
//   Purpose: Return a serialized version of just the grid config
//            components; likely its original configuration string.
//            "pts={0,0:100,0:100,100:0,100},cell_size=10,
//             cell_vars=x:0:y:0:z:0,cell_min=x:0,cell_max=x:100"

string XYConvexGrid::getConfigStr() const
{
  string spec = m_config_poly.get_spec();

  spec += ",cell_size=" + doubleToStringX(m_config_cell_size);

  string cell_vars_spec;
  unsigned int j, jsize = m_cell_vars.size();
  for(j=0; j<jsize; j++) {
    if(j != 0) 
      cell_vars_spec += ":";
    cell_vars_spec += m_cell_vars[j] + ":";
    cell_vars_spec += doubleToStringX(m_cell_init_vals[j]);
  }
  if(cell_vars_spec != "")
    spec += ",cell_vars=" + cell_vars_spec;

  unsigned int cix, cvsize = m_cell_vars.size();
  for(cix=0; cix<cvsize; cix++) {
    if(cellVarMinLimited(cix)) {
      spec += ",cell_min=";
      spec += m_cell_vars[cix] + ":";      
      double min_limit = getMinLimit(cix);
      spec += doubleToStringX(min_limit);
    }
    if(cellVarMaxLimited(cix)) {
      spec += ",cell_max=";
      spec += m_cell_vars[cix] + ":";      
      double max_limit = getMaxLimit(cix);
      spec += doubleToStringX(max_limit);
    }
  }

  return(spec);
}

//-------------------------------------------------------------
// Procedure: print
//   Purpose: For debugging.

void XYConvexGrid::print() const
{
  unsigned int i, vsize = m_elements.size();
  for(i=0; i<vsize; i++) 
    cout << "[" << i << "]: " << m_elements[i].get_spec() << endl;
}
