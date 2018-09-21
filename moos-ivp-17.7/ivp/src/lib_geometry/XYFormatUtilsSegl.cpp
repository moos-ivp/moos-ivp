/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYFormatUtilsSegl.cpp                                */
/*    DATE: May 18th, 2009 (Reworked from XYBuildUtils.h)        */
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
#include <cstring>
#include <vector>
#include "XYFormatUtilsSegl.h"
#include "XYPatternBlock.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2SegList
//
// Formats: default, zigzag
//
// Fields common to all:
//    label, source, active, msg, label_color, vertex_color
//    vertex_size
// 
// points = label=val, source=val, active=val, pts="0,0:200,-150:100,100"
// points = 0,0:200,-150:100,100:label,val:active,val:source,val

XYSegList string2SegList(string str)
{
  XYSegList null_segl;

  XYSegList new_segl = stringStandard2SegList(str);
  if(new_segl.valid())
    return(new_segl);
  
  // Perform some backward compatibility measures
  if(strContains(str, "format")) {
    string no_white_string = removeWhite(str);
    if(strContains(no_white_string, "format=zigzag"))
      new_segl = stringZigZag2SegList(str);
    else if(strContains(no_white_string, "format=lawnmower"))
      new_segl = stringLawnmower2SegList(str);
    else if(strContains(no_white_string, "format=bowtie"))
      new_segl = stringBowTie2SegList(str);

    if(new_segl.valid())
      return(new_segl);
  }

  if(strBegins(str, "zigzag:"))
    return(stringZigZag2SegList(str.substr(7)));
  else if(strBegins(str, "lawnmower:"))
    return(stringLawnmower2SegList(str.substr(10)));
  
  // Last chance...
  return(stringAbbreviated2SegList(str));
}

//---------------------------------------------------------------
// Procedure: stringStandard2SegList (Method #1)
// 
// points = label=val, source=val, active=val, pts={0,0:200,-150:100,100}
// points = pts={0,0:200,-150:100,100}, label=val, source=val, active=val

XYSegList stringStandard2SegList(string str)
{
  XYSegList null_segl;
  XYSegList new_seglist;

  string rest = str;

  while(rest != "") {
    string left = biteStringX(rest, '=');
    
    if(left == "pts") {
      string pstr = biteStringX(rest, '}');
      
      // Empty set of points is an error
      if(pstr == "")
	return(null_segl);

      // Points should begin with an open brace (but discard now)
      if(pstr[0] != '{') 
	return(null_segl);
      else
	pstr = pstr.substr(1);

      // If more components after pts={}, then it should begin w/ comma
      if(rest != "") {
	if(rest[0] != ',')
	  return(null_segl);
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
	  return(null_segl);
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
	new_seglist.add_vertex(xval, yval, zval, property);
      }
    }
    else {
      string right = biteStringX(rest, ',');
      new_seglist.set_param(left, right);
    }
  }	  				  

  return(new_seglist);
}

//---------------------------------------------------------------
// Procedure: stringAbbreviated2SegList  (Method #2)
//      Note: This format allows the user to specify a very simple
//            list of points as first example below. Supported partly
//            for convenience and partly for backward compatibility.
//  Examples: 10,15
//            label,foobar : 10,15 : 20,25 : 30,35

XYSegList stringAbbreviated2SegList(string str)
{
  XYSegList null_seglist;
  
  // Support some deprecated scenarios:
  if(strBegins(str, "pts:"))
    str = str.substr(4);
  if(strBegins(str, "points:"))
    str = str.substr(7);
  
  XYSegList new_seglist;
  
  vector<string> mvector = parseString(str, ':');
  unsigned int i, vsize = mvector.size();
  for(i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    string left = tolower(stripBlankEnds(biteString(mvector[i], ',')));
    string rest = stripBlankEnds(mvector[i]);
    
    bool handled = new_seglist.set_param(left, rest);
    if(!handled) {
      string xstr = left;
      string ystr = stripBlankEnds(biteString(rest, ','));
      string zstr = stripBlankEnds(rest);
      if((zstr != "") && !isNumber(zstr))
	return(null_seglist);
      if(!isNumber(xstr) || !isNumber(ystr))
	return(null_seglist);
      double xval = atof(xstr.c_str());
      double yval = atof(ystr.c_str());
      if(zstr == "")
	new_seglist.add_vertex(xval, yval);
      else {
	double zval = atof(zstr.c_str());
	new_seglist.add_vertex(xval, yval, zval);	
      }
    }
  }
  return(new_seglist);
}

//---------------------------------------------------------------
// Procedure: init_zigzag    (Method #3)                                    
//    Format: startx, starty, angle, length, period, amplitude
//   Example: 0, 0, 45, 100, 20, 50
//   Example: 0, 0, 45, 100, 20, 50, 1   (1 is the snapval)
//
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

