/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PDMapBuilder.cpp                                     */
/*    DATE: May 25th, 2016                                       */
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
#include "PDMapBuilder.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: setIvPDomain

void PDMapBuilder::setIvPDomain(IvPDomain ivp_domain)
{
  m_ivp_domain = ivp_domain;
}

//-------------------------------------------------------------
// Procedure: setDomainVals()
//   Purpose: Set the vector of domain values.

void PDMapBuilder::setDomainVals(vector<unsigned int> vals)
{
  m_dom_vals = vals;
}

//-------------------------------------------------------------
// Procedure: setRangeVals

void PDMapBuilder::setRangeVals(vector<double> vals)
{
  m_rng_vals = vals;
}

//-------------------------------------------------------------
// Procedure: preprocess()

bool PDMapBuilder::preprocess()
{
  // Part 1: Sanity checks
  if((m_dom_vals.size() < 1) || (m_rng_vals.size() < 1))
    return(false);

  if(m_dom_vals.size() != m_rng_vals.size())
    return(false);

  if(m_ivp_domain.size() != 1)
    return(false);

  // The domain values should be on the extreme ends, zero, and N-1
  // where N is the number of points in the domain.

  // First check the left
  if(m_dom_vals[0] != 0)
    return(false);

  // Now check the right
  unsigned int ivp_domain_pts = m_ivp_domain.getVarPoints(0);
  unsigned int dom_val_high = m_dom_vals[m_dom_vals.size()-1];
  
  if((dom_val_high+1) != ivp_domain_pts)
    return(false);
  
  // Part 2: Ensure domain values are ordered.  
  for(unsigned int i=0; i<m_dom_vals.size(); i++) {
    if((i!=0) && (m_dom_vals[i] <= m_dom_vals[i-1]))
      return(false);
  }

  return(true);
}

//-------------------------------------------------------------
// Procedure: getPDMap
//
// Case 1: only one point in the domain
// Case 2: only two points in the domain
// Case 3: first point in the domain (of size 2+)
// Case 4: "consecutive points" (d[i+1] = d[i]+1)
//   4A: one point remaining
//   4B: the normal case (2+ points remaining)
// Case 5: "non-consecutive points"
//   no special subcases

