/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Flatten.cpp                                          */
/*    DATE: April 28th 2006                                      */
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
#include "Flatten.h"
#include "BuildUtils.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: flattenFunction
//   Purpose: 

IvPFunction *flattenFunction(IvPFunction *ivp_function, 
			     const IvPDomain& map_domain)
{
  // Handle error case if pass a null IvPFunction
  if(!ivp_function)
    return(0);
  
  PDMap *pdmap = ivp_function->getPDMap();

  IvPDomain ivp_domain = pdmap->getDomain();

  // Handle error case if passed mis-matched domain sizes
  if(map_domain.size() != ivp_domain.size())
    return(0);

  // Handle error case if passed mis-matched domain names
  int i, dim = map_domain.size();
  for(i=0; i<dim; i++) 
    if(map_domain.getVarName(i) != ivp_domain.getVarName(i))
      return(0);

  // Check that the map_domain is otherwise legal - has only 
  // two dimensions specified for target 2D representation, 
  // and all others have a single sample point specified.
  int dim_count = 0;
  for(i=0; i<dim; i++) {
    if(map_domain.getVarPoints(i) != 1) 
      dim_count++;
    else
      if(map_domain.getVarLow(i) != map_domain.getVarHigh(i))
	return(0);
  }
  if(dim_count > 2)
    return(0);


  // Build the "flatten map" or "fmap".
  int *fmap = new int[dim];
  for(i=0; i<dim; i++) {
    if(map_domain.getVarPoints(i) != 1) {
      fmap[i] = -1;
    }
    else {
      double map_val = map_domain.getVarLow(i);
      double dlow    = ivp_domain.getVarLow(i);
      double ddelta  = ivp_domain.getVarDelta(i);
      double adelta  = (map_val - dlow);
      int    ival    = (int)((adelta/ddelta)+0.5);
      fmap[i] = ival;
    }
  }

  PDMap *new_pdmap = flattenPDMap(pdmap, fmap);
  delete [] fmap;
  
  if(new_pdmap) {
    IvPFunction *new_function = new IvPFunction(new_pdmap);
    return(new_function);
  }
  else
    return(0);
}

//--------------------------------------------------------------
// Procedure: flattenPDMap
//   Purpose: 

PDMap *flattenPDMap(PDMap *old_pdmap, const int* fmap)
{
  if(!old_pdmap || !fmap)
    return(0);

  // If the given PDMap is already 2D - just return a copy of it
  if(old_pdmap->getDim() == 2) {
    PDMap *new_pdmap = new PDMap(old_pdmap);
    return(new_pdmap);
  }
    
  // Otherwise, build a new PDMap by first building a query box
  // to be used to get boxes from the grid that intersect the 
  // cutting plane represented by desired 2D flattened function.
  int i, dim = old_pdmap->getDim();
  int degree = old_pdmap->getDegree();
  IvPDomain domain = old_pdmap->getDomain();
  IvPBox query_box = domainToBox(domain);

  for(i=0; i<dim; i++)
    if(fmap[i] != -1)
      query_box.setPTS(i, fmap[i], fmap[i]);
  
    
  // Now use the query box to get the boxes. 
  BoxSet *orig_set = old_pdmap->getBS(&query_box);
  BoxSet  flat_set;
  
  // Now get a "flattened" copy of the above flat_set boxes.
  BoxSetNode *bsn = orig_set->retBSN();
  while(bsn) {
    IvPBox *new_box = flattenBox(bsn->getBox(), fmap);
    
    if(new_box)
      flat_set.addBox(new_box);
    bsn = bsn->getNext();
  }

  // If something went wrong, free memory and return null.
  if(orig_set->getSize() != flat_set.getSize()) {
    flat_set.makeEmptyAndDeleteBoxes();
    delete(orig_set);
    return(0);
  }
      
  // Now build a new PDMap from the flattened boxes.
  int index    = 0;
  int new_size = flat_set.getSize();
  PDMap *new_pdmap = new PDMap(new_size, domain, degree);
  bsn = flat_set.retBSN();
  while(bsn) {
    new_pdmap->bx(index) = bsn->getBox();
    index++;
    bsn = bsn->getNext();
  }

  // Flatten the Grid Element Box also and reset
  IvPBox old_gelbox = old_pdmap->getGelBox();
  IvPBox *new_gelbox = flattenBox(&old_gelbox, fmap);
  new_pdmap->setGelBox(*new_gelbox);

  // Free up memory taken from the heap
  delete(orig_set);
  delete(new_gelbox);

  return(new_pdmap);
}

//---------------------------------------------------------------
// Procdudure: flattenBox
//    Purpose: Create a 2D box from the given old_box. The slice
//             is determined by the integer array fmap. 

IvPBox *flattenBox(const IvPBox *old_box, const int* fmap)
{
  int old_deg = old_box->getDegree();
  int old_dim = old_box->getDim();

  // Check for boxes that are not not flattenable.
  // If the old box is already 2D, return NULL.
  // Only handle piecewise constant (deg=0) or linear (deg=1)
  if((old_deg > 1) || (old_dim < 2))
    return(0);

  IvPBox *new_box = new IvPBox(2, old_deg);

  // First determine the boundary info for the new box.
  int  rdim = 0;
  for(int i=0; i<old_dim; i++) {
    if(fmap[i] == -1) {
      new_box->setPTS(rdim, old_box->pt(i,0), old_box->pt(i,1));
      new_box->setBDS(rdim, old_box->bd(i,0), old_box->bd(i,1));
      rdim++;
    }
  }
  
  // check that the flatten map (fmap) didn't have more than
  // two dimensions for saving.
  if(rdim != 2) {
    delete(new_box);
    return(0);
  }

  // Now handle setting the weight(s) for the new box.
  // wts [0]   [1]   [2]   [3]   {4}   old_box
  //     0.7   0.5   0.1   0.8   0.2   old_box wts
  //     -1     2    -1     4          fmap
  //
  // wts [0]   [1]    {2}  
  //     0.7   0.1   (0.2+(0.5*2)+(0.8*4))

  if(old_deg == 0) { // pcwise constant
    new_box->wt(0) = old_box->wt(0);
  }
  else {
    double wtval = old_box->wt(old_dim);
    rdim = 0;
    for(int i=0; i<old_dim; i++) {
      if(fmap[i] == -1) {
	new_box->wt(rdim) = old_box->wt(i);
	rdim++;
      }
      else
	wtval += old_box->wt(i) * fmap[i];
    }
    new_box->wt(2) = wtval;
  }

  return(new_box);
}







