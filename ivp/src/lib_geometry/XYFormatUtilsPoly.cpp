/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFormatUtilsPoly.cpp                                */
/*    DATE: May 16, 2009 After bbq @ Hows                        */
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
#include "XYFormatUtilsPoly.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2Poly (Method #0)
//
// Formats: default, radial, wedge, ellipse, rangewedge
// Fields common to all:
//    label, source, msg, active, snap(?)
//
// Fields defined for ellipse:
//    x, y, z, major, minor, rads, degs, pts, snap, msg
// 
// Fields defined for radial:
//    x, y, z, radius, pts, snap, label, active, msg
//
// Fields defined for wedge:
//    x, y, lang, rang, range, pts, snap, label, active, msg
// 
// Fields defined for rangewedge:
//    x, y, lang, rang, range_min, range_max, pts, snap, label, active, msg
//
// Fields defined for pylon poly
//    x1, y1, x2, y2, z, axis_pad, snap, label, active, msg
//
// pts=x1,y1:x2,y2:x3,y3 # label=val # source=val # active=val
// format=radial # pts=12 # radius=40 # label=val # source=val # active=val

XYPolygon string2Poly(string str)
{
  str = stripBlankEnds(str);

  // First try the most likely format since this is the one that is 
  // created by the poly.get_spec() method and is likely to be the one
  // passed around in the greatest volume. The others formats are more 
  // likely to be found in initialization routines so it doesnt need to
  // be terribly fast.
  XYPolygon new_poly = stringStandard2Poly(str);
  if(new_poly.valid())
    return(new_poly);

  // Perform some backward compatibility measures
  if(strContains(str, "format")) {
    string no_white_string = removeWhite(str);
    if(strContains(no_white_string, "format=radial"))
      new_poly = stringRadial2Poly(str);
    else if(strContains(no_white_string, "format=ellipse"))
      new_poly = stringEllipse2Poly(str);
    else if(strContains(no_white_string, "format=wedge"))
      new_poly = stringPieWedge2Poly(str);
    else if(strContains(no_white_string, "format=piewedge"))
      new_poly = stringPieWedge2Poly(str);
    if(strContains(no_white_string, "format=rangewedge"))
      new_poly = stringRangeWedge2Poly(str);
    else if(strContains(no_white_string, "format=pylon"))
      new_poly = stringPylon2Poly(str);

    if(new_poly.valid())
      return(new_poly);
  }
  
  if(strBegins(str, "radial::"))
    return(stringRadial2Poly(str.substr(8)));
  if(strBegins(str, "radial:")) 
    return(stringShortRadial2Poly(str.substr(7)));
  if(strBegins(str, "ellipse::"))
    return(stringEllipse2Poly(str.substr(9)));
  if(strBegins(str, "ellipse:"))
    return(stringEllipse2Poly(str.substr(8)));
  if(strBegins(str, "wedge:"))
    return(stringPieWedge2Poly(str.substr(6)));
  if(strBegins(str, "piewedge:"))
    return(stringPieWedge2Poly(str.substr(9)));
  if(strBegins(str, "rangewedge:"))
    return(stringRangeWedge2Poly(str.substr(11)));
  if(strBegins(str, "pylon:"))
    return(stringPylon2Poly(str.substr(6)));

  // Last chance .....
  return(stringAbbreviated2Poly(str));
}

//---------------------------------------------------------------
// Procedure: stringStandard2Poly  (Method #1)
//      Note: This function is key because it represents the format
//            produced by default when an existing XYPolygon instance
//            is converted into a string.
// 
// Examples: [pts="10,15:20,25:30,35", label=foobar]

