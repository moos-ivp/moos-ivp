/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYHexGrid.cpp                                        */
/*    DATE: Oct 30th 2005                                        */
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
#include "XYHexGrid.h"
#include "MBUtils.h"
#include "XYBuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: getElement

XYHexagon XYHexGrid::getElement(unsigned int ix) const
{
  XYHexagon retElement;

  if((ix >= 0) && (ix < elements.size()))
    return(elements[ix]);
  else
    return(retElement);
}

//-------------------------------------------------------------
// Procedure: initialize
//      Note: An alternative way of initializing convenient
//            for reading a specification from a file.

bool XYHexGrid::initialize(string given_config_str)
{
  // If initialization fails, elements vector will be empty
  this->clear();

  vector<string> svector = parseString(given_config_str, '@');
  int vsize = svector.size();
  if((vsize != 2) && (vsize != 3))
    return(false);
  
  string poly_string = svector[0];
  string unit_string = svector[1];
  double initial_val = 0.0;

  if(vsize == 3)
    initial_val = atof(svector[2].c_str());

  XYPolygon poly = stringToPoly(poly_string);
  if(poly.size() == 0)
    return(false);
  
  // Label of poly will also be label of this searchgrid.
  string poly_label = poly.get_label();
  if(poly_label == "") {
    cout << "Un-labeled searchgrid specification" << endl;
    return(false);
  }

  unit_string = stripBlankEnds(unit_string);
  svector = parseString(unit_string, ',');
  vsize = svector.size();
  if(vsize != 2)
    return(false);

  double xl = 0;
  double xh = atof(svector[0].c_str());
  double yl = 0;
  double yh = atof(svector[1].c_str());
  XYSquare unit_square(xl,xh,yl,yh);
  // Check for the validity of the square
  if(!unit_square.isValid())
    return(false);

  bool ok = initialize(poly, unit_square);

  if(ok) {
    config_string = given_config_str;
    label = poly_label;
  }

  return(ok);
}

//-------------------------------------------------------------
// Procedure: initialize

bool XYHexGrid::initialize(XYPolygon poly,
			   const XYSquare&  unit_square)
{
  return(true);
}









