/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Quadratic.cpp                                    */
/*    DATE: Jun 22nd, 2005 In Waterloo for vaca w/ Charlie       */
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
 
#include "AOF_Quadratic.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_Quadratic::AOF_Quadratic(const IvPDomain& gdomain) 
  : AOF(gdomain)
{
  //IvPBox new_universe(2,0);

  //new_universe.setPTS(0, 0, 400);
  //new_universe.setPTS(1, 0, 400);

  //universe = new_universe;

  //universe = new IvPBox(*ubox);
  int dim = getDim();

  vals = new double[dim*2+1];

  if(dim == 2) {
    vals[0] = -0.00495;
    vals[1] = -0.00495;
    vals[2] = 1.98;
    vals[3] = 1.98;
    vals[4] = 0;
  }
}

//----------------------------------------------------------
// Procedure: Destructor

AOF_Quadratic::~AOF_Quadratic()
{
  delete [] vals;
}

//----------------------------------------------------------
// Procedure: evalBox()

double AOF_Quadratic::evalBox(const IvPBox *b) const
{
  if(b->getDim() != 2)
    return(0);

  double x = (double)(b->pt(0,0));
  double y = (double)(b->pt(1,0));

  double z = (x*x*vals[0]) + (y*y*vals[1]) + 
    (x*vals[2]) + (y*vals[3]) + vals[4];

  return(z);
}

//----------------------------------------------------------
// Procedure: evalBox()

double AOF_Quadratic::evalBox(const int *b) const
{
  return(0);
}