XYPolygon stringStandard2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  string rest = stripBlankEnds(str);

  while(rest != "") {
    string left = stripBlankEnds(biteString(rest, '='));
    rest = stripBlankEnds(rest);
    
    if(left == "pts") {
      string pstr = biteStringX(rest, '}');
      
      // Empty set of points is an error
      if(pstr == "")
	return(null_poly);

      // Points should begin with an open brace (but discard now)
      if(pstr[0] != '{') 
	return(null_poly);
      else
	pstr = pstr.substr(1);

      // If more components after pts={}, then it should begin w/ comma
      if(rest != "") {
	if(rest[0] != ',')
	  return(null_poly);
	else
	  rest = rest.substr(1);
      }

      vector<string> svector = parseString(pstr, ':');
      unsigned int i, vsize = svector.size();
      for(i=0; i<vsize; i++) {
	string vertex = stripBlankEnds(svector[i]);
	string xstr = biteStringX(vertex, ',');
	string ystr = biteStringX(vertex, ',');
	string zstr = biteStringX(vertex, ',');
	string pstr = biteStringX(vertex, ',');
	  
	string property;
	  
	if(!isNumber(xstr) || !isNumber(ystr))
	  return(null_poly);
	double xval = atof(xstr.c_str());
	double yval = atof(ystr.c_str());
	double zval = 0;
	if(isNumber(zstr))
	  zval = atof(zstr.c_str());
	else if(zstr != "")
	  property = zstr;
	  
	if(pstr != "") {
	  if(property != "")
	    property += ",";
	  property += pstr;
	}
	new_poly.add_vertex(xval, yval, zval, property, false);
      }
      new_poly.determine_convexity();
    }
    else {
      string right = stripBlankEnds(biteString(rest, ','));
      rest = stripBlankEnds(rest);
      new_poly.set_param(left, right);
    }
  }	  				  
	
  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}

//---------------------------------------------------------------
// Procedure: stringAbbreviated2Poly  (Method #2)
//  Examples: 2,3:-8,5:label,foobar:source,alpha:point_color=blue:
//            vertex_size=4:type,waypoint

XYPolygon stringAbbreviated2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  vector<string> mvector = parseString(str, ':');
  int vsize = mvector.size();
  for(int i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    string left = tolower(biteStringX(mvector[i], ','));
    string rest = mvector[i];

    bool handled = new_poly.set_param(left, rest);
    if(!handled) {
      string xstr = left;
      string ystr = biteStringX(rest, ',');
      string zstr = rest;
      if((zstr != "") && !isNumber(zstr))
	return(null_poly);
      if(!isNumber(xstr) || !isNumber(ystr))
	return(null_poly);
      double xval = atof(xstr.c_str());
      double yval = atof(ystr.c_str());
      if(zstr == "")
	new_poly.add_vertex(xval, yval);
      else {
	double zval = atof(zstr.c_str());
	new_poly.add_vertex(xval, yval, zval);	
      }
    }
  }
  return(new_poly);
}

//---------------------------------------------------------------
//  Procedure: stringEllipse2Poly  (Method #3)
//
/// Initializes a polygon that approximates an ellipse
/// The format of the string is "type=ellipse, x=val, y=val, 
/// major=val, minor=val, degs=val, rads=val, pts=val,
//  snap=val, label=val"

