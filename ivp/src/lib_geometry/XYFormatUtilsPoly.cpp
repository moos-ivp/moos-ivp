/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYFormatUtilsPoly.cpp                                */
/*    DATE: May 16, 2009 After bbq @ Hows                        */
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
#ifdef _WIN32
   #define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include "XYFormatUtilsPoly.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

#ifdef _WIN32
  #define strncasecmp strnicmp
#endif

using namespace std;

//---------------------------------------------------------------
// Procedure: #0 string2Poly
//
// Formats: default, radial, wedge, ellipse, rangewedge
// Fields common to all:
//    label, source, active, snap(?)
//
// Fields defined for ellipse:
//    x, y, z, major, minor, rads, degs, pts, snap
// 
// Fields defined for radial:
//    x, y, z, radius, pts, snap, label, active
//
// Fields defined for wedge:
//    x, y, lang, rang, range, pts, snap, label, active
// 
// Fields defined for rangewedge:
//    x, y, lang, rang, range_min, range_max, pts, snap, label, active
//
// Fields defined for pylon poly
//    x1, y1, x2, y2, z, axis_pad, snap, label, active
//
// pts=x1,y1:x2,y2:x3,y3 # label=val # source=val # active=val
// format=radial # pts=12 # radius=40 # label=val # source=val # active=val

XYPolygon string2Poly(string str)
{
  XYPolygon null_poly;
  
  str = stripBlankEnds(str);
  
  // Support for now a few depricated ways of specifying a poly
  str = findReplace(str, "radial::", "format=radial,");
  str = findReplace(str, "ellipse::", "format=ellipse,");

  if(!strncasecmp("radial:", str.c_str(), 7)) {
    str = str.c_str()+7;
    return(stringShortRadial2Poly(str));
  }

  str = stripBlankEnds(str);
  vector<string> svector = parseStringQ(str, ',');
  unsigned int i, vsize = svector.size();
  string format = "default";
  for(i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    string left  = stripBlankEnds(biteString(svector[i], '='));
    string right = stripBlankEnds(svector[i]);
    if((left == "format") && (right != ""))
      format = right;
  }
  format=tolower(format);
  
  if(format == "points")
    return(stringPoints2Poly(str));
  if(format == "default")
    return(stringShortPoints2Poly(str));
  else if(format == "radial")
    return(stringRadial2Poly(str));
  else if(format == "ellipse")
    return(stringEllipse2Poly(str));
  else if(format == "pylon")
    return(stringPylon2Poly(str));
  else if((format == "wedge") || (format=="piewedge"))
    return(stringPieWedge2Poly(str));
  else if(format == "rangewedge")
    return(stringRangeWedge2Poly(str));
  else
    return(null_poly);
}

//---------------------------------------------------------------
//  Procedure: stringEllipse2Poly  (Method #1)
//
/// Initializes a polygon that approximates an ellipse
/// The format of the string is "type=elipse, x=val, y=val, 
/// major=val, minor=val, degs=val, rads=val, pts=val,
//  snap=val, label=val"

