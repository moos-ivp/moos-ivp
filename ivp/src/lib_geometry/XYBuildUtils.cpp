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
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: stringToPoly
//

XYPolygon stringToPoly(string str)
{
  str = stripBlankEnds(str);

  XYPolygon null_poly;
  if(!strncasecmp("radial::", str.c_str(), 8)) {
    str = str.c_str()+8;
    return(stringPairsToRadialPoly(str));
  }
  else if(!strncasecmp("radial:", str.c_str(), 7)) {
    str = str.c_str()+7;
    return(stringShortToRadialPoly(str));
  }
  else if(!strncasecmp("ellipse::", str.c_str(), 9)) {
    str = str.c_str()+9;
    return(stringPairsToEllipsePoly(str));
  }
  else if(!strncasecmp("pts:", str.c_str(), 5)) {
    str = str.c_str()+5;
    return(stringShortToPointsPoly(str));
  }
  else
    return(stringShortToPointsPoly(str));

  return(null_poly);
}



//---------------------------------------------------------------
// Procedure: stringPairsToEllipsePoly
//
/// Initializes a polygon that approximates an ellipse
/// The format of the string is "type=elipse, x=val, y=val, 
/// major=val, minor=val, degs=val, rads=val, pts=val,
//  snap_value=val, label=val"

XYPolygon stringPairsToEllipsePoly(string str)
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
    else if((param == "x") && (isNumber(value))) {
      xpos_set = true;
      xpos = atof(value.c_str());
    }
    else if((param == "y") && (isNumber(value))) {
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
    else if(param == "label") {
      label = value;
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


  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  // The convexity determination needs to be done before applying
  // the snap value since a snap is rejected if it creates a non-
  // convex poly from a previously determined convex poly. 
  new_poly.determine_convexity();
  if(snap>=0)
    new_poly.apply_snap(snap);
  new_poly.set_label(label);

  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}



//---------------------------------------------------------------
// Procedure: stringPairsToRadialPoly
//
/// Initializes a polygon that approximates a circle.
/// The format of the string is 
/// "radial:: x=val, y=val, radius=val, pts=val, snap=val, label=val"

XYPolygon stringPairsToRadialPoly(string str)
{
  XYPolygon null_poly;

  str = tolower(stripBlankEnds(str));
  vector<string> mvector = parseString(str, ',');
  int vsize = mvector.size();

  // Below are the mandatory parameters - check they are set.
  bool xpos_set   = false;
  bool ypos_set   = false;
  bool radius_set = false;
  bool pts_set    = false;

  double xpos, ypos, radius, snap=0;
  string label;
  int    pts;
  
  for(int i=0; i<vsize; i++) {
    vector<string> svector = parseString(mvector[i], '=');
    if(svector.size() != 2)
      return(null_poly);
    string param = stripBlankEnds(svector[0]);
    string value = stripBlankEnds(svector[1]);
    if(param == "type") {
      if(value != "radial")
	return(null_poly);
    }
    else if((param == "x") && (isNumber(value))) {
      xpos_set = true;
      xpos = atof(value.c_str());
    }
    else if((param == "y") && (isNumber(value))) {
      ypos_set = true;
      ypos = atof(value.c_str());
    }
    else if((param == "radius") && (isNumber(value))) {
      double dval = atof(value.c_str());
      if(dval > 0) {
	radius_set = true;
	radius = dval;
      }
    }
    else if((param == "pts") && (isNumber(value))) {
      int ival = atoi(value.c_str());
      if(ival >= 3) {
	pts_set = true;
	pts = ival;
      }
    }
    else if((param == "snap") && (isNumber(value))) {
      double dval = atof(value.c_str());
      if(dval >= 0)
	snap = dval;
    }
    else if(param == "label") {
      label = value;
    }
  }

  if(!xpos_set || !ypos_set || !radius_set || !pts_set)
    return(null_poly);
  
  XYPolygon new_poly;

  double delta = 360.0 / pts;
  for(double deg=(delta/2); deg<360; deg+=delta) {
    double new_x, new_y;
    projectPoint(deg, radius, xpos, ypos, new_x, new_y);
    new_poly.add_vertex(new_x, new_y, false);
  }

  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  // The convexity determination needs to be done before applying
  // the snap value since a snap is rejected if it creates a non-
  // convex poly from a previously determined convex poly. 
  new_poly.determine_convexity();
  if(snap >= 0)
    new_poly.apply_snap(snap);
  new_poly.set_label(label);

  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}


//---------------------------------------------------------------
// Procedure: stringShortToRadialPoly
//

XYPolygon stringShortToRadialPoly(string str)
{
  XYPolygon null_poly;

  if(!strncasecmp("radial:", str.c_str(), 7))
    str = str.c_str()+7;

  vector<string> svector = parseString(str, ',');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++)
    svector[i] = stripBlankEnds(svector[i]);

  if((vsize < 4) || (vsize > 6))
    return(null_poly);

  double px   = atof(svector[0].c_str());
  double py   = atof(svector[1].c_str());
  double prad = atof(svector[2].c_str());
  double ppts = atof(svector[3].c_str());

  if(prad <= 0)
    return(null_poly);

  XYPolygon new_poly;
  double snap_value = 0;
  if(vsize >= 5) {
    snap_value = atof(svector[4].c_str());
    if(snap_value < 0)
      snap_value = 0;
  }

  if(vsize == 6) // Label present
    new_poly.set_label(svector[5]);

  double delta = 360.0 / ppts;
  for(double deg=(delta/2); deg<360; deg+=delta) {
    double new_x, new_y;
    projectPoint(deg, prad, px, py, new_x, new_y);
    new_poly.add_vertex(new_x, new_y, false);
  }

  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  // The convexity determination needs to be done before applying
  // the snap value since a snap is rejected if it creates a non-
  // convex poly from a previously determined convex poly. 
  new_poly.determine_convexity();
  if(snap_value >= 0)
    new_poly.apply_snap(snap_value);

  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}


//---------------------------------------------------------------
// Procedure: stringShortToPointsPoly
//

XYPolygon stringShortToPointsPoly(string str)
{
  XYPolygon null_poly;

  if(!strncasecmp("pts:", str.c_str(), 4))
    str = str.c_str()+4;
  else if(!strncasecmp("points:", str.c_str(), 7))
    str = str.c_str()+7;

  XYPolygon new_poly;

  vector<string> mvector = parseString(str, ':');
  int vsize = mvector.size();
  for(int i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    vector<string> svector = parseString(mvector[i], ',');
    if(svector.size() != 2)
      return(null_poly);
    string xstr = stripBlankEnds(svector[0]);
    string ystr = stripBlankEnds(svector[1]);

    if((!isNumber(xstr)) || (!isNumber(ystr))) {
      xstr = tolower(xstr);
      if(xstr == "label") 
	new_poly.set_label(ystr);
      else
	return(null_poly);
    }
    else {
      double xval = atof(xstr.c_str());
      double yval = atof(ystr.c_str());
      new_poly.add_vertex(xval, yval, false);
    }
  }

  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  new_poly.determine_convexity();
  return(new_poly);
}