XYPolygon stringEllipse2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  // Below are the mandatory parameters - check they are set.
  bool xpos_set    = false;
  bool ypos_set    = false;
  //bool zval_set    = false;
  bool major_set   = false;
  bool minor_set   = false;
  bool degrees_set = false;  // Either degrees OR radians must
  bool radians_set = false;  // be specified.
  bool pts_set     = false;

  double xpos=0, ypos=0, zval=0, major=0, minor=0, radians=0, degrees=0, snap=0;
  int    pts = 0;
   
  str = stripBlankEnds(str);
  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();

  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(mvector[i], '=')));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());
    if(param == "snap_value")
      param = "snap";

    if(param == "format") {
      if(value != "ellipse")
	return(null_poly);
    }
    else if((param == "active") && (tolower(value)=="false")) {
      null_poly.set_active(false);
      new_poly.set_active(false);
    }
    else if((param == "x") && (isNumber(value))) {
      xpos_set = true;
      xpos = dval;
    }
    else if((param == "y") && (isNumber(value))) {
      ypos_set = true;
      ypos = dval;
    }
    else if((param == "z") && (isNumber(value))) {
      //zval_set = true;
      zval = dval;
    }
    else if((param == "major") && (isNumber(value))) {
      if(dval > 0) {
	major_set = true;
	major = dval;
      }
    }
    else if((param == "minor") && (isNumber(value))) {
      if(dval > 0) {
	minor_set = true;
	minor = dval;
      }
    }
    else if((param == "rads") && (isNumber(value))) {
      radians_set = true;
      radians = dval;
    }
    else if((param == "degs") && (isNumber(value))) {
      degrees_set = true;
      degrees = dval;
    }
    else if((param == "pts") && (isNumber(value))) {
      int ival = atoi(value.c_str());
      if(ival >= 4) {
	pts_set = true;
	pts = ival;
      }
    }
    else if((param == "snap") && isNumber(value) && (dval>0))
      snap = dval;
    else {
      bool handled = new_poly.set_param(param, value);
      if(!handled)
	return(null_poly);
    }
  }

  if(!xpos_set || !ypos_set || !major_set || !minor_set || !pts_set)
    return(null_poly);

  if(!radians_set && !degrees_set)
    return(null_poly);

  double rads = radians;
  if(!radians_set && degrees_set)
    rads = degToRadians(degrees);

  // The "false" parameter in add_vertex() below indicates that a
  // convexity determination is *not* to be made as part of the call
  // which would be the default behavior. The convexity determination
  // is instead delayed until after all the vertices are added to for
  // the sake of efficiency. The determine_convexity() call is made
  // below this block.
  double delta = (2*M_PI) / pts;
  for(int i=0; i<pts; i++) {
    double angle = -M_PI + (i*delta);
    double new_x = xpos + (major/2 * cos(angle) * cos(rads)) - 
      (minor/2 * sin(angle) * sin(rads));
    double new_y = ypos + (minor/2 * sin(angle) * cos(rads)) +
      (major/2 * cos(angle) * sin(rads));
    new_poly.add_vertex(new_x, new_y, zval, false);
  }

  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  // The convexity determination needs to be done before applying
  // the snap value since a snap is rejected if it creates a non-
  // convex poly from a previously determined convex poly. 
  new_poly.determine_convexity();
  if(snap>=0)
    new_poly.apply_snap(snap);

  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}


//---------------------------------------------------------------
//  Procedure: stringRadial2Poly  (Method #4)
//
/// Initializes a polygon that approximates a circle.
/// The format of the string is 
/// "radial:: x=val, y=val, radius=val, pts=val, snap=val, label=val"

XYPolygon stringRadial2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  // Below are the mandatory parameters - check they are set.
  bool xpos_set   = false;
  bool ypos_set   = false;
  //bool zval_set   = false;
  bool radius_set = false;
  bool pts_set    = false;

  double xpos = 0;
  double ypos = 0;
  double zval = 0;
  double radius = 0;
  double snap = 0;
  int    pts = 0;
  
  str = stripBlankEnds(str);
  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();

  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(mvector[i], '='));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());
    if(param == "format") {
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
    else if((param == "z") && (isNumber(value))) {
      //zval_set = true;
      zval = atof(value.c_str());
    }
    else if((param == "active") && (tolower(value)=="false")) {
      null_poly.set_active(false);
      new_poly.set_active(false);
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
      if(ival >= 3) { // Must be at least a triangle
	pts_set = true;
	pts = ival;
      }
    }
    else if((param == "snap") && isNumber(value) && (dval>0))
      snap = dval;
    else {
      bool handled = new_poly.set_param(param, value);
      if(!handled)
	return(null_poly);
    }
  }

  if(!xpos_set || !ypos_set || !radius_set || !pts_set)
    return(null_poly);
  
  // The "false" parameter in add_vertex() below indicates that a
  // convexity determination is *not* to be made as part of the call
  // which would be the default behavior. The convexity determination
  // is instead delayed until after all the vertices are added to for
  // the sake of efficiency. The determine_convexity() call is made
  // below this block.
  double delta = 360.0 / pts;
  for(double deg=(delta/2); deg<360; deg+=delta) {
    double new_x, new_y;
    projectPoint(deg, radius, xpos, ypos, new_x, new_y);
    new_poly.add_vertex(new_x, new_y, zval, false);
  }

  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  // The convexity determination needs to be done before applying
  // the snap value since a snap is rejected if it creates a non-
  // convex poly from a previously determined convex poly. 
  new_poly.determine_convexity();
  if(snap >= 0)
    new_poly.apply_snap(snap);

  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}


