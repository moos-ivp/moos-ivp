/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IO_GeomUtils.h                                       */
/*    DATE: Sep 4th, 2005                                        */
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
#include "IO_GeomUtils.h"
#include "XYEncoders.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//--------------------------------------------------------
// Procedure: readPolysFromFile

vector<XYPolygon> readPolysFromFile(const string& filestr)
{
  vector<XYPolygon> poly_vector;

  vector<string> file_vector = fileBuffer(filestr);
  int lineCount = file_vector.size();
  
  for(int i=0; i < lineCount; i++) {
    string line = stripBlankEnds(file_vector[i]);
    
    if((line.length()!=0) && ((line)[0]!='#')) {
      vector<string> svector = parseString(line, '=');
      if(svector.size() == 2) {
	string left = stripBlankEnds(svector[0]);
	if((left == "polygon") || 
	   (left == "poly")  ||
	   (left == "ellipse")  ||
	   (left == "points")  ||
	   (left == "radial")) {
	  string right = stripBlankEnds(svector[1]);
	  XYPolygon poly;
	  bool res;
	  if(left=="radial")
	    res = poly.initialize("radial:"+right);
	  else
	    res = poly.initialize(right);
	  poly_vector.push_back(poly);
	}
      }
    }
  }
  return(poly_vector);
}


//--------------------------------------------------------
// Procedure: readGridsFromFile

vector<XYGrid> readGridsFromFile(const string& filestr)
{
  vector<XYGrid> grid_vector;

  vector<string> file_vector = fileBuffer(filestr);
  int lineCount = file_vector.size();
  
  for(int i=0; i < lineCount; i++) {
    string line = stripBlankEnds(file_vector[i]);
    
    if((line.length()!=0) && ((line)[0]!='#')) {
      vector<string> svector = chompString(line, '=');
      if(svector.size() == 2) {
	string left = stripBlankEnds(svector[0]);
	if((left == "searchgrid") || (left == "sgrid")) {
	  string right = stripBlankEnds(svector[1]);
	  XYGrid sgrid;
	  bool res = sgrid.initialize(right);
	  grid_vector.push_back(sgrid);
	}
	if((left == "fullgrid") || (left == "fgrid")) {
	  string right = stripBlankEnds(svector[1]);
	  XYGrid sgrid = StringToXYGrid(right);
	  grid_vector.push_back(sgrid);
	}
      }
    }
  }
  return(grid_vector);
}


//--------------------------------------------------------
// Procedure: readCirclesFromFile

vector<XYCircle> readCirclesFromFile(const string& filestr)
{
  vector<XYCircle> circle_vector;

  vector<string> file_vector = fileBuffer(filestr);
  int lineCount = file_vector.size();
  
  for(int i=0; i < lineCount; i++) {
    string line = stripBlankEnds(file_vector[i]);
    
    if((line.length()!=0) && ((line)[0]!='#')) {
      vector<string> svector = chompString(line, '=');
      if(svector.size() == 2) {
	string left = stripBlankEnds(svector[0]);
	if(left == "circle") {
	  string right = stripBlankEnds(svector[1]);
	  XYCircle new_circle;
	  bool res = new_circle.initialize(right);
	  circle_vector.push_back(new_circle);
	}
      }
    }
  }
  return(circle_vector);
}


//--------------------------------------------------------
// Procedure: readArcsFromFile

vector<XYArc> readArcsFromFile(const string& filestr)
{
  vector<XYArc> arc_vector;

  vector<string> file_vector = fileBuffer(filestr);
  int lineCount = file_vector.size();
  
  for(int i=0; i < lineCount; i++) {
    string line = stripBlankEnds(file_vector[i]);
    
    if((line.length()!=0) && ((line)[0]!='#')) {
      vector<string> svector = chompString(line, '=');
      if(svector.size() == 2) {
	string left = stripBlankEnds(svector[0]);
	if(left == "arc") {
	  string right = stripBlankEnds(svector[1]);
	  XYArc new_arc;
	  bool ok = new_arc.initialize(right);
	  if(ok)
	    arc_vector.push_back(new_arc);
	}
      }
    }
  }
  return(arc_vector);
}


//--------------------------------------------------------
// Procedure: readHexagonFromFile

vector<XYHexagon> readHexagonsFromFile(const string& filestr)
{
  vector<XYHexagon> hexa_vector;

  vector<string> file_vector = fileBuffer(filestr);
  int lineCount = file_vector.size();
  
  for(int i=0; i < lineCount; i++) {
    string line = stripBlankEnds(file_vector[i]);
    
    if((line.length()!=0) && ((line)[0]!='#')) {
      vector<string> svector = chompString(line, '=');
      if(svector.size() == 2) {
	string left = stripBlankEnds(svector[0]);
	if(left == "hexagon") {
	  string right = stripBlankEnds(svector[1]);
	  XYHexagon new_hexagon;
	  bool ok = new_hexagon.initialize(right);
	  if(ok)
	    hexa_vector.push_back(new_hexagon);
	}
      }
    }
  }
  return(hexa_vector);
}


//--------------------------------------------------------
// Procedure: printSquare

void printSquare(const XYSquare& square)
{
  cout << "xl:" << square.getVal(0,0) << " ";
  cout << "xh:" << square.getVal(0,1) << " ";
  cout << "yl:" << square.getVal(1,0) << " ";
  cout << "yh:" << square.getVal(1,1) << endl;
}