XYPolygon stringEllipse2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  // Below are the mandatory parameters - check they are set.
  bool xpos_set    = false;
  bool ypos_set    = false;
  bool zval_set    = false;
  bool major_set   = false;
  bool minor_set   = false;
  bool degrees_set = false;  // Either degrees OR radians must
  bool radians_set = false;  // be specified.
  bool pts_set     = false;

  double xpos, ypos, zval, major, minor, radians, degrees, snap=0;
  int    pts;
   
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
    else if((param == "x") && (isNumber(value))) {
      xpos_set = true;
      xpos = dval;
    }
    else if((param == "y") && (isNumber(value))) {
      ypos_set = true;
      ypos = dval;
    }
    else if((param == "z") && (isNumber(value))) {
      zval_set = true;
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
    else if(param == "label")
      new_poly.set_label(value);
    else if(param == "type")
      new_poly.set_type(value);
    else if(param == "source")
      new_poly.set_source(value);
    else if(param == "vertex_color")
      new_poly.set_vertex_color(value);
    else if(param == "edge_color")
      new_poly.set_edge_color(value);
    else if((param == "vertex_size") && (dval >= 0))
      new_poly.set_vertex_size(dval);
    else if((param == "edge_size") && (dval >= 0))
      new_poly.set_edge_size(dval);
    else if(param == "active")
      new_poly.set_active(tolower(value) == "true");
    else
      return(null_poly);
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
//  Procedure: stringRadial2Poly  (Method #2)
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
  bool zval_set   = false;
  bool radius_set = false;
  bool pts_set    = false;

  double xpos = 0, ypos = 0, zval = 0, radius = 0, snap=0;
  int    pts;
  
  str = stripBlankEnds(str);
  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();

  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(mvector[i], '=')));
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
      zval_set = true;
      zval = atof(value.c_str());
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
    else if(param == "label")
      new_poly.set_label(value);
    else if(param == "type")
      new_poly.set_type(value);
    else if(param == "source")
      new_poly.set_source(value);
    else if(param == "vertex_color")
      new_poly.set_vertex_color(value);
    else if(param == "edge_color")
      new_poly.set_edge_color(value);
    else if((param == "vertex_size") && (dval >= 0))
      new_poly.set_vertex_size(dval);
    else if((param == "edge_size") && (dval >= 0))
      new_poly.set_edge_size(dval);
    else if(param == "active")
      new_poly.set_active(tolower(value) == "true");
    else
      return(null_poly);
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
// Procedure: stringShortRadial2Poly  (Method #3)
//
// Examples: "radial: px, py, prad, ppts, snapval, label"
//           "px, py, prad, ppts, snapval, label"

XYPolygon stringShortRadial2Poly(string str)
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
// Procedure: stringPoints2Poly  (Method #4)
// 
// Examples: [pts="10,15:20,25:30,35", label=foobar]
//           [label=foobar, pts="10,15:20,25:30,35"]
//           [label=foobar, "10,15 : 20,25 : 30,35"]

XYPolygon stringPoints2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  double snap_val = 0;
  str = stripBlankEnds(str);
  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();

  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(mvector[i], '=')));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());
    if(param == "snap_value")
      param = "snap";

    if(param=="format") {
      if(value!="default")
	return(null_poly);
    }
    else if(param == "label")
      new_poly.set_label(value);
    else if(param == "active")
      new_poly.set_active(tolower(value)=="true");
    else if((param == "snap") && isNumber(value) && (dval >= 0))
      snap_val = dval;
    else if(param == "type")
      new_poly.set_type(value);
    else if(param == "source")
      new_poly.set_source(value);
    else if(param == "vertex_color")
      new_poly.set_vertex_color(value);
    else if(param == "edge_color")
      new_poly.set_edge_color(value);
    else if((param == "vertex_size") && (dval >= 0))
      new_poly.set_vertex_size(dval);
    else if((param == "edge_size") && (dval >= 0))
      new_poly.set_edge_size(dval);
    else if((param=="pts")||(param=="points")||(value=="")) {
      string xypairs = value;
      if(value == "")
	xypairs = param;
      vector<string> pairs_vector = parseString(xypairs, ':');
      unsigned int j, psize = pairs_vector.size();
      for(j=0; j<psize; j++) {
	vector<string> svector = parseString(pairs_vector[i], ',');
	if((svector.size() < 2) || (svector.size() > 3))
	  return(null_poly);
	string xstr = stripBlankEnds(svector[0]);
	string ystr = stripBlankEnds(svector[1]);
	string zstr = "";
	if(svector.size()==3)
	  zstr = stripBlankEnds(svector[2]);
	if((zstr != "") && (!isNumber(zstr)))
	  return(null_poly);
    
	if((!isNumber(xstr)) || (!isNumber(ystr))) {
	  xstr = tolower(xstr);
	  if(xstr == "label") 
	    new_poly.set_label(ystr);
	  else if(xstr == "active") 
	    new_poly.set_active(tolower(ystr)=="true");
	  else
	    return(null_poly);
	}
	else {
	  double xval = atof(xstr.c_str());
	  double yval = atof(ystr.c_str());
	  if(zstr == "")
	    new_poly.add_vertex(xval, yval, false);
	  else {
	    double zval = atof(zstr.c_str());
	    new_poly.add_vertex(xval, yval, zval, false);
	  }	
	}
      }
    }
  }

  // Make a call to determine_convexity here because convexity 
  // determinations are not made when adding vertices above.
  // The convexity determination needs to be done before applying
  // the snap value since a snap is rejected if it creates a non-
  // convex poly from a previously determined convex poly. 
  new_poly.determine_convexity();
  if(snap_val >= 0)
    new_poly.apply_snap(snap_val);

  if(new_poly.is_convex())
    return(new_poly);
  else
    return(null_poly);
}

