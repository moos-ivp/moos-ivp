/*****************************************************************/
/*    NAME: Andrew Shafer and Michael Benjamin                   */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: XYArtifactGrid.cpp                                   */
/*    DATE: 3 JUL 2007                                           */
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

#include "XYArtifactGrid.h"

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <numeric> // For accumulate
#include "MBUtils.h"
#include "XYBuildUtils.h"
#include "XYSquare.h"
#include "XYPolygon.h"

using namespace std;

XYArtifactGrid::XYArtifactGrid(){}

//---------------------------------------------------------------
// Procedure: getElement
//
/// Returns element \param ix

XYSquare XYArtifactGrid::getElement(unsigned int ix) const
{
  XYSquare retElement;
  
  if((ix >= 0) && (ix < elements.size()))
    return(elements[ix]);
  else
    return(retElement);
}

//---------------------------------------------------------------
// Procedure: setClearance
//
/// Set the clearance in \param index to \param val
void XYArtifactGrid::setClearance(unsigned int index, double val)
{
  if((index >= 0) && (index < clearances.size())){
    if(val > 1){
      clearances.at(index) = 1;
      return;
    }
    
    if(val < 0){
      clearances.at(index) = 0;
      return;
    }
    
    clearances.at(index) = val;
  }
}		

//---------------------------------------------------------------
// Procedure: getClearance

/// Get the clearance in \param index
double XYArtifactGrid::getClearance(unsigned int index) const
{
  if((index >= 0) && (index < clearances.size())){
    return clearances.at(index);
  }
  else return -1;
}

/// Computes the average clearance in all of the cells

double XYArtifactGrid::getAvgClearance() const
{
  if(clearances.size() == 0) 
    return(0);
  
  double sum = accumulate(clearances.begin(), clearances.end(), 0.0);
  double count = (double)clearances.size();
  
  return(sum / count);
}

/// Set the artifact vector in \param index to \param artvec
void XYArtifactGrid::setArtVec(unsigned int index, const ArtVec & artvec)
{
  if((index >= 0) && (index < artifacts.size())){
    artifacts.at(index) = artvec;
  }
}

/// Get the scanned bit in \param index
ArtVec XYArtifactGrid::getArtVec(unsigned int index) const
{
  ArtVec nullvec;
  if((index >= 0) && (index < artifacts.size())) 
    return(artifacts.at(index));
  else
    return(nullvec);
}

uint XYArtifactGrid::getDetectedArts() const
{
  int count = 0;
  
  vector<ArtVec>::const_iterator p;
  for(p = artifacts.begin(); p != artifacts.end(); p++)
    count += p->size();
  
  return(count);
}


/// The initialization string looks like:
/// polystring@elementstring
bool XYArtifactGrid::initialize(string given_config_str)
{
  // If initialization fails, elements vector will be empty
  this->clear();
  
  vector<string> svector = parseString(given_config_str, '@');
  int vsize = svector.size();
  if (vsize != 2)
    return(false);
  
  string poly_string  = svector.at(0);
  string unit_string  = svector.at(1);
  
  bounding_poly = stringToPoly(poly_string);
  if(!bounding_poly.is_convex()){
    cout << "Bounding poly failed initialization" << endl;
    cout << "I tried: " << poly_string << endl;
    return(false);
  }
  
  // Label of poly will also be label of this searchgrid.
  string poly_label = bounding_poly.get_label();
  if(poly_label == "") {
    cout << "Un-labeled searchgrid specification" << endl;
    return(false);
  }
  
  unit_string = stripBlankEnds(unit_string);
  svector = parseString(unit_string, ',');
  vsize = svector.size();
  if(vsize != 2){
    cout << "Unit square failed parsing" << endl;
    cout << "I tried: " << unit_string << endl;
    return(false);
  }
  
  double xl = 0;
  double xh = atof(svector.at(0).c_str());
  double yl = 0;
  double yh = atof(svector.at(1).c_str());
  XYSquare unit_square(xl,xh,yl,yh);
  // Check for the validity of the square
  if(!unit_square.isValid()){
    cout << "Unit square failed initialization" << endl;
    return(false);
  }
  
  bool ok = initialize(bounding_poly, unit_square);
  
  if(ok) {
    config_string = given_config_str;
    label = poly_label;
  }
  
  return(ok);
}


/// Checks if a given point is contained in any sub-element
bool XYArtifactGrid::containsPoint(double x, double y) const
{
	std::vector<XYSquare>::const_iterator p;
	for (p = elements.begin(); p != elements.end(); p++){
		if (p->containsPoint(x, y)) return true;
	}
	return false;
}