//---------------------------------------------------------------
// Procedure: stringShortRadial2Poly  (Method #5)
//
// Examples: "radial: px, py, prad, ppts, snapval, label"
//           "px, py, prad, ppts, snapval, label"

XYPolygon stringShortRadial2Poly(string str)
{
  XYPolygon null_poly;

  if(strBegins(str, "radial:"))
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
// Procedure: stringPieWedge2Poly  (Method #6)
//
/// Initializes a polygon that approximates a pie wedge
/// Format of the string is "type=wedge, x=val, y=val, lang=val, 
//  rang=val, pts=val, range=val, snap=val, label=val"

XYPolygon stringPieWedge2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  // Below are the mandatory parameters - check they are set.
  bool xpos_set  = false;
  bool ypos_set  = false;
  bool lang_set  = false;
  bool rang_set  = false;
  bool range_set = false;
  
  double xpos=0, ypos=0, lang=0, rang=0, range=0, snap=0;
  int    pts=0;
  
  str = stripBlankEnds(str);
  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();

  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(mvector[i], '=')));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());
    if(param == "snap_value")
      param = "snap";

    if(param == "format") {
      if((value != "wedge") && (value != "piewedge"))
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
    else if((param == "lang") && (isNumber(value))) {
      lang_set = true;
      lang = atof(value.c_str());
    }
    else if((param == "rang") && (isNumber(value))) {
      rang_set = true;
      rang = atof(value.c_str());
    }
    else if((param == "range") && (isNumber(value))) {
      range_set = true;
      range = atof(value.c_str());
    }
    else if((param == "pts") && (isNumber(value))) {
      int ival = atoi(value.c_str());
      if(ival >= 0)
	pts = ival;
    }
    else if((param == "snap") && isNumber(value) && (dval > 0))
	snap = dval;
    else {
      bool handled = new_poly.set_param(param, value);
      if(!handled) 
	return(null_poly);
    }
  }

  if(!xpos_set || !ypos_set || !lang_set || !rang_set || !range_set)
    return(null_poly);

  lang = angle360(lang);
  rang = angle360(rang);
  if(rang < lang)
    rang += 360;

  double delta = 0;
  if(rang > lang)
    delta = (rang - lang) / ((double)(pts));

  new_poly.add_vertex(xpos, ypos);

  // The "false" parameter in add_vertex() below indicates that a
  // convexity determination is *not* to be made as part of the call
  // which would be the default behavior. The convexity determination
  // is instead delayed until after all the vertices are added to for
  // the sake of efficiency. The determine_convexity() call is made
  // below this block.
  double ptx, pty;
  projectPoint(lang, range, xpos, ypos, ptx, pty);
  new_poly.add_vertex(ptx, pty, 0, false);

  double project_angle = lang;
  while(project_angle < rang) {
    project_angle += delta;
    if(project_angle > rang)
      project_angle = rang;
    projectPoint(project_angle, range, xpos, ypos, ptx, pty);
    new_poly.add_vertex(ptx, pty, 0, false);
  }
  
  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  // The convexity determination needs to be done before applying
  // the snap value since a snap is rejected if it creates a non-
  // convex poly from a previously determined convex poly. 
  new_poly.determine_convexity();
  if(snap>=0)
    new_poly.apply_snap(snap);

  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}


