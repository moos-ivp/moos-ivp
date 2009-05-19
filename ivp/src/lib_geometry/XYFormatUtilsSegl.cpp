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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "XYFormatUtilsSegl.h"
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
//    label, source, type, active, 
// 
// points = label=val, source=val, active=val, pts="0,0:200,-150:100,100"
// points = 0,0:200,-150:100,100:label,val:active,val:source,val

XYSegList string2SegList(string str)
{
  str = stripBlankEnds(str);
  
  XYPolygon null_seglist;
  if(!strncasecmp("zigzag:", str.c_str(), 7)) {
    str = str.c_str()+7;
    return(stringZigZag2SegList(str));
  }
  else if(!strncasecmp("pts:", str.c_str(), 5)) {
    str = str.c_str()+5;
    return(stringPoints2SegList(str));
  }
  else
    return(stringPoints2SegList(str));

  return(null_seglist);
}

//---------------------------------------------------------------
// Procedure: stringPoints2SegList
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
    if((svector.size() < 2) || (svector.size() > 3))
      return(null_seglist);
    string xstr = stripBlankEnds(svector[0]);
    string ystr = stripBlankEnds(svector[1]);
    string zstr = "";
    if(svector.size()==3)
      zstr = stripBlankEnds(svector[2]);
    
    if((zstr != "") && (!isNumber(zstr)))
      return(null_seglist);
    
    if((!isNumber(xstr)) || (!isNumber(ystr))) {
      xstr = tolower(xstr);
      if(xstr == "label") 
	new_seglist.set_label(ystr);
      else if(xstr == "active") 
	new_seglist.set_active(tolower(ystr)=="true");
      else
	return(null_seglist);
    }
    else {
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
