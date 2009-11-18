/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYFormatUtilsSegl.cpp                                */
/*    DATE: May 18th, 2009 (Reworked from XYBuildUtils.h)        */
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
#include "XYFormatUtilsSegl.h"
#include "XYPatternBlock.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

#ifdef WIN32
  #define strncasecmp strnicmp
#endif

using namespace std;

//---------------------------------------------------------------
// Procedure: string2SegList
//
// Formats: default, zigzag
//
// Fields common to all:
//    label, source, type, active, 
// 
// points = label=val, source=val, active=val, pts="0,0:200,-150:100,100"
// points = 0,0:200,-150:100,100:label,val:active,val:source,val

XYSegList string2SegList(string str)
{
  XYSegList null_segl;

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
  

  if(!strncasecmp("zigzag:", str.c_str(), 7))
    format = "zigzag";


  if((format == "default") || (format == "points"))
    return(stringPoints2SegList(str));
  else if(format == "zigzag")
    return(stringZigZag2SegList(str));
  else if(format == "lawnmower")
    return(stringLawnmower2SegList(str));
  else
    return(null_segl);

  return(null_segl);
}

//---------------------------------------------------------------
// Procedure: stringPoints2SegList
//

XYSegList stringPoints2SegList(string str)
{
  XYSegList null_seglist;
  
  if(!strncasecmp("pts:", str.c_str(), 4))
    str = str.c_str()+4;
  else if(!strncasecmp("points:", str.c_str(), 7))
    str = str.c_str()+7;
  
  XYSegList new_seglist;
  
  vector<string> mvector = parseString(str, ':');
  unsigned int i, vsize = mvector.size();
  for(i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    string left = tolower(stripBlankEnds(biteString(mvector[i], ',')));
    string rest = stripBlankEnds(mvector[i]);
    double dval = atof(rest.c_str());
    
    if(left == "label") 
      new_seglist.set_label(rest);
    else if(left == "label_color") 
      new_seglist.set_label_color(rest);
    else if(left == "vertex_color") 
      new_seglist.set_vertex_color(rest);
    else if((left == "vertex_size") && isNumber(rest))
      new_seglist.set_vertex_size(dval);
    else if(left == "edge_color")
      new_seglist.set_edge_color(rest);
    else if((left == "edge_size") && isNumber(rest))
      new_seglist.set_edge_size(dval);
    else if(left == "active") 
      new_seglist.set_active(tolower(rest)=="true");
    else {
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

XYSegList stringZigZag2SegList(string str)
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
// Procedure: stringLawnmower2SegList

XYSegList stringLawnmower2SegList(string str)
{
  XYSegList null_seglist;

  // Below are the mandatory parameters - check they are set.
  bool xpos_set    = false;
  bool ypos_set    = false;
  bool width_set   = false;
  bool height_set  = false;
  bool swath_set   = false;
  bool startx_set  = false;
  bool starty_set  = false;
  bool degs_set    = false;
  bool rads_set    = false;

  bool active      = true;

  string xpos, ypos, vertex_color, edge_color, label, label_color;
  string start, source, type, rows="ew";
  double height, width, degs, rads, swath;
  double startx=0;
  double starty=0;
  double snapval = 0.00001;

  vector<string> mvector = parseStringQ(str, ',');
  unsigned int i, vsize = mvector.size();
  
  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(mvector[i], '=')));
    string value = stripBlankEnds(mvector[i]);
    double dval  = atof(value.c_str());
    if((param == "format") && (value != "lawnmower"))
      return(null_seglist);
    else if((param == "x") && isNumber(value)) {
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
    else if((param == "degs") && isNumber(value)) {
      degs = dval;
      degs_set = true;
    }
    else if((param == "rads") && isNumber(value)) {
      rads = dval;
      rads_set = true;
    }
    else if(((param == "swath") || (param == "lane_width")) 
	    && isNumber(value)) {
      swath = dval;
      swath_set = true;
    }
    else if((param == "startx") && isNumber(value)) {
      startx = dval;
      startx_set = true;
    }
    else if((param == "starty") && isNumber(value)) {
      starty = dval;
      starty_set = true;
    }
    else if((param == "snap") && isNumber(value) && (dval >= 0))
      snapval = dval;
    else if(param == "rows")
      rows = tolower(value);
    else if(param == "label")
      label = value;
    else if(param == "edge_color")
      edge_color = value;
    else if(param == "vertex_color")
      vertex_color = value;
    else if(param == "type")
      type = value;
    else if(param == "source")
      source = value;
  }

  if(!xpos_set || !ypos_set || !height_set || !width_set || !swath_set)
    return(null_seglist);

  if(!rads_set && !degs_set)
    return(null_seglist);

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
  if(label_color != "")
    new_seglist.set_label_color(label_color);
  if(vertex_color != "")
    new_seglist.set_vertex_color(vertex_color);
  if(edge_color != "")
    new_seglist.set_edge_color(edge_color);
  if(type != "")
    new_seglist.set_type(type);
  if(source != "")
    new_seglist.set_source(type);

  if(snapval > 0)
    new_seglist.apply_snap(snapval);
  return(new_seglist);
}