XYSegList stringZigZag2SegList(string str)
{
  XYSegList null_seglist;
  
  // Support a deprecated style:
  if(strBegins(str, "zigzag:"))
    str = str.substr(7);
  
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
  double snapval = 0.0001;
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


//---------------------------------------------------------------
// Procedure: stringLawnmower2SegList (Method #4)
// Example: x=0, y=8, width=100, height=80, format=lawnmower
//          degs=45, swath=20, startx=-40, starty=80, start=tl
// Example: lawnmower: x=0, y=8, width=100, height=80, 
//          degs=45, swath=20, startx=-40, starty=80, start=tl

XYSegList stringLawnmower2SegList(string str)
{
  XYSegList null_seglist;

  // Below are the mandatory parameters - check they are set.
  bool xpos_set    = false;
  bool ypos_set    = false;
  bool width_set   = false;
  bool height_set  = false;
  bool swath_set   = false;

  string xpos, ypos, vertex_color, edge_color, label, label_color;
  string start, source, msg, edge_size, vertex_size, rows="ew";  
  string active;
  double height = 0;
  double width  = 0;
  double degs   = 0;
  double swath  = 0;
  double startx = 0;
  double starty = 0;
  double snapval = 0.00001;

  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();
  
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(mvector[i], '='));
    string value = mvector[i];
    double dval  = atof(value.c_str());
    if((param == "x") && isNumber(value)) {
      xpos = value;
      xpos_set = true;
    }
    else if((param == "y") && isNumber(value)) {
      ypos = value;
      ypos_set = true;
    }
    else if((param == "height") && isNumber(value)) {
      height = dval;
      height_set = true;
    }
    else if((param == "width") && isNumber(value)) {
      width = dval;
      width_set = true;
    }
    else if((param == "degs") && isNumber(value))
      degs = dval;
    else if(((param == "swath") || (param == "lane_width")) 
	    && isNumber(value)) {
      swath = dval;
      swath_set = true;
    }
    else if((param == "startx") && isNumber(value))
      startx = dval;
    else if((param == "starty") && isNumber(value)) 
      starty = dval;
    else if((param == "snap") && isNumber(value) && (dval >= 0))
      snapval = dval;
    else if(param == "active") 
      active = value;
    else if(param == "rows")
      rows = tolower(value);
    else if(param == "label")
      label = value;
    else if(param == "msg")
      msg = value;
    else if(param == "edge_color")
      edge_color = value;
    else if(param == "vertex_color")
      vertex_color = value;
    else if(param == "source")
      source = value;
    else if(param == "edge_size")
      edge_size = value;
    else if(param == "vertex_size")
      vertex_size = value;
  }

  if(!xpos_set || !ypos_set || !height_set || !width_set || !swath_set)
    return(null_seglist);

  if(rows == "east-west")
    rows = "ew";
  else if(rows == "north-south")
    rows = "ns";

  if(rows == "east-west")
    rows = "ew";
  else if(rows == "north-south")
    rows = "ns";

  if((rows != "ew") && (rows != "ns"))
    rows = "ew";

  if(rows == "ew") {
    double tmp = width;
    width = height;
    height = tmp;
    degs += 90;
  }

  XYPatternBlock pblock;
  pblock.setParam("id_point", (xpos + "," + ypos));
  pblock.setParam("block_width", width);
  pblock.setParam("block_length", height);
  pblock.setParam("swath_width", swath);
  pblock.setParam("angle", degs);

  pblock.buildCompositeSegList(startx, starty);
  
  XYSegList new_seglist = pblock.getCompositeSegList();

  if(label != "")
    new_seglist.set_label(label);
  if(msg != "")
    new_seglist.set_msg(msg);
  if(label_color != "")
    new_seglist.set_color("label", label_color);
  if(vertex_color != "")
    new_seglist.set_color("vertex", vertex_color);
  if(edge_color != "")
    new_seglist.set_color("edge", edge_color);
  if(isNumber(edge_size)) {
    double dval = atof(edge_size.c_str());
    if(dval >= 0)
      new_seglist.set_edge_size(dval);
  }
  if(isBoolean(active))
    new_seglist.set_active(tolower(active) == "true");
  if(isNumber(vertex_size)) {
    double dval = atof(vertex_size.c_str());
    if(dval >= 0)
      new_seglist.set_vertex_size(dval);
  }
  if(snapval > 0)
    new_seglist.apply_snap(snapval);
  return(new_seglist);
}


//---------------------------------------------------------------
// Procedure: stringBowTie2SegList (Method #5)
// Example: "format=bowtie, x=0, y=8, height=100, wid1=10, wid2=25, 
//          wid3=30, startx=-40, starty=80"

