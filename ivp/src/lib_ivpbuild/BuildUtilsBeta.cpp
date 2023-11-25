/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BuildUtilsBetat.cpp                                  */
/*    DATE: March 15th, 20023                                    */
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
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <list>
#include "BuildUtilsBeta.h"
#include "MBUtils.h"

using namespace std;

#define min(x, y) ((x)<(y)?(x):(y))
#define max(x, y) ((x)>(y)?(x):(y))

//-------------------------------------------------------------
// Procedure: build1DPointBox()

IvPBox build1DPointBox(IvPDomain domain, string domstr, double val)
{
  return(build1DBox(domain, domstr, val, val));
}

//-------------------------------------------------------------
// Procedure: build1DBox()

IvPBox build1DBox(IvPDomain domain, string domstr,
		  double vmin, double vmax)
{
  // Part 1: Sanity Checks
  IvPBox nullbox;
  if(vmin > vmax)
    return(nullbox);
  
  int dom_ix = domain.getIndex(domstr); 
  if(dom_ix < 0)
    return(nullbox);
  
  unsigned int dom_pts = domain.getVarPoints((unsigned int)(dom_ix));
  if(dom_pts == 0)
    return(nullbox);

  // ============================================================
  // Part 2: Case (vmin == vmax)
  //
  // a pointbox is always returned, even if the value is between 
  // two domain points, we'll choose the closest
  if(vmin == vmax) {
    unsigned int disc_val = domain.getDiscreteVal(dom_ix, vmin, 2);
    IvPBox newbox(1);
    newbox.setPTS(0, disc_val, disc_val);  
    return(newbox);
  }
  
  // ============================================================
  // Part 3 Case (vmin < vmax)
  //
  // Mappings from native to discrete values is done conservatively.
  // If the domain is:
  //     [0]   [1]   [2]   [3]   [4]   [5]   [6]   [7]   [8]   [9]
  //     0.0   0.1   0.2   0.3   0.4   0.5   0.6   0.7   0.8   0.9
  // And vmin = 0.23, vmax = 0.74
  // Then the indices will be [3] and [7] respectively

  unsigned int disc_val_low = domain.getDiscreteVal(dom_ix, vmin, 1);
  unsigned int disc_val_hgh = domain.getDiscreteVal(dom_ix, vmax, 0);

  // Handle edge case where both native values fall between the same
  // pair of discrete indices:
  if(disc_val_low > disc_val_hgh) {
    double avg = (vmin + vmax)/2;
    disc_val_low = domain.getDiscreteVal(dom_ix, avg, 2);
    disc_val_hgh = disc_val_low;
  }
  
  IvPBox newbox(1);
  newbox.setPTS(0, disc_val_low, disc_val_hgh);  
  return(newbox);
}
  


//-------------------------------------------------------------
// Procedure: build2DPointBox()

IvPBox build2DBox(IvPDomain domain,
		  string domstr1, string domstr2,
		  double val1, double val2)
{
  return(build2DBox(domain, domstr1, domstr2,
		    val1, val1, val2, val2));
}

//-------------------------------------------------------------
// Procedure: build2DBox()

IvPBox build2DBox(IvPDomain domain,
		  string domstr1, string domstr2,
		  double vmin1, double vmax1,
		  double vmin2, double vmax2)
{
  // Part 1: Sanity Checks
  IvPBox nullbox;
  if((vmin1 > vmax1) || (vmin2 > vmax2))
    return(nullbox);
  
  if(domstr1 == domstr2)
    return(nullbox);
  
  int dom1_ix = domain.getIndex(domstr1); 
  int dom2_ix = domain.getIndex(domstr2); 
  if((dom1_ix < 0) || (dom2_ix < 0))
    return(nullbox);
  
  unsigned int dom1_pts = domain.getVarPoints((unsigned int)(dom1_ix));
  unsigned int dom2_pts = domain.getVarPoints((unsigned int)(dom2_ix));
  if((dom1_pts == 0) || (dom2_pts == 0))
    return(nullbox);
  
  unsigned int disc1_val_low, disc1_val_hgh;
  unsigned int disc2_val_low, disc2_val_hgh;
     
  // ============================================================
  // Part 2: Handle domain1
  if(vmin1 == vmax1) {
    disc1_val_low = domain.getDiscreteVal(dom1_ix, vmin1, 2);
    disc1_val_hgh = disc1_val_low;
  }
  else {
    disc1_val_low = domain.getDiscreteVal(dom1_ix, vmin1, 1);
    disc1_val_hgh = domain.getDiscreteVal(dom1_ix, vmax1, 0);
    // Handle edge case where both native values fall between the same
    // pair of discrete indices:
    if(disc1_val_low > disc1_val_hgh) {
      double avg = (vmin1 + vmax1)/2;
      disc1_val_low = domain.getDiscreteVal(dom1_ix, avg, 2);
      disc1_val_hgh = disc1_val_low;
    }
  }

  // ============================================================
  // Part 3: Handle domain2
  if(vmin2 == vmax2) {
    disc2_val_low = domain.getDiscreteVal(dom2_ix, vmin2, 2);
    disc2_val_hgh = disc2_val_low;
  }
  else {
    disc2_val_low = domain.getDiscreteVal(dom2_ix, vmin2, 1);
    disc2_val_hgh = domain.getDiscreteVal(dom2_ix, vmax2, 0);
    // Handle edge case where both native values fall between the same
    // pair of discrete indices:
    if(disc2_val_low > disc2_val_hgh) {
      double avg = (vmin2 + vmax2)/2;
      disc2_val_low = domain.getDiscreteVal(dom2_ix, avg, 2);
      disc2_val_hgh = disc2_val_low;
    }
  }
  
  IvPBox newbox(2);
  if(dom1_ix < dom2_ix) {
    newbox.setPTS(0, disc1_val_low, disc1_val_hgh);  
    newbox.setPTS(1, disc2_val_low, disc2_val_hgh);  
  }
  else { 
    newbox.setPTS(0, disc2_val_low, disc2_val_hgh);  
    newbox.setPTS(1, disc1_val_low, disc1_val_hgh);  
  }

  return(newbox);
}
 

