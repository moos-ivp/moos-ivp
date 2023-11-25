/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PDMap.cpp                                            */
/*    DATE: June 1st, 2004 (pulled from OF class)                */
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
#include <cstdio>
#include <cassert>
#include <cmath>
#include "PDMap.h"
#include "BoxSet.h"
#include "IvPGrid.h"

#ifdef _WIN32
#   include <float.h>
#endif

template <typename T>
bool
my_isnan(const T x)
{
#if __cplusplus >= 201103L
  using std::isnan;
#endif
#ifdef _WIN32
  return _isnan(x);
#else
  return isnan(x);
#endif
}

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

PDMap::PDMap(int g_boxCount, const IvPDomain& g_domain, int g_degree)
{
  assert(g_degree >= 0);

  m_boxCount = g_boxCount;
  m_domain   = g_domain;
  m_degree   = g_degree;
  m_grid     = 0;

  int dim = m_domain.size();

  m_gelbox = IvPBox(dim);
  for(int d=0; d<dim; d++)
    m_gelbox.setPTS(d, 0, m_domain.getVarPoints(d)-1);

  m_boxes = new IvPBox *[m_boxCount];
  for(int i=0; i<m_boxCount; i++)
    m_boxes[i] = 0;
}

//-------------------------------------------------------------
// Procedure: Constructor

PDMap::PDMap(const PDMap *pdmap)
{
  int i;
  m_boxCount = pdmap->m_boxCount;
  m_boxes    = new IvPBox *[m_boxCount];
  for(i=0; i<m_boxCount; i++)
    m_boxes[i] = pdmap->m_boxes[i]->copy();

  m_degree   = pdmap->m_degree;
  m_gelbox   = pdmap->getGelBox();
  m_domain   = pdmap->getDomain();  // bugfix mikerb jun3014

  m_grid = new IvPGrid(m_domain, true);
  m_grid->initialize(m_gelbox);
  for(i=0; (i < m_boxCount); i++)
    m_grid->addBox(m_boxes[i], 1, 1);
}

//-------------------------------------------------------------
// Procedure: Destructor

PDMap::~PDMap()
{
  if(m_boxes) {
    for(int i=0; (i < m_boxCount); i++)
      if(m_boxes[i])
	delete(m_boxes[i]);
    delete [] m_boxes;
  }

  if(m_grid) 
    delete(m_grid);
}

//-------------------------------------------------------------
// Procedure: getIX
//   Purpose: o Given a box lying somewhere in domain, return
//              a box in the "boxes" array that intersects it.
//            o Typically the given box is a point box, chosen
//              randomly in the domain. This means that there
//              should only be ONE box in the "boxes" array that
//              intersects it.
//            o This process would be terribly slow w/out the
//              use of a grid. So we assume that the user would
//              not be invoking this function with a null grid.

int PDMap::getIX(const IvPBox *gbox)
{ 
  assert(m_grid);
  BoxSet *intBoxes = m_grid->getBS(gbox);
  int retIX = intBoxes->retBSN()->getBox()->ofindex();
  delete(intBoxes);
  return(retIX);
}

//-------------------------------------------------------------
// Procedure: applyWeight
//   Purpose: Multiply the given "weight" to the interior function
//            of each piece/box in the pdmap. And to the upper
//            bound values set inside the grid.
//      Note: Typical use is in applying the "priority" weight of
//            an objective function. 
//      Note: 

void PDMap::applyWeight(double weight)
{
  for(int i=0; (i < m_boxCount); i++)
    m_boxes[i]->scaleWT(weight);
  if(m_grid) 
    m_grid->scaleBounds(weight);
}

//-------------------------------------------------------------
// Procedure: applyScalar
//   Purpose: Add the given "scalar_val" to the interior function
//            of each piece/box in the pdmap. And to the upper
//            bound values set inside the grid.

void PDMap::applyScalar(double scalar_val)
{
  for(int i=0; (i < m_boxCount); i++)
    m_boxes[i]->moveIntercept(scalar_val);
  if(m_grid) 
    m_grid->moveBounds(scalar_val);
}

//-------------------------------------------------------------
// Procedure: normalize
//   Purpose: 

void PDMap::normalize(double target_base, double target_range)
{
  double existing_base = getMinWT();
  double existing_max  = getMaxWT();
  double existing_range = existing_max - existing_base;

  if(existing_range <= 0)
    return;

  double base_adjustment  = target_base - existing_base; 
  double range_adjustment = target_range / existing_range;

  applyScalar(base_adjustment);
  applyWeight(range_adjustment);
}