//---------------------------------------------------------------
// Procedure: stringRangeWedge2Poly  (method #7)
//
/// Initializes a polygon that approximates a range wedge
/// Format of the string is "type=wedge, x=val, y=val, lang=val, 
//  rang=val, pts=val, range=val, snap=val, label=val"

XYPolygon stringRangeWedge2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  // Below are the mandatory parameters - check they are set.
  bool xpos_set  = false;
  bool ypos_set  = false;
  bool lang_set  = false;
  bool rang_set  = false;
  bool range_set = false;

  double range_min = 0;
  double xpos = 0;
  double ypos = 0;
  double lang = 0;
  double rang = 0;
  double snap = 0;
  double range_max = 0;
  int    pts=0;
  
  str = stripBlankEnds(str);
  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();

  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(mvector[i], '=')));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());
    if(param == "snap_value")
      param = "snap";

    if(param == "format") {
      if(value != "rangewedge")
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
    else if((param == "lang") && (isNumber(value))) {
      lang_set = true;
      lang = atof(value.c_str());
    }
    else if((param == "rang") && (isNumber(value))) {
      rang_set = true;
      rang = atof(value.c_str());
    }
    else if((param == "range_min") && (isNumber(value)))
      range_min = atof(value.c_str());
    else if((param == "range_max") && (isNumber(value))) {
      range_set = true;
      range_max = atof(value.c_str());
    }
    else if((param == "pts") && (isNumber(value))) {
      int ival = atoi(value.c_str());
      if(ival >= 0)
	pts = ival;
    }
    else if((param == "snap") && isNumber(value) && (dval>0))
      snap = dval;
    else {
      bool handled = new_poly.set_param(param, value);
      if(!handled)
	return(null_poly);
    }
  }

  if(!xpos_set || !ypos_set || !lang_set || !rang_set || !range_set)
    return(null_poly);

  if(range_min >= range_max)
    return(null_poly);

  lang = angle360(lang);
  rang = angle360(rang);
  if(rang < lang)
    rang += 360;

  double delta = 0;
  if(rang > lang)
    delta = (rang - lang) / ((double)(pts));

  double ptx, pty, project_angle;

  // The "false" parameter in add_vertex() below indicates that a
  // convexity determination is *not* to be made as part of the call
  // which would be the default behavior. The convexity determination
  // is instead delayed until after all the vertices are added to for
  // the sake of efficiency. The determine_convexity() call is made
  // below this block.

  // First add the points from the outer arc
  projectPoint(lang, range_max, xpos, ypos, ptx, pty);
  new_poly.add_vertex(ptx, pty);
  project_angle = lang;
  while(project_angle < rang) {
    project_angle += delta;
    if(project_angle > rang)
      project_angle = rang;
    projectPoint(project_angle, range_max, xpos, ypos, ptx, pty);
    new_poly.add_vertex(ptx, pty);
  }

  // Then add the points from the inner arc
  projectPoint(rang, range_min, xpos, ypos, ptx, pty);
  new_poly.add_vertex(ptx, pty);
  project_angle = rang;
  while(project_angle > lang) {
    project_angle -= delta;
    if(project_angle < lang)
      project_angle = lang;
    projectPoint(project_angle, range_min, xpos, ypos, ptx, pty);
    new_poly.add_vertex(ptx, pty);
  }

  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  // The convexity determination needs to be done before applying
  // the snap value since a snap is rejected if it creates a non-
  // convex poly from a previously determined convex poly. 
  new_poly.determine_convexity();
  if(snap>=0)
    new_poly.apply_snap(snap);

  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}