XYSegList stringBowTie2SegList(string str)
{
  XYSegList null_seglist;
  XYSegList new_seglist;

  // Below are the mandatory parameters - check they are set.
  bool xpos_set   = false;
  bool ypos_set   = false;
  bool wid1_set   = false;
  bool wid2_set   = false;
  bool wid3_set   = false;
  bool height_set = false;
  bool startx_set = false;
  bool starty_set = false;

  double xpos   = 0;
  double ypos   = 0;
  double height = 0;
  double wid1   = 0;
  double wid2   = 0;
  double wid3   = 0;
  double startx = 0;
  double starty = 0;

  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();
  
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(mvector[i], '='));
    string value = mvector[i];
    double dval  = atof(value.c_str());
    if((param == "x") && isNumber(value)) {
      xpos = dval;
      xpos_set = true;
    }
    else if((param == "y") && isNumber(value)) {
      ypos = dval;
      ypos_set = true;
    }
    else if((param == "height") && isNumber(value)) {
      height = dval;
      height_set = true;
    }
    else if((param == "wid1") && isNumber(value)) {
      wid1 = dval;
      wid1_set = true;
    }
    else if((param == "wid2") && isNumber(value)) {
      wid2 = dval;
      wid2_set = true;
    }
    else if((param == "wid3") && isNumber(value)) {
      wid3 = dval;
      wid3_set = true;
    }
    else if((param == "startx") && isNumber(value)) {
      startx = dval;
      startx_set = true;
    }
    else if((param == "starty") && isNumber(value)) {
      starty = dval;
      starty_set = true;
    }
    else
      new_seglist.set_param(param, value);
  }

  if(!xpos_set || !ypos_set || !height_set || !wid1_set || !wid2_set || !wid3_set)
    return(null_seglist);

  //double x0 = xpos;
  //double y0 = ypos;

  double x1 = xpos - wid2;
  double y1 = ypos + (height/2);

  double x2 = xpos - wid1; 
  double y2 = ypos + (height/2);

  double x3 = xpos + wid1;
  double y3 = ypos + (height/2);

  double x4 = xpos + wid2;
  double y4 = ypos + (height/2);

  double x5 = xpos - wid3;
  double y5 = ypos;

  double x6 = xpos + wid3;
  double y6 = ypos;

  double x7 = xpos - wid2;
  double y7 = ypos - (height/2);

  double x8 = xpos - wid1; 
  double y8 = ypos - (height/2);

  double x9 = xpos + wid1;
  double y9 = ypos - (height/2);

  double x10 = xpos + wid2;
  double y10 = ypos - (height/2);

  string acase = "ne";
  if(startx_set && starty_set) {
    double rang = relAng(xpos, ypos, startx, starty);
    if((rang >= 0) && (rang < 90))
      acase = "ne";
    else if((rang >= 90) && (rang < 180))
      acase = "se";
    else if((rang >= 180) && (rang < 270))
      acase = "sw";
    else 
      acase = "nw";
  }

  //new_seglist.add_vertex(x0,  y0);

  if(acase == "nw") {
    new_seglist.add_vertex(x9,  y9);
    new_seglist.add_vertex(x10, y10);
    new_seglist.add_vertex(x6,  y6);
    new_seglist.add_vertex(x4,  y4);
    new_seglist.add_vertex(x3,  y3);
    new_seglist.add_vertex(x8,  y8);
    new_seglist.add_vertex(x7,  y7);
    new_seglist.add_vertex(x5,  y5);
    new_seglist.add_vertex(x1,  y1);
    new_seglist.add_vertex(x2,  y2);
  }
  else if(acase == "ne") {
    new_seglist.add_vertex(x8,  y8);
    new_seglist.add_vertex(x7,  y7);
    new_seglist.add_vertex(x5,  y5);
    new_seglist.add_vertex(x1,  y1);
    new_seglist.add_vertex(x2,  y2);
    new_seglist.add_vertex(x9,  y9);
    new_seglist.add_vertex(x10, y10);
    new_seglist.add_vertex(x6,  y6);
    new_seglist.add_vertex(x4,  y4);
    new_seglist.add_vertex(x3,  y3);
  }
  else if(acase == "se") {
    new_seglist.add_vertex(x2,  y2);
    new_seglist.add_vertex(x1,  y1);
    new_seglist.add_vertex(x5,  y5);
    new_seglist.add_vertex(x7,  y7);
    new_seglist.add_vertex(x8,  y8);
    new_seglist.add_vertex(x3,  y3);
    new_seglist.add_vertex(x4,  y4);
    new_seglist.add_vertex(x6,  y6);
    new_seglist.add_vertex(x10, y10);
    new_seglist.add_vertex(x9,  y9);
  }
  else if(acase == "sw") {
    new_seglist.add_vertex(x3,  y3);
    new_seglist.add_vertex(x4,  y4);
    new_seglist.add_vertex(x6,  y6);
    new_seglist.add_vertex(x10, y10);
    new_seglist.add_vertex(x9,  y9);
    new_seglist.add_vertex(x2,  y2);
    new_seglist.add_vertex(x1,  y1);
    new_seglist.add_vertex(x5,  y5);
    new_seglist.add_vertex(x7,  y7);
    new_seglist.add_vertex(x8,  y8);
  }


  //new_seglist.add_vertex(x0,  y0);

  return(new_seglist);
}