//-------------------------------------------------------------
// Procedure: getBS
//   Purpose: 

BoxSet* PDMap::getBS(const IvPBox *qbox)
{
  if(m_grid)
    return(m_grid->getBS(qbox));

  BoxSet *retBS = new BoxSet();
  
  for(int i=0; (i < m_boxCount); i++) 
    if(qbox->intersect(m_boxes[i]))
      retBS->addBox(m_boxes[i]);
  
  return(retBS);
}

//-------------------------------------------------------------
// Procedure: getUniverse
//   Purpose: 

IvPBox PDMap::getUniverse() const
{
  int dim = m_domain.size();
  
  IvPBox rbox(dim, m_degree);
  for(int i=0; i<dim; i++)
    rbox.setPTS(i, 0, m_domain.getVarPoints(i)-1);
  
  return(rbox);
}

//---------------------------------------------------------------------
// Procedure: getMinWT
//   Purpose: Return the minimum value of all points in the domain
//            as evaluated by the individual interior functions for
//            each piece in the pdmap.

double PDMap::getMinWT() const
{
  if(m_boxCount == 0) return(0);

  double minWeight = m_boxes[0]->minVal();

  for(int i=1; (i < m_boxCount); i++) {
    double lowVal  = m_boxes[i]->minVal();
    if(lowVal < minWeight)
      minWeight = lowVal;
  }
  return(minWeight);
}

//---------------------------------------------------------------------
// Procedure: getMaxWT
//   Purpose: Return the maximum value of all points in the domain
//            as evaluated by the individual interior functions for
//            each piece in the pdmap.

double PDMap::getMaxWT() const
{
  if(m_boxCount == 0) return(0);

  double maxWeight = m_boxes[0]->maxVal();

  for(int i=1; (i < m_boxCount); i++) {
    double highVal = m_boxes[i]->maxVal();
    if(highVal > maxWeight)
      maxWeight = highVal;
  }
  return(maxWeight);
}

//---------------------------------------------------------------------
// Procedure: setGelBox
//     Notes: The gelbox is used when initiating the grid. 
//            The given box is checked to make sure that it is the 
//              same dimension as the domain, and that its at
//              least smaller than the domain.

bool PDMap::setGelBox(const IvPBox& b)
{
  int udim = m_domain.size();
  int bdim = b.getDim();

  bool ok = true;

  ok = ok && (udim == bdim);

  for(int d=0; d<bdim; d++) {
    ok = ok && (b.pt(d,0) <= b.pt(d,1));
    ok = ok && (b.pt(d,0) >= 0);
    ok = ok && (b.pt(d,1) <= (int)(m_domain.getVarPoints(d)-1));
  }

  if(!ok) {
    bool ok = true;
    ok = ok && (udim == bdim);
    cout << "DimOK:" << ok << endl;
    if(!ok) {
      m_domain.print();
      cout << "udim:" << udim << endl;
      cout << "bdim:" << bdim << endl;
    }
    for(int d=0; d<bdim; d++) {
      cout << "D: " << d << endl;
      ok = ok && (b.pt(d,0) <= b.pt(d,1));
      cout << "  ok1:" << ok << endl;
      ok = ok && (b.pt(d,0) >= 0);
      cout << "  ok2:" << ok << endl;
      ok = ok && (b.pt(d,1) <= (int)(m_domain.getVarPoints(d)-1));
      cout << "  ok3:" << ok << endl;
    }
    return(false);
  }

  m_gelbox = b;
  return(true);
}

//---------------------------------------------------------------------
// Procedure: setGelBox
//   Purpose: A default way of setting the gelbox, if no way has been
//            adopted. Tries to set the number of gels to 1/4 the number
//            of boxes in this PDMap, with a ceiling of 40K.

void PDMap::setGelBox()
{
  int    gpe  = 1;                           // gels per edge
  double gels = 1;
  int    dim  = m_domain.size();
  int maxGels = (m_boxCount/4);

  if(dim < 1) 
    return;

  if(maxGels < (pow((double)2, (double)dim)))
     maxGels = (int)(pow((double)2, (double)dim));

  if(maxGels > 40000)               // Arbitrary (well kindof)
    maxGels = 40000;

  m_gelbox = IvPBox(dim,0);

  while(gels <= maxGels) {
    gpe++;
    gels = pow(gpe, (double)dim);
  }

  for(int d=0; d<dim; d++) {
    int varsize = m_domain.getVarPoints(d);
    int gelsze  = varsize / gpe;
    if((varsize % gpe) != 0)
      gelsze++;
    if(varsize < gelsze)
      gelsze = varsize;
    m_gelbox.setPTS(d, gelsze, gelsze);
  }
}