PDMap *PDMapBuilder::getPDMap()
{
  if(preprocess() == false) {
    addWarning("preprocess failed");
    return(0);
  }

  bool all_pieces_built_ok = true; 
  
  // Case 1: only one point in the domain.
  if(m_dom_vals.size() == 1) {
    IvPBox *piece = buildBox(m_dom_vals[0], m_rng_vals[0],
			     m_dom_vals[0], m_rng_vals[0]);
    if(piece)
      m_bld_pieces.push_back(piece);
    else {
      all_pieces_built_ok = false;
      addWarning("Failure in building a piece in the IvPFunction");
    }
  }    
  
  // Case 2: only two points in the domain.
  if(m_dom_vals.size() == 2) {
    IvPBox *piece = buildBox(m_dom_vals[0], m_rng_vals[0],
			     m_dom_vals[1], m_rng_vals[1]);
    if(piece)
      m_bld_pieces.push_back(piece);
    else {
      all_pieces_built_ok = false;
      addWarning("Failure in building a piece in the IvPFunction");
    }
  }    

  // Case 3: handle first point in the domain
  

  
  // All other cases handled here
  if(m_dom_vals.size() > 2) {
    unsigned int i, vsize = m_dom_vals.size();
    for(i=0; (i<vsize-1) && all_pieces_built_ok; i++) {

      IvPBox *piece = 0;  // The piece to be built on this iter
      
      // Case 3: first point in the domain
      if(i == 0) 
	piece = buildBox(m_dom_vals[0], m_rng_vals[0], m_dom_vals[1], m_rng_vals[1]);

      else {
	// Case 4: consecutive points
	if((m_dom_vals[i]+1) == m_dom_vals[i+1]) {
	  // Case 4A: one point remaining
	  if((i+1) == (vsize-1)) {
	    piece = buildBox(m_dom_vals[i+1], m_rng_vals[i+1],
			     m_dom_vals[i+1], m_rng_vals[i+1]);
	  }
	  else {
	    piece = buildBox(m_dom_vals[i], m_rng_vals[i],
			     m_dom_vals[i+1], m_rng_vals[i+1]);
	  }
	}
	// Case 5: normal case
	else {
	  piece = buildBox(m_dom_vals[i], m_rng_vals[i],
			   m_dom_vals[i+1], m_rng_vals[i+1]);
	  if(piece) {
	    int low_ix = piece->pt(0,0);
	    piece->pt(0,0) = low_ix + 1;
	  }
	}
      }
      
      // Handle if a piece wasn't built ok in any of the above cases.
      if(piece)
	m_bld_pieces.push_back(piece);
      else {
	all_pieces_built_ok = false;
	addWarning("Failure in building a piece in the IvPFunction");
      }

    }
  }

  // Check if all pieces were built without a problem. If a problem
  // occurred, clean up the mess and return a null PDMap.
  if(!all_pieces_built_ok) {
    clearBldPieces();
    return(0);
  }

  unsigned int piece_count = m_bld_pieces.size();
  if(piece_count == 0) {
    addWarning("No pieces with which to build a PDMap/IvPFunction");
    return(0);
  }
  
  PDMap *pdmap = new PDMap(piece_count, m_ivp_domain, 1);
  if(!pdmap) {
    addWarning("Null PDMap generated");
    clearBldPieces();
    return(0);
  }
  
  for(unsigned int i=0; i<piece_count; i++)
    pdmap->bx(i) = m_bld_pieces[i];

  // The returning PDMap now "owns" the pieces so clear the vector
  // of pointers.
  m_bld_pieces.clear();

  // We always want to exit this function with an empty m_bld_pieces.
  return(pdmap);
}


//-------------------------------------------------------------
// Procedure: clearBldPieces
//   Purpose: Make sure we don't have any inadvertant memory leaks 
//            due to aborted attempts to create an IvP function.

void PDMapBuilder::clearBldPieces()
{
  for(unsigned int i=0; i< m_bld_pieces.size(); i++) {
    if(m_bld_pieces[i])
      delete(m_bld_pieces[i]);
  }
  m_bld_pieces.clear();
}

//-------------------------------------------------------------
// Procedure: buildBox

IvPBox *PDMapBuilder::buildBox(unsigned int ix_low, double val_low,
			       unsigned int ix_hgh, double val_hgh)
{
  if(ix_low > ix_hgh) {
    addWarning("Building box with low>high end of interval");
    return(0);
  }

  IvPBox *piece = new IvPBox(1,1);
  if(!piece)
    return(0);

  double rise = (val_hgh - val_low);
  double run  = (double)(ix_hgh-ix_low);

  double slope = 0;
  if(run > 0)
    slope = rise / run;

  // b = y-mx
  double intercept = val_low - (slope * (double)(ix_low));

  piece->setPTS(0, ix_low, ix_hgh);
  piece->wt(0) = slope;
  piece->wt(1) = intercept;

  return(piece);
}


//-------------------------------------------------------------
// Procedure: addWarning()

void PDMapBuilder::addWarning(string warning)
{
  m_warnings.push_back(warning);
}


//-------------------------------------------------------------
// Procedure: getWarnings()

string PDMapBuilder::getWarnings()
{
  if(m_warnings.size() == 0)
    return("");

  string return_str = "(" + uintToString(m_warnings.size()) + ")";
  
  for(unsigned int i=0; i<m_warnings.size(); i++)
    return_str += ":" + m_warnings[i];

  return(return_str);
}

//-------------------------------------------------------------
// Procedure: hasWarnings()

bool PDMapBuilder::hasWarnings()
{
  return(m_warnings.size() > 0);
}

//-------------------------------------------------------------
// Procedure: clearWarnings()

void PDMapBuilder::clearWarnings()
{
  m_warnings.clear();
}








