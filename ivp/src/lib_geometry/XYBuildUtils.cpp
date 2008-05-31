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
#include <stdlib.h>
#include <string.h>
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
  else if((!strncasecmp("pts:", str.c_str(), 4)) ||
	  (!strncasecmp("points:", str.c_str(), 7)) ||
	  (!strncasecmp("poly:", str.c_str(), 5)) ||
	  (!strncasecmp("polygon:", str.c_str(), 5))) {
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
// Procedure: stringPairsToPylonPoly
//
//                     o (x2,y2) 
//                   /   
//                 /     
//      (x1,y1)  o    
//
/// Initializes a polygon (rectangle) based on two points
/// The format of the string is "x1=val, y1=val, x2=val, y2=val, 
//           axis_pad=val, perp_pad=val".


XYPolygon stringPairsToPylonPoly(string str)
{
  XYPolygon null_poly;

  str = tolower(stripBlankEnds(str));
  vector<string> mvector = parseString(str, ',');
  int vsize = mvector.size();

  // Below are the mandatory parameters - check they are set.
  bool x1_set  = false;
  bool y1_set  = false;
  bool x2_set  = false;
  bool y2_set  = false;
  bool axis_pad_set = false;
  bool perp_pad_set = false;  // Either degrees OR radians must

  double x1, y1, x2, y2, axis_pad, perp_pad, snap=0;
  string label;
  
  for(int i=0; i<vsize; i++) {
    vector<string> svector = parseString(mvector[i], '=');
    if(svector.size() != 2)
      return(null_poly);
    string param = stripBlankEnds(svector[0]);
    string value = stripBlankEnds(svector[1]);
    double dval  = atof(value.c_str());
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
    else if((param == "snap") && (isNumber(value))) {
      if(dval >= 0)
	snap = dval;
    }
    else if(param == "label") {
      label = value;
    }
  }

  if(!x1_set || !y1_set || !x2_set || !y2_set)
    return(null_poly);

  if(!axis_pad_set || !perp_pad_set)
    return(null_poly);

  XYPolygon new_poly;

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


  // Now add the four points. The "false" in the first three
  // calls indicates that no determination of convexity is made.
  // That is saved only for the last call - minor time savings.
  new_poly.add_vertex(px1,py1,false);
  new_poly.add_vertex(px2,py2,false);
  new_poly.add_vertex(px3,py3,false);
  new_poly.add_vertex(px4,py4,true);

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
// Examples: "radial: px, py, prad, ppts, snapval, label"
//           "px, py, prad, ppts, snapval, label"

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

//===============================================================
//                                            XYSegList Functions
//===============================================================


//---------------------------------------------------------------
// Procedure: stringToSegList
//

XYSegList stringToSegList(string str)
{
  str = stripBlankEnds(str);
  
  XYPolygon null_seglist;
  if(!strncasecmp("zigzag:", str.c_str(), 7)) {
    str = str.c_str()+7;
    return(stringShortToZigZagSegList(str));
  }
  else if(!strncasecmp("pts:", str.c_str(), 5)) {
    str = str.c_str()+5;
    return(stringShortToPointsSegList(str));
  }
  else
    return(stringShortToPointsSegList(str));

  return(null_seglist);
}

//---------------------------------------------------------------
// Procedure: stringShortToPointsSegList
//

XYSegList stringShortToPointsSegList(string str)
{
  XYSegList null_seglist;
  
  if(!strncasecmp("pts:", str.c_str(), 4))
    str = str.c_str()+4;
  else if(!strncasecmp("points:", str.c_str(), 7))
    str = str.c_str()+7;
  
  XYSegList new_seglist;
  
  vector<string> mvector = parseString(str, ':');
  int vsize = mvector.size();
  for(int i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    vector<string> svector = parseString(mvector[i], ',');
    if(svector.size() != 2)
      return(null_seglist);
    string xstr = stripBlankEnds(svector[0]);
    string ystr = stripBlankEnds(svector[1]);
    
    if((!isNumber(xstr)) || (!isNumber(ystr))) {
      xstr = tolower(xstr);
      if(xstr == "label") 
	new_seglist.set_label(ystr);
      else
	return(null_seglist);
    }
    else {
      double xval = atof(xstr.c_str());
      double yval = atof(ystr.c_str());
      new_seglist.add_vertex(xval, yval);
    }
  }
  return(new_seglist);
}



//---------------------------------------------------------------
// Procedure: init_zigzag                                             |
//                                                                    |
//         o                               o                          |
//       /   \                           /   \                        |
//     /       \                       /       \                      |
//   /           \                   /           \                    |
//  o--------------o---------------o---------------o--------------->  |
//                   \           /                   \           /    |
//                     \       /                       \       /      |
//                       \   /                           \   /        |
//                         o                               o          |
//  p1     p2              p3             p4               p5     p6  |  
//                                                                  

XYSegList stringShortToZigZagSegList(string str)
{
  XYSegList null_seglist;
  
  if(!strncasecmp("zigzag:", str.c_str(), 7))
    str = str.c_str()+7;
  
  vector<string> svector = parseString(str, ',');
  int vsize = svector.size();
  
  // Should have 6 fields, but optional 7th field, snapval is ok
  if((vsize < 6) || (vsize > 7))
    return(null_seglist);
  
  for(int i=0; i<vsize; i++) 
    if(!isNumber(svector[i]))
      return(null_seglist);

  double startx  = atof(svector[0].c_str());
  double starty  = atof(svector[1].c_str());
  double angle   = atof(svector[2].c_str());
  double length  = atof(svector[3].c_str());
  double period  = atof(svector[4].c_str());
  double amplit  = atof(svector[5].c_str());
  double snapval = 0;
  if(vsize == 7)
    snapval = atof(svector[6].c_str());
  
  // Check for whatever semantic errors we can
  if((period<=0) || (amplit<=0) || (length<=0) || (snapval<0))
    return(null_seglist);
  
  XYSegList new_seglist;
  
  new_seglist.add_vertex(startx, starty);
  
  double zigside = -90;
  double zigdist = period / 4;
  while(zigdist < length) {
    double axis_x, axis_y;
    projectPoint(angle, zigdist, startx, starty, axis_x, axis_y);
    double new_x, new_y;
    projectPoint((angle+zigside), amplit, axis_x, axis_y, new_x, new_y);
    new_seglist.add_vertex(new_x, new_y);
    zigside *= -1;
    zigdist += (period / 2);
  }
  
  // Now apply the snapval if a valid one was requested
  if(snapval > 0)
    new_seglist.apply_snap(snapval);

  return(new_seglist);
}


