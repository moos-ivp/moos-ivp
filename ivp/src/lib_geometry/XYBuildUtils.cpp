/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYBuildUtils.cpp                                     */
/*    DATE: Mar 16, 2008 Sunday Afternoon at Brugger's           */
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
#include <vector>
#include "XYBuildUtils.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: strintToEllipse
//
/// Initializes a polygon that approximates an ellipse
/// The format of the string is "type=elipse, xpos=val, ypos=val, 
/// major=val, minor=val, degs=val, rads=val, pts=val,
//  snap_value=val, label=val"

XYPolygon stringToEllipse(string str)
{
  XYPolygon null_poly;

  str = tolower(stripBlankEnds(str));
  vector<string> mvector = parseString(str, ',');
  int vsize = mvector.size();

  // Below are the mandatory parameters - check they are set.
  bool xpos_set    = false;
  bool ypos_set    = false;
  bool major_set   = false;
  bool minor_set   = false;
  bool degrees_set = false;  // Either degrees OR radians must
  bool radians_set = false;  // be specified.
  bool pts_set     = false;

  double xpos, ypos, major, minor, radians, degrees, snap=0;
  string label;
  int    pts;
  
  for(int i=0; i<vsize; i++) {
    vector<string> svector = parseString(mvector[i], '=');
    if(svector.size() != 2)
      return(null_poly);
    string param = stripBlankEnds(svector[0]);
    string value = stripBlankEnds(svector[1]);
    if(param == "type") {
      if(value != "ellipse")
	return(null_poly);
    }
    else if((param == "xpos") && (isNumber(value))) {
      xpos_set = true;
      xpos = atof(value.c_str());
    }
    else if((param == "ypos") && (isNumber(value))) {
      ypos_set = true;
      ypos = atof(value.c_str());
    }
    else if((param == "major") && (isNumber(value))) {
      double dval = atof(value.c_str());
      if(dval > 0) {
	major_set = true;
	major = dval;
      }
    }
    else if((param == "minor") && (isNumber(value))) {
      double dval = atof(value.c_str());
      if(dval > 0) {
	minor_set = true;
	minor = dval;
      }
    }
    else if((param == "rads") && (isNumber(value))) {
      radians_set = true;
      radians = atof(value.c_str());
    }
    else if((param == "degs") && (isNumber(value))) {
      degrees_set = true;
      degrees = atof(value.c_str());
    }
    else if((param == "pts") && (isNumber(value))) {
      int ival = atoi(value.c_str());
      if(ival >= 4) {
	pts_set = true;
	pts = ival;
      }
    }
    else if((param == "snap") && (isNumber(value))) {
      double dval = atof(value.c_str());
      if(dval >= 0)
	snap = dval;
    }
  }

  if(!xpos_set || !ypos_set || !major_set || !minor_set || !pts_set)
    return(null_poly);

  if(!radians_set && !degrees_set)
    return(null_poly);

  XYPolygon new_poly;

  double rads = radians;
  if(!radians_set && degrees_set)
    rads = degToRadians(degrees);

  double delta = (2*M_PI) / pts;
  for(int i=0; i<pts; i++) {
    double angle = -M_PI + (i*delta);
    double new_x = xpos + (major/2 * cos(angle) * cos(rads)) - 
      (minor/2 * sin(angle) * sin(rads));
    double new_y = ypos + (minor/2 * sin(angle) * cos(rads)) +
      (major/2 * cos(angle) * sin(rads));
    new_poly.add_vertex(new_x, new_y, false);
  }


  if(snap==0)
    new_poly.apply_snap(snap);
  new_poly.set_label(label);

  new_poly.determine_convexity();
  if(new_poly.is_convex())
    return(new_poly);

  return(null_poly);
}