//---------------------------------------------------------------
// Procedure: stringPylon2Poly (Method #8)
//
//                     o (x2,y2) 
//                   /   
//                 /     
//      (x1,y1)  o    
//
/// Initializes a polygon (rectangle) based on two points
/// The format of the string is "x1=val, y1=val, x2=val, y2=val, 
//           axis_pad=val, perp_pad=val, [zval=val]".


XYPolygon stringPylon2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  // Below are the mandatory parameters - check they are set.
  bool x1_set  = false;
  bool y1_set  = false;
  bool x2_set  = false;
  bool y2_set  = false;
  //bool zval_set = false;
  bool axis_pad_set = false;
  bool perp_pad_set = false;  // Either degrees OR radians must

  double x1=0, y1=0, x2=0, y2=0, zval=0, axis_pad=0, perp_pad=0, snap=0;
  
  str = stripBlankEnds(str);
  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();

  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(mvector[i], '=')));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());
    if(param == "snap_value")
      param = "snap";

    if((param == "x1") && isNumber(value)) {
      x1_set = true;
      x1 = dval;
    }
    else if((param == "y1") && isNumber(value)) {
      y1_set = true;
      y1 = dval;
    }
    else if((param == "x2") && isNumber(value)) {
      x2_set = true;
      x2 = dval;
    }
    else if((param == "y2") && isNumber(value)) {
      y2_set = true;
      y2 = dval;
    }
    else if((param == "z") && isNumber(value)) {
      //zval_set = true;
      zval = dval;
    }
    else if((param == "axis_pad") && isNumber(value)) {
      if(dval >= 0) {
	axis_pad_set = true;
	axis_pad = dval;
      }
    }
    else if((param == "perp_pad") && isNumber(value)) {
      if(dval >= 0) {
	perp_pad_set = true;
	perp_pad = dval;
      }
    }
    else if((param == "snap") && isNumber(value) && (dval>0))
      snap = dval;
    else {
      bool handled = new_poly.set_param(param, value);
      if(!handled)
	return(null_poly);
    }
  }

  if(!x1_set || !y1_set || !x2_set || !y2_set)
    return(null_poly);

  if(!axis_pad_set || !perp_pad_set)
    return(null_poly);

  double rel_ang = relAng(x1,y1,x2,y2);
  
  double px1, py1;
  projectPoint(rel_ang-180, axis_pad, x1,  y1,  px1, py1);
  projectPoint(rel_ang-90,  perp_pad, px1, py1, px1, py1);
  
  double px2, py2;
  projectPoint(rel_ang-180, axis_pad, x1,  y1,  px2, py2);
  projectPoint(rel_ang+90,  perp_pad, px2, py2, px2, py2);
  
  double px3, py3;
  projectPoint(rel_ang,    axis_pad, x2,  y2,  px3, py3);
  projectPoint(rel_ang+90, perp_pad, px3, py3, px3, py3);

  double px4, py4;
  projectPoint(rel_ang,    axis_pad, x2,  y2,  px4, py4);
  projectPoint(rel_ang-90, perp_pad, px4, py4, px4, py4);


  // The "false" parameter in add_vertex() below indicates that a
  // convexity determination is *not* to be made as part of the call
  // which would be the default behavior. The convexity determination
  // is instead delayed until after all the vertices are added to for
  // the sake of efficiency. The determine_convexity() call is made
  // below this block.
  new_poly.add_vertex(px1, py1, zval, false);
  new_poly.add_vertex(px2, py2, zval, false);
  new_poly.add_vertex(px3, py3, zval, false);
  new_poly.add_vertex(px4, py4, zval, false);

  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  // The convexity determination needs to be done before applying
  // the snap value since a snap is rejected if it creates a non-
  // convex poly from a previously determined convex poly. 
  new_poly.determine_convexity();
  if(snap>=0)
    new_poly.apply_snap(snap);

  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}









