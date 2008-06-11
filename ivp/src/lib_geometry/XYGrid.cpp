/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYGrid.cpp                                           */
/*    DATE: Aug 27th 2005                                        */
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

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "XYGrid.h"
#include "MBUtils.h"
#include "XYBuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

XYGrid::XYGrid()
{
  min_val           = 0;
  max_val           = 0;
  min_util          = 0;
  max_util          = 0;
  min_util_possible = 0;
  max_util_possible = 1;
}

//-------------------------------------------------------------
// Procedure: getElement

XYSquare XYGrid::getElement(unsigned int ix) const
{
  XYSquare retElement;

  if((ix >= 0) && (ix < elements.size()))
    return(elements[ix]);
  else
    return(retElement);
}

//-------------------------------------------------------------
// Procedure: setVal
//      Note: reflects min/max val ever seen - as currently impl.

void XYGrid::setVal(unsigned int ix, double val)
{
  if((ix >= 0) && (ix < elements.size()))
    values[ix] = val;

  if(val < min_val)
    min_val = val;
  if(val > max_val)
    max_val = val;
}

//-------------------------------------------------------------
// Procedure: getVal

double XYGrid::getVal(unsigned int ix) const
{
  if((ix >= 0) && (ix < values.size()))
    return(values[ix]);
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: setUtil

void XYGrid::setUtil(unsigned int ix, double val)
{
  if(val > max_util_possible)
    val = max_util_possible;
  if(val < min_util_possible)
    val = min_util_possible;
  
  if((ix >= 0) && (ix < elements.size()))
    utilities[ix] = val;
  else
    return;

  if(val > max_util)
    max_val = val;
  if(val < min_util)
    min_val = val;
}

//-------------------------------------------------------------
// Procedure: setUtilRange
//      Note: Set the utility range. If utilities have been set
//            prior, they are examined and adjusted (clipped) if
//            they are outside the range.

void XYGrid::setUtilRange(double lval, double hval)
{
  if(lval > hval)
    return;

  min_util_possible = lval;
  max_util_possible = hval;
  
  int vsize = utilities.size();
  for(int i=0; i<vsize; i++) {
    if(utilities[i] < min_util_possible)
      utilities[i] = min_util_possible;
    if(utilities[i] > max_util_possible)
      utilities[i] = max_util_possible;
  }
}

//-------------------------------------------------------------
// Procedure: getUtil

double XYGrid::getUtil(unsigned int ix) const
{
  if((ix >= 0) && (ix < utilities.size()))
    return(utilities[ix]);
  else
    return(min_util_possible);
}

//-------------------------------------------------------------
// Procedure: initialize
//      Note: An alternative way of initializing convenient
//            for reading a specification from a file.

bool XYGrid::initialize(string given_config_str)
{
  // If initialization fails, elements vector will be empty
  this->clear();

  vector<string> svector = parseString(given_config_str, '@');
  int vsize = svector.size();
  if((vsize != 2) && (vsize != 3))
    return(false);
  
  string poly_string = svector[0];
  string unit_string = svector[1];
  double initial_val = 0.0;

  if(vsize == 3)
    initial_val = atof(svector[2].c_str());

  bounding_poly = stringToPoly(poly_string);
  if(bounding_poly.size() == 0)
    return(false);

  // Label of poly will also be label of this searchgrid.
  string poly_label = bounding_poly.get_label();
  if(poly_label == "") {
    cout << "Un-labeled searchgrid specification" << endl;
    return(false);
  }

  unit_string = stripBlankEnds(unit_string);
  svector = parseString(unit_string, ',');
  vsize = svector.size();
  if(vsize != 2)
    return(false);

  double xl = 0;
  double xh = atof(svector[0].c_str());
  double yl = 0;
  double yh = atof(svector[1].c_str());
  XYSquare unit_square(xl,xh,yl,yh);
  // Check for the validity of the square
  if(!unit_square.isValid())
    return(false);

  bool ok = initialize(bounding_poly, unit_square, initial_val);

  if(ok) {
    config_string = given_config_str;
    label = poly_label;
  }

  return(ok);
}

//-------------------------------------------------------------
// Procedure: handleSegment

void XYGrid::handleSegment(double x1, double y1,
			   double x2, double y2)
{
  for(unsigned int i=0; i<elements.size(); i++) {
    double val = elements[i].segIntersectLength(x1,y1,x2,y2);
    setVal(i, val);
  }
}

//-------------------------------------------------------------
// Procedure: resetFromMin

void XYGrid::resetFromMin()
{
  for(unsigned int i=0; i<elements.size(); i++) {
    double curr_val = getVal(i);
    setVal(i, (curr_val - min_val));
  }
}

//-------------------------------------------------------------
// Procedure: ptIntersect
//   Purpose: Determine is a given point is contained within the
//            the grid, i.e., within any one element

bool XYGrid::ptIntersect(double x, double y) const
{
  unsigned int vsize = elements.size();
  for(unsigned int i=0; i<vsize; i++) 
    if(elements[i].containsPoint(x, y))
      return(true);
  return(false);
}

//-------------------------------------------------------------
// Procedure: ptIntersectBound
//   Purpose: Determine is a given point is contained within the
//            bounding box of all grid squares.

bool XYGrid::ptIntersectBound(double x, double y) const
{
  return(bounding_square.containsPoint(x, y));
}

//-------------------------------------------------------------
// Procedure: segIntersectBound
//   Purpose: Determine is a given line segment intersects the
//            bounding box of all grid squares.

bool XYGrid::segIntersectBound(double x1, double y1,
			       double x2, double y2) const
{
  return(bounding_square.segIntersectLength(x1,y1,x2,y2) > 0);
}

//-------------------------------------------------------------
// Procedure: processDelta
//      Note: format: 
//
//              "LABEL @ index,old_val,new_val,old_util,new_util
//                       index,old_val,new_val:
//                       index,old_val,new_val,old_util,new_util"
//
//            The reason why old_val is part of the structure is
//            so that a series of delta's can be processed in 
//            reverse by an application if needed.

bool XYGrid::processDelta(const string& str)
{
  string stripped_str = stripBlankEnds(str);

  vector<string> svector = parseString(stripped_str, '@');
  unsigned int vsize = svector.size();
  if(vsize != 2)
    return(false);
  if(svector[0] != label)
    return(false);

  svector = parseString(svector[1], ':');
  vsize   = svector.size();

  for(unsigned int i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> dvector = parseString(svector[i], ',');
    int dsize = dvector.size();
    if((dsize != 3) && (dsize != 5))
      return(false);
    for(int j=0; j<dsize; j++)
      dvector[j] = stripBlankEnds(dvector[j]);
    
    unsigned int  index  = atoi(dvector[0].c_str());

    //double old_val = atof(dvector[1].c_str());
    double new_val = atof(dvector[2].c_str());

    if((index < 0) || (index >= values.size()))
      return(false);

    setVal(index, new_val);

    if(dsize == 5) {
      //double old_util = atof(dvector[3].c_str());
      double new_util = atof(dvector[4].c_str());
      setUtil(index, new_util);
    }
  }
  return(true);
}

//-------------------------------------------------------------
// Procedure: initialize

bool XYGrid::initialize(XYPolygon poly,
			const XYSquare&  unit_square, double init_value)
{
  int i, psize = poly.size();
  if(psize == 0)
    return(false);

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
  vector<double>   int_values;
  vector<double>   int_utils;

  bool ok = initialize(outer_square, unit_square, init_value);
  if(!ok)
    return(false);

  for(unsigned i=0; i<elements.size(); i++) {
    xlow  = elements[i].getVal(0,0);
    xhigh = elements[i].getVal(0,1);
    ylow  = elements[i].getVal(1,0);
    yhigh = elements[i].getVal(1,1);
    XYPolygon spoly;
    spoly.add_vertex(xlow,  ylow);
    spoly.add_vertex(xhigh, ylow);
    spoly.add_vertex(xhigh, yhigh);
    spoly.add_vertex(xlow,  yhigh);
    if(spoly.intersects(poly)) {
      int_elements.push_back(elements[i]);
      int_values.push_back(init_value);
      int_utils.push_back(0);
    }
  }

  elements  = int_elements;
  values    = int_values;
  utilities = int_values;
  min_val   = 0;
  max_val   = 0;
  return(true);
}


//-------------------------------------------------------------
// Procedure: initialize

bool XYGrid::initialize(const XYSquare& outer_square,
			const XYSquare& unit_square, double init_val)
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
      elements.push_back(new_square);
      values.push_back(init_val);
    }
  }

#if 0
  if(shift_flag) {
    double shift_x = -x_extra / 2.0;
    double shift_y = -y_extra / 2.0;
    for(int k=0; k<elements.size(); k++) {
      elements[k].shiftX(shift_x);
      elements[k].shiftY(shift_y);
    }
  }
#endif  

  bounding_square = outer_square;
  min_val = 0;
  max_val = 0;
  return(true);
}

//-------------------------------------------------------------
// Procedure: clear

void XYGrid::clear()
{
  elements.clear();
  values.clear();
  config_string = "";
  XYSquare sq;
  bounding_square = sq;
  min_val = 0;
  max_val = 0;
}










