/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYEncoders.cpp                                       */
/*    DATE: Nov 24th 2006 (Thanksgiving Waterloo)                */
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
#include <stdio.h>
#include <stdlib.h>
#include "MBUtils.h"
#include "XYEncoders.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: XYPolygonToString

string XYPolygonToString(const XYPolygon& g_poly, int digits)
{
  string str;
  int size = g_poly.size();
  
  string label = g_poly.get_label();
  if(label != "")
    str += ("label," + label + ":");

  for(int i=0; i<size; i++) {
    str += dstringCompact(doubleToString(g_poly.get_vx(i),digits)) + ","; 
    str += dstringCompact(doubleToString(g_poly.get_vy(i),digits));
    if(i < size-1)
      str += ":";
  }
  return(str);
}


//--------------------------------------------------------------
// Procedure: XYSegListToString

string XYSegListToString(const XYSegList& g_seglist)
{
  string str;
  int size = g_seglist.size();

  for(int i=0; i<size; i++) {
    str += dstringCompact(doubleToString(g_seglist.get_vx(i))) + ","; 
    str += dstringCompact(doubleToString(g_seglist.get_vy(i)));
    if(i < size-1)
      str += ":";
  }
  return(str);
}


//--------------------------------------------------------------
// Procedure: XYSquareToString

string XYSquareToString(const XYSquare& g_square)
{
  string str;

  str += dstringCompact(doubleToString(g_square.getVal(0,0))) + ","; 
  str += dstringCompact(doubleToString(g_square.getVal(0,1))) + ","; 
  str += dstringCompact(doubleToString(g_square.getVal(1,0))) + ","; 
  str += dstringCompact(doubleToString(g_square.getVal(1,1))); 

  return(str);
}


//--------------------------------------------------------------
// Procedure: XYGridToString

string XYGridToString(const XYGrid& g_grid)
{
  string str;
  int gsize = g_grid.size();

  str += "label="   + g_grid.getLabel()  + "#";
  str += "size="    + dstringCompact(doubleToString(gsize)) + "#";
  str += "min_val=" + dstringCompact(doubleToString(g_grid.getMinVal())) + "#";
  str += "max_val=" + dstringCompact(doubleToString(g_grid.getMaxVal())) + "#";
 
  if(gsize <= 0)
    return(str);

  XYSquare sbound = g_grid.getSBound();
  str += "sbound=" + XYSquareToString(sbound) + "#";
  
  XYPolygon pbound = g_grid.getPBound();
  str += "pbound=" + XYPolygonToString(pbound) + "#";
  
  str += "squares=";
  for(int i=0; i<gsize; i++) {
    str += XYSquareToString(g_grid.getElement(i));
    str += "," + dstringCompact(doubleToString(g_grid.getVal(i)));
    if(i < gsize-1)
      str += "@";
  }
  return(str);
}


//--------------------------------------------------------------
// Procedure: StringToXYGrid
//   Purpose: 

XYGrid StringToXYGrid(const string& str)
{
  XYGrid bad_grid;
  XYGrid new_grid;

  vector<string> svector = parseString(str, '#');
  int i, vsize = svector.size();

  string label, size, min_val, max_val, sbound, pbound, squares;

  for(i=0; i<vsize; i++) {
    vector<string> evector = parseString(svector[i], '=');
    if(evector.size() != 2) {
      cout << "Failed StringToXYGrid 1" << endl;
      return(bad_grid);
    }
    if(evector[0] == "label")
      label = evector[1];
    else if(evector[0] == "size")
      size  = evector[1];
    else if(evector[0] == "min_val")
      min_val  = evector[1];
    else if(evector[0] == "max_val")
      max_val  = evector[1];
    else if(evector[0] == "sbound")
      sbound  = evector[1];
    else if(evector[0] == "pbound")
      pbound  = evector[1];
    else if(evector[0] == "squares")
      squares  = evector[1];
  }

  if((label == "") || (size == "") || (min_val == "") || (max_val == "") ||
     (sbound == "") || (pbound == "") || (squares == "")) {
    cout << "Failed StringToXYGrid 2" << endl;
    return(bad_grid);
  }

  vector<XYSquare> vsquares;
  vector<double>   vdoubles;

  vector<string> qvector = parseString(squares, '@');
  int qsize = qvector.size();
  
  if(qsize == 0) {
    cout << "Failed StringToXYGrid 3" << endl;
    return(bad_grid);
  }

  for(i=0; i<qsize; i++) {
    vector<string> evector = parseString(qvector[i], ',');
    if(evector.size() != 5) {
      cout << "Failed StringToXYGrid 4" << endl;
      return(bad_grid);
    }
    double ll  = atof(evector[0].c_str());
    double lh  = atof(evector[1].c_str());
    double hl  = atof(evector[2].c_str());
    double hh  = atof(evector[3].c_str());
    double val = atof(evector[4].c_str());
    XYSquare new_square(ll,lh,hl,hh);
    vsquares.push_back(new_square);
    vdoubles.push_back(val);
  }
  double x_length = vsquares[0].getLengthX();
  double y_length = vsquares[0].getLengthY();

  // Grid init string:
  // "poly=label:foo,0,10:20,30  @ 20,20  @  100"
  //    bounding poly             unitsize   initvalue

  string grid_init_str = "label," + label + ":" + pbound + "@";
  grid_init_str += dstringCompact(doubleToString(x_length)) + ",";
  grid_init_str += dstringCompact(doubleToString(y_length));

  bool ok = new_grid.initialize(grid_init_str);
  if(!ok) {
    cout << "Failed StringToXYGrid 4.5" << endl;
    cout << "grid_init_str: " << grid_init_str << endl;
    return(bad_grid);
  }

  if(qsize != new_grid.size()) {
    cout << "Failed StringToXYGrid 5" << endl;
    return(bad_grid);
  }

  for(i=0; i<qsize; i++)
    new_grid.setVal(i, vdoubles[i]); 

  return(new_grid);
}