//---------------------------------------------------------------------
// Procedure: getGridConfig
//      Note: Added mikerb 12/19/15 for further performance analysis

string PDMap::getGridConfig() const
{
  if(m_grid)
    return(m_grid->getGridConfig());
  return("");
}

//---------------------------------------------------------------------
// Procedure: evalPoint
//     Notes: o Evaluate the value (based on the pieces) of given box.
//            o The given box should be a point box.
//            o If grid is "filled" use it. Else iterate thru boxes.
//            o If "covered" is non-null, it is to reflect whether or not
//              the given pointbox is contained/covered by one of the boxes
//              in the PDMap. This may be interpreted as negative infinity
//              by the caller.


double PDMap::evalPoint(const IvPBox *gbox, bool* covered) const
{
  if(m_boxCount == 0) 
    return(0);

  if(covered) 
    *covered = false;

  double retVal = 0.0;

  if(gbox->isPtBox() != true) {
    gbox->isPtBox();
    return(retVal);
  }    

  if(m_grid==0) {
    //cout << "Warning!!! PDMap::evalPoint() working w/out grid!!!" << endl;
    for(int i=0; (i < m_boxCount); i++)
      if(gbox->intersect(m_boxes[i])) {
      if(covered) 
	*covered = true;
      return(m_boxes[i]->ptVal(gbox));
      }
    //cout << "Failed to find the given point in the PDMap!!!" << endl;
    return(retVal);
  }

  if(m_grid) {
    BoxSet *bs = m_grid->getBS(gbox);
    if(bs->getSize() == 0)
      return(retVal);
    else {
      if(covered) *covered = true;
      if(bs->getSize() > 1) {
	BoxSetNode *bsn = bs->retBSN();
	int numHits = 0;
	while(bsn != NULL) {
	  IvPBox *bx = bsn->getBox();
	  if(gbox->intersect(bx)) {
	    numHits++;
	    retVal = bs->retBSN()->getBox()->ptVal(gbox);
	  }
	  bsn = bsn->getNext();
	}
	if(numHits != 1) {
	  if(covered) *covered = false;
	  return(0.0);
	}
      }
      else
	retVal = bs->retBSN()->getBox()->ptVal(gbox);
    }
    if(bs) delete(bs);
  }
  return(retVal);
}

//-------------------------------------------------------------
// Procedure: updateGrid
//   Purpose: 1) As the grid grows significantly, the number
//               of gridsquares becomes less appropriate.
//            2) While creating new boxes, when a new box is
//               refined or split, the "old" half becomes
//               smaller, but its corresponding gridsquares is
//               not updated, for efficiency sake. So after
//               significant refining, the LLists in the grid
//               squares is less accurate, thus the utility of
//               completely retooling / repopulating a new grid.

void PDMap::updateGrid(bool BX, bool UB)
{
  if(m_gelbox.getDim() == 0) {
    cout << "WARNING: PDMap updateGrid with null gelbox"  << endl;
    cout << "Will guess a good gelbox - perhaps inefficient" << endl;
    setGelBox();
  }

  if(m_grid) 
    delete(m_grid);

  m_grid = new IvPGrid(m_domain, BX);

  m_grid->initialize(m_gelbox);

  for(int i=0; (i < m_boxCount); i++)
    m_grid->addBox(m_boxes[i], BX, UB);
}

//-------------------------------------------------------------
// Procedure: Print

void PDMap::print(bool full) const
{
  cout << "PDMap::print() Total Boxes: " << m_boxCount << endl;
  for(int i=0; (i < m_boxCount); i++) {
    cout << "Box: " << i << "  ";
    m_boxes[i]->print(full);
  }
}

//-------------------------------------------------------------
// Procedure: growBoxArray

void PDMap::growBoxArray(int amt)
{
  int i;
  IvPBox **newboxes = new IvPBox *[m_boxCount + amt];
  for(i=0; (i < m_boxCount); i++)
    newboxes[i] = m_boxes[i];
  for(i=m_boxCount; (i < m_boxCount+amt); i++)
    newboxes[i] = 0;
  delete [] m_boxes;
  m_boxes = newboxes;
}