//---------------------------------------------------------------
// Procedure: stringShortPoints2Poly  (Method #5)
//  Examples: 2,3:-8,5:label,foobar:source,alpha:point_color=blue:
//            vertex_size=4:type,waypoint

XYPolygon stringShortPoints2Poly(string str)
{
  XYPolygon null_poly;
  XYPolygon new_poly;

  vector<string> mvector = parseString(str, ':');
  int vsize = mvector.size();
  for(int i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    string left = tolower(stripBlankEnds(biteString(mvector[i], ',')));
    string rest = stripBlankEnds(mvector[i]);
    
    if(left == "label") 
      new_poly.set_label(rest);
    else if(left == "label_color") 
      new_poly.set_label_color(rest);
    else if(left == "vertex_color") 
      new_poly.set_vertex_color(rest);
    else if(left == "edge_color")
      new_poly.set_edge_color(rest);
    else if((left == "edge_size") && isNumber(rest))
      new_poly.set_edge_size(atof(rest.c_str()));
    else if((left == "vertex_size") && isNumber(rest))
      new_poly.set_vertex_size(atof(rest.c_str()));
    else if(left == "active") 
      new_poly.set_active(tolower(rest)=="true");
    else {
      string xstr = left;
      string ystr = stripBlankEnds(biteString(rest, ','));
      string zstr = stripBlankEnds(rest);
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
  
  double xpos, ypos, lang, rang, range, snap=0;
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
    else if(param == "label")
      new_poly.set_label(value);
    else if(param == "type")
      new_poly.set_type(value);
    else if(param == "source")
      new_poly.set_source(value);
    else if(param == "vertex_color")
      new_poly.set_vertex_color(value);
    else if(param == "edge_color")
      new_poly.set_edge_color(value);
    else if((param == "vertex_size") && (dval >= 0))
      new_poly.set_vertex_size(dval);
    else if((param == "edge_size") && (dval >= 0))
      new_poly.set_edge_size(dval);
    else if(param == "active")
      new_poly.set_active(tolower(value) == "true");
    else
      return(null_poly);
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
  double xpos, ypos, lang, rang, range_max, snap=0;
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
    else if(param == "label")
      new_poly.set_label(value);
    else if(param == "type")
      new_poly.set_type(value);
    else if(param == "source")
      new_poly.set_source(value);
    else if(param == "vertex_color")
      new_poly.set_vertex_color(value);
    else if(param == "edge_color")
      new_poly.set_edge_color(value);
    else if((param == "vertex_size") && (dval >= 0))
      new_poly.set_vertex_size(dval);
    else if((param == "edge_size") && (dval >= 0))
      new_poly.set_edge_size(dval);
    else if(param == "active")
      new_poly.set_active(tolower(value) == "true");
    else
      return(null_poly);
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
  bool zval_set = false;
  bool axis_pad_set = false;
  bool perp_pad_set = false;  // Either degrees OR radians must

  double x1, y1, x2, y2, zval, axis_pad, perp_pad, snap=0;
  
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
      zval_set = true;
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
    else if(param == "label")
      new_poly.set_label(value);
    else if(param == "type")
      new_poly.set_type(value);
    else if(param == "source")
      new_poly.set_source(value);
    else if(param == "vertex_color")
      new_poly.set_vertex_color(value);
    else if(param == "edge_color")
      new_poly.set_edge_color(value);
    else if((param == "vertex_size") && (dval >= 0))
      new_poly.set_vertex_size(dval);
    else if((param == "edge_size") && (dval >= 0))
      new_poly.set_edge_size(dval);
    else if(param == "active")
      new_poly.set_active(tolower(value) == "true");
    else
      return(null_poly);
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