/// Checks if a given point is contained in the bounding square
bool XYArtifactGrid::ptIntersectBound(double x, double y) const
{
  return(bounding_square.containsPoint(x, y));
}

/// Checks if a given segment intersects the bounding square
bool XYArtifactGrid::segIntersectBound(double x1, double y1,
				    double x2, double y2) const
{
  return(bounding_square.segIntersectLength(x1,y1,x2,y2) > 0);
}

/// A delta string has the format:
/// LABEL@index,old_clearance,new_clearance[:index...]
bool XYArtifactGrid::processDelta(const string& str)
{
  string stripped_str = stripBlankEnds(str);
  
  vector<string> svector = parseString(stripped_str, '@');
  
  unsigned int vsize = svector.size();
  if(vsize != 2)
    return(false);
  if(svector.at(0) != label)
    return(false);
  
  svector = parseString(svector.at(1), ':');
  vsize   = svector.size();
  
  for(unsigned int i=0; i<vsize; i++) { // For each index,old_clearance,new_clearance
    svector.at(i) = stripBlankEnds(svector.at(i));
    vector<string> dvector = parseString(svector[i], ',');
    int dsize = dvector.size();
    if (dsize != 3)
      return(false);
    for(int j=0; j<dsize; j++)
      dvector[j] = stripBlankEnds(dvector[j]);
    
    unsigned int  index  = atoi(dvector.at(0).c_str());
    //double old_clearance = atof(dvector.at(1).c_str());
    double new_clearance = atof(dvector.at(2).c_str());
    
    if((index < 0) || (index >= clearances.size()))
      return(false);
    
    setClearance(index, new_clearance);
  }
  
  return(true);
}

/// Creates the squares that fill the polygon
bool XYArtifactGrid::initialize(const XYPolygon& poly, const XYSquare&  unit_square)
{
  unsigned int i, psize = poly.size();
  if(psize == 0)
    return false;
  
  // Part One: get bounding square for the polygon and create 
  //           elements based on that square
  double xlow  = poly.get_vx(0);
  double xhigh = xlow;
  double ylow  = poly.get_vy(0);
  double yhigh = ylow;
  
  for(i=1; i<psize; i++) {
    double x = poly.get_vx(i);
    double y = poly.get_vy(i);
    if(x < xlow)    xlow  = x;
    if(x > xhigh)   xhigh = x;
    if(y < ylow)    ylow  = y;
    if(y > yhigh)   yhigh = y;
  }
  
  XYSquare outer_square(xlow, xhigh, ylow, yhigh);
  
  vector<XYSquare>    int_elements;
  
  bool ok = initialize(outer_square, unit_square);
  if(!ok)
    return false;
  
  // elements now contains all the squares that fill the bounding
  // square.  We'll keep elements that are contained in the poly
  for(i=0; i<elements.size(); i++) {
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
    }
  }
  
  // This assignment sets elements to contain only the squares in the bounding poly
  elements = int_elements;
  
  int isize = elements.size();
  
  std::vector<double>   int_clearances(isize, 0);
  std::vector<ArtVec>   int_artifacts(isize);
  
  artifacts = int_artifacts;
  clearances = int_clearances;
  
  return(true);
}


//-------------------------------------------------------------
// Procedure: initialize

bool XYArtifactGrid::initialize(const XYSquare& outer_square, const XYSquare& unit_square)
{
  double outer_x_len = outer_square.getLengthX();
  double outer_y_len = outer_square.getLengthY();
  double unit_x_len  = unit_square.getLengthX();
  double unit_y_len  = unit_square.getLengthY();
  
  if(outer_y_len < unit_y_len)
    return false;
  if(outer_x_len < unit_x_len)
    return false;

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
    double x_low  = (i * unit_x_len) + outer_square.getVal(0,0);
    double x_high = x_low + unit_x_len;
    for(int j=0; j<y_count; j++) {
      double y_low  = (j * unit_y_len) + outer_square.getVal(1,0);
      double y_high = y_low + unit_y_len;
      new_square.set(x_low, x_high, y_low, y_high);
      elements.push_back(new_square);
    }
  }
  
  bounding_square = outer_square;
  return(true);
}

//-------------------------------------------------------------
// Procedure: clear

void XYArtifactGrid::clear()
{
  elements.clear();
  clearances.clear();
  artifacts.clear();
  
  config_string = "";
  label = "";
  XYSquare sq;
  bounding_square = sq;
  XYPolygon poly;
  bounding_poly = poly;
}