//---------------------------------------------------------------------
// Procedure: transDomain
//   Purpose:

bool PDMap::transDomain(const IvPDomain& gdomain,
			const int newPlacement[])
{
  int i,j;
  int oldDim = m_domain.size();
  int newDim = gdomain.size();
 
  m_gelbox.transDomain(newDim-oldDim, newPlacement);

  for(i=0; (i < m_boxCount); i++)
    m_boxes[i]->transDomain(newDim-oldDim, newPlacement);

  // setFlag[i] is TRUE if dimension i in all existing boxes is
  // to take on the full range specified by the domain. Typically
  // this is true for the "new" dimensions being added. By default
  // we set this true for all dimensions and then set it to zero
  // each dimension specified in the newPlacement[] array.

  bool *setFlag = new bool[newDim];
  for(i=0; i<newDim; i++)
    setFlag[i] = true;

  for(i=0; i<oldDim; i++)
    setFlag[newPlacement[i]] = false;

  for(i=0; i<newDim; i++)
    if(setFlag[i]) {
      int dimHgh = gdomain.getVarPoints(i)-1;
      m_gelbox.setPTS(i, 0, dimHgh);
      for(j=0; (j < m_boxCount); j++)
	m_boxes[j]->setPTS(i, 0, dimHgh);
    }

  for(i=0; i<oldDim; i++) {
    assert(newPlacement[i] >= 0);
    assert(newPlacement[i] < newDim);
  }

  m_domain = gdomain;  // Added mikerb
  delete [] setFlag;
  if(m_grid)
    updateGrid(1,1);
  return(true);
}

//---------------------------------------------------------------------
// Procedure: removeNULLs
//   Purpose: Remove all NULL entries in the boxes[] array. Useful
//            for situations where several boxes are removed from
//            the OF after it was made, but before being involved
//            in a problem solution.
//            Example is when compaction is done on boxes prior to
//            problem solving. Some boxes may infact be empty and
//            can be removed from the OF.

void PDMap::removeNULLs()
{
  int i, nullCount = 0;
  for(i=0; (i < m_boxCount); i++)
    if(m_boxes[i]==NULL)
      nullCount++;

  if(nullCount == 0)
    return;

  int nonNullIndex = 0;
  IvPBox **newBoxes = new IvPBox*[m_boxCount - nullCount];
  for(i=0; (i < m_boxCount); i++)
    if(m_boxes[i]!=NULL) {
      newBoxes[nonNullIndex] = m_boxes[i];
      nonNullIndex++;
    }
  m_boxCount = m_boxCount - nullCount;
  delete [] m_boxes;
  m_boxes = newBoxes;

  if(nullCount>0)
    updateGrid(1,1);
}

//---------------------------------------------------------------------
// Procedure: freeOfNan
//   Purpose: Confirm that none of the weights comprising the interior
//            function are set to NAN, non-a-number, possibly through an
//            error introduced during regression if the underlying 
//            function should evaluate to NAN during sampling. 

bool PDMap::freeOfNan() const
{
  for(int i=0; (i < m_boxCount); i++) {
    if(!m_boxes[i])
      return(false);
    int wtc = m_boxes[i]->getWtc();
    for(int j=0; j<wtc; j++)
      if(my_isnan(m_boxes[i]->wt(j)))
	return(false);
  }
  return(true);
}

//---------------------------------------------------------------------
// Procedure: valid
//   Purpose: Confirm that each point in the IvPDomain is contained in
//            exactly one box

bool PDMap::valid(bool verbose) const
{
  int dim = (int)(m_domain.size());
  
  IvPBox box(dim);
  for(int d=0; d<dim; d++) 
    box.setPTS(d, 0, 0);

  bool done = false;
  while(!done) {

    unsigned int count = 0;
    for(unsigned int i=0; i<(unsigned int)(m_boxCount); i++)
      if(m_boxes[i]->intersect(&box))
	count++;

    if(count != 1) {
      if(verbose) {
	print();
	cout << "bad box:" << endl;
	box.print();
	cout << "intCount:" << count << endl;
      }
      return(false);
    }

    // Increment
    bool incremented=false;
    for(int d=0; (d<dim)&&(!incremented); d++) {
      int v = box.pt(d);
      if((unsigned int)(v) < m_domain.getVarPoints(d)-1) {
	box.setPTS(d, v+1, v+1);
	incremented = true;
      }
      else
	box.setPTS(d, 0, 0);
    }
    if(!incremented)
      done = true; 
  }    
  return(true);
}






