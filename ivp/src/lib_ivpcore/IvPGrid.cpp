/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPGrid.cpp                                          */
/*    DATE: Too long ago to remember (1996-1999)                 */
/*                                                               */
/* (IvP) The interval programming model is a mathematical        */
/* programming model for representing and solving multi-         */
/* objective optimization problems.                              */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#include <iostream>
#include <assert.h>
#include <math.h>
#include "IvPGrid.h"
#include "IvPDomain.h"

#define min(x, y) ((x)<(y)?(x):(y))
#define max(x, y) ((x)>(y)?(x):(y))

const bool LOW  = 0;
const bool HIGH = 1;

using namespace std;

//---------------------------------------------------------------
// Constructor
// Notes: The constructor does not do many things that are left for
//        the initialization routine. In particular it does not
//        affect the granularity associated with the grid, or the
//        decision as to whether boxes will be stored within the
//        grid. The "boxFlag" determines if a box will be stored in
//        a BoxSet associate with a particular grid when added to the
//        overall grid. It is by default 0 and can be changed upon
//        initialization

IvPGrid::IvPGrid(const IvPDomain& gdomain, bool gboxFlag)
{
  dim           = gdomain.size();
  boxFlag       = gboxFlag;
  total_grids   = 1;          // uninitialized
  grid          = 0;          // uninitialized
  gridUB        = 0;          // uninitialized
  gridLUB       = 0;          // uninitialized
  dup_flag      = false;
  maxval        = 0.0;
  empty         = true;
  GELS_PER_DIM  = new int   [dim];
  PTS_PER_GEL   = new int   [dim];
  DIM_WT        = new long  [dim];
  IX_BOX_BOUND  = new long *[dim];
  IX_BOX        = new long  [dim];
  DOMAIN_LOW    = new int   [dim];
  DOMAIN_HIGH   = new int   [dim];
  DOMAIN_SIZE   = new int   [dim];
  for(int i=0; i<dim; i++) {
    DOMAIN_LOW[i]   = 0;
    DOMAIN_HIGH[i]  = gdomain.getVarPoints(i) - 1;
    DOMAIN_SIZE[i]  = 1+ DOMAIN_HIGH[i] - DOMAIN_LOW[i];
    IX_BOX_BOUND[i] = new long[2];
  }

  // Make sure the "maxpt" box is of the right dimension
  IvPBox tmpbox(dim);
  maxpt = tmpbox;
}

//---------------------------------------------------------------
// Procedure: Destructor

IvPGrid::~IvPGrid()
{
  for(int i=0; i<dim; i++)        // These first eight vars are
    delete [] IX_BOX_BOUND[i];    // non-0 upon creation, so no
  delete [] IX_BOX_BOUND;         // need to check before deleting.

  delete [] GELS_PER_DIM;
  delete [] PTS_PER_GEL;
  delete [] DIM_WT;          
  delete [] IX_BOX;          
  delete [] DOMAIN_LOW;
  delete [] DOMAIN_HIGH;     
  delete [] DOMAIN_SIZE;

  if(gridUB)      delete [] gridUB;            
  if(gridUBFresh) delete [] gridUBFresh;  
  if(grid) {
    for(int i=0; i<total_grids; i++)
      if(grid[i])
	delete(grid[i]);
    delete [] grid;
  }

#if 0  // Linear Upper Bound code in testing
  if(gridLUB) {
    for(int i=0; i<total_grids; i++) 
      delete [] gridLUB[i];
    delete [] gridLUB;
  }
#endif

}

//---------------------------------------------------------------
// Procedure: initByGelBox
//   Purpose: Set number of points per grid element (PTS_PER_GEL).
//            Use given box (gel) for build parameters.
//            Initialize rest of grid by call to initializeAUX.
//   Purpose: o Check for and correct extremes in PTS_PER_GEL.
//            o Set GELS_PER_DIM based on PTS_PER_GEL and DOMAIN_SIZE.
//            o Set DIM_WT, important for indexing the grid.
//            o Create array of BoxSets that are initially empty.

void IvPGrid::initialize(const IvPBox &gelbox)
{
  if(gelbox.getDim() != dim) {
    cout << "dim: " << dim << endl;
    cout << "gelbox dim : " << gelbox.getDim() << endl;
    //int ddim = domain.size();
    //cout << "Domain dim: "  << endl;
    //for(int k=0; k<ddim; k++) {
    //  cout << "  [" << k << "]: " << domain.getVarName(k) << endl;
    //}
    assert(0);
  }
    
  assert(gelbox.getDim() == dim);

  int i;
  for(i=0; i<dim; i++)
    PTS_PER_GEL[i] = gelbox.pt(i, HIGH) + 1;

  for(i=0; i<dim; i++) {                    // Check for extremes.
    if(PTS_PER_GEL[i] < 2)
      PTS_PER_GEL[i] = 2;
    if(PTS_PER_GEL[i] > DOMAIN_SIZE[i])
      PTS_PER_GEL[i] = DOMAIN_SIZE[i];
  }

  for(i=0; i<dim; i++) {
    GELS_PER_DIM[i] = DOMAIN_SIZE[i] / PTS_PER_GEL[i];
    if((DOMAIN_SIZE[i] % PTS_PER_GEL[i]) != 0)
      GELS_PER_DIM[i]++;
    total_grids *= GELS_PER_DIM[i];
  }

  DIM_WT[0] = 1;
  for(int d=1; d<dim; d++)
    DIM_WT[d] = DIM_WT[d-1] * GELS_PER_DIM[d-1];

  // Initialize the Grid. A one dimensional array of pointers to
  // BoxSets. The index can be calculated dynamically from the
  // indexes in each range. The lowest dimension being the "most
  // significant" digit etc.

  if(boxFlag) {
    grid = new BoxSet* [total_grids];
    for(i=0; i<total_grids; i++)
      grid[i] = new BoxSet;
  }

  gridUB      = new double  [total_grids];
  gridUBFresh = new bool    [total_grids];
  for(i=0; i<total_grids; i++)
    gridUBFresh[i] = true;

#if 0  // Linear Upper Bound code in testing
  gridLUB = new double* [total_grids];
  for(i=0; i<total_grids; i++) 
    gridLUB[i] = new double [dim+1];
#endif
}

//---------------------------------------------------------------
// Procedure: addBox
//   Purpose: o Take the given box, add it to the BoxSets residing
//              at each grid square associated with this box.

void IvPGrid::addBox(IvPBox *b, bool BX, bool UB)
{
  setIXBOX(b);                       // Set IX_BOX array.
  long   ix;
  bool   moreGrids = true;

  double b_maxval = b->maxVal();
  
  if(empty || (b_maxval > maxval)) {
    b->maxPt(maxpt);
    maxval = b_maxval;
  }

  while(moreGrids) {
    ix = 0;                          // March thru each grid that
    for(int d=dim-1; d>=0; d--)      // intersects given box. IX_BOX[]
      ix += IX_BOX[d] * DIM_WT[d];   // set in setIXBOX(b) call above.

    if(BX) {
      grid[ix]->addBox(b, LAST);
      empty = false;
    }
    if(UB) {
      if(gridUBFresh[ix] == false) gridUB[ix] = max(gridUB[ix], b_maxval);
      if(gridUBFresh[ix] == true)  gridUB[ix] = b_maxval;

#if 0  // Linear Upper Bound code in testing
      double *wts = b->getWTS();
      if(gridUBFresh[ix] == true) {  // First entry
	if(b->pcwise() == 0) {
	  assert(0);
	  for(int i=0; i<dim; i++)
	    gridLUB[ix][i] = 0.0;
	  gridLUB[ix][dim] = wts[0];
	}
	else {
	  for(int i=0; i<dim+1; i++)
	    gridLUB[ix][i] = wts[i];
	}
      }
      else {                        // Not first entry
	if(b->pcwise() == 0) {
	  assert(0);
	  for(int i=0; i<dim; i++)
	    gridLUB[ix][i] = max(gridLUB[ix][i], 0.0);
	  gridLUB[ix][dim] = max(gridLUB[ix][dim], wts[0]);
	}
	else {
	  for(int i=0; i<dim+1; i++)
	    gridLUB[ix][i] = max(gridLUB[ix][i], wts[i]);
	}
      }
#endif
      gridUBFresh[ix] = false;
    }
    
    moreGrids = moveToNextGrid();
    if(moreGrids) 
      dup_flag = true;
  }
}


//---------------------------------------------------------------
// Procedure: remBox
//   Purpose: o Take given box, visit each of the grids associated
//              with the box, and

void IvPGrid::remBox(const IvPBox *rbox)
{
  setIXBOX(rbox);                   // Set IX_BOX array.

  bool moreGrids = true;
  while(moreGrids) {
    long ix = 0;                    // March thru each grid that
    for(int d=dim-1; d>=0; d--)     // intersects given box. IX_BOX[]
      ix += IX_BOX[d] * DIM_WT[d];  // set in setIXBOX(b) call above.

    BoxSetNode *bsn = grid[ix]->retBSN(FIRST);
    BoxSetNode *nextbsn;
    while(bsn != 0) {
      nextbsn = bsn->getNext();
      IvPBox *ibox = bsn->getBox();
      if(rbox == ibox) {
	grid[ix]->remBSN(bsn);      // Must delete BSN also 
	delete(bsn);             
      }
      bsn = nextbsn;
    }
    moreGrids = moveToNextGrid();
  }
}

//---------------------------------------------------------------
// Procedure: getBS
//   Purpose: o Take given box, visit each of the grids associated
//              with the box, and return all boxes in each grid,
//              that ACTUALLY INTERSECT THE GIVEN BOX.
//            o We want all the duplicates removed.
//            o Assume given box is NOT in the grid

BoxSet *IvPGrid::getBS(const IvPBox *b, bool int_check)
{
  BoxSet *retBS = new BoxSet();
  setIXBOX(b);                      // Set IX_BOX array.

  bool moreGrids = true;
  while(moreGrids) {
    long ix = 0;                  // March thru each grid that
    for(int d=dim-1; d>=0; d--)     // intersects given box. IX_BOX[]
      ix += IX_BOX[d] * DIM_WT[d];  // set in setIXBOX(b) call above.
    
    if(int_check) {
      BoxSetNode *bsn = grid[ix]->retBSN(FIRST);
      while(bsn != 0) {
	IvPBox *iBox = bsn->getBox();
	if(b->intersect(iBox))
	  retBS->addBox(iBox, LAST);
	bsn = bsn->getNext();
      }
    }
    else
      retBS->mergeCopy(*(grid[ix]));

    moreGrids = moveToNextGrid();
  }
  if(dup_flag) retBS->removeDups();

  return(retBS);
}

//---------------------------------------------------------------
// Procedure: getBS
//   Purpose: o Take given box, visit each of the grids associated
//              with the box, and return all boxes in each grid.
//            o We want all the duplicates removed.
//            o Assume given box is NOT in the grid
//      Note: Only user, at this time, of this function is OFR_Nav.cpp
//            The threshhold represents an operating depth, and all
//            boxes in the grid are bathymetry boxes. This is a
//            frequently called function in that algorithm, so we
//            allow this slightly modified version to co-exist with
//            the other "getBS" function.

BoxSet *IvPGrid::getBS_Thresh(const IvPBox *qbox, double thresh)
{
  BoxSet *retBS = new BoxSet();
  setIXBOX(qbox);                   // Set IX_BOX array.

  bool moreGrids = true;
  while(moreGrids) {
    long ix = 0;                    // March thru each grid that
    for(int d=dim-1; d>=0; d--)     // intersects given box. IX_BOX[]
      ix += IX_BOX[d] * DIM_WT[d];  // set in setIXBOX(b) call above.

    BoxSetNode *bsn = grid[ix]->retBSN(FIRST);
    while(bsn != 0) {
      IvPBox *iBox = bsn->getBox();
      if((iBox->maxVal()<thresh) && (qbox->intersect(iBox)))
	retBS->addBox(iBox, LAST);
      bsn = bsn->getNext();
    }
    moreGrids = moveToNextGrid();
  }
  if(dup_flag) retBS->removeDups();
  return(retBS);
}

//---------------------------------------------------------------
// Procedure: getCheapBound
//   Purpose: There is an upper bound associated with each grid element.
//            In this functions the bound is retrieved for either
//            the entire grid (qbox=0) or for the grids that 
//            intersect the given box (qbox!=0).

double IvPGrid::getCheapBound(const IvPBox *qbox)
{
  long    ix;
  double  result=-99999.0;

  bool firstGrid = true;
  if(qbox) {
    setIXBOX(qbox);                  // Set IX_BOX array.
    bool moreGrids = true;
    while(moreGrids) {
      ix = 0;                        // March thru each grid that
      for(int d=dim-1; d>=0; d--)    // intersects given box. IX_BOX[]
	ix += IX_BOX[d] * DIM_WT[d]; // set in setIXBOX(b) call above.
      if(!gridUBFresh[ix])
	if(firstGrid || (gridUB[ix]>result))
	  result = gridUB[ix];
      firstGrid = false;
      moreGrids = moveToNextGrid();
    }
  }
  if(!qbox) {
    for(int ix=1; ix<total_grids; ix++) {
      if(!gridUBFresh[ix])
	if(firstGrid || (gridUB[ix] > result))
	  result = gridUB[ix];
    }
  }
  return(result);
}

//---------------------------------------------------------------
// Procedure: getLinearBound

double* IvPGrid::getLinearBound(const IvPBox *qbox)
{
  assert(gridLUB);

  long    ix;
  double* result = new double[dim+1];

  if(qbox) {
    setIXBOX(qbox);                  // Set IX_BOX array.
    bool moreGrids = true;
    bool firstGrid = true;
    while(moreGrids) {
      ix = 0;                        // March thru each grid that
      for(int d=dim-1; d>=0; d--)    // intersects given box. IX_BOX[]
	ix += IX_BOX[d] * DIM_WT[d]; // set in setIXBOX(b) call above.
      if(firstGrid)
	for(int i=0; i<dim+1; i++)
	  result[i] = gridLUB[ix][i];
      else
	for(int i=0; i<dim+1; i++)
	  result[i] = max(gridLUB[ix][i], result[i]);
      firstGrid = false;
      moreGrids = moveToNextGrid();
    }
  }
  return(result);
}

//---------------------------------------------------------------
// Procedure: getTightBound
//   Purpose: The tight bound is derived by getting all boxes 
//              belonging to all grid elements intersecting qbox.

#if 0
double IvPGrid::getTightBound(const IvPBox *qbox)
{
  assert(qbox);
  
  BoxSet*     qbs = getBS(qbox);

  BoxSetNode* bsn = qbs->retBSN(FIRST);
  double      res = bsn->getBox()->maxVal();

  bsn = bsn->getNext();
  while(bsn != 0) {
    double val = bsn->getBox()->maxVal();
    if(val > res)
      res = val;
    bsn = bsn->getNext();
  }

  return(res);
}
#endif

//---------------------------------------------------------------
// Procedure: scaleBounds
//   Purpose: Useful when the grid is associated with an objective
//            function, and that function has just had a multiplier,
//            i.e., priority weight, applied to the interior 
//            functions of all its boxes. 

void IvPGrid::scaleBounds(double amount)
{
  for(int ix=0; ix<total_grids; ix++) {
    if(!gridUBFresh[ix])
      gridUB[ix] = gridUB[ix] * amount;

#if 0  // Linear Upper Bound code in testing
    for(int j=0; j<dim+1; j++)
      gridLUB[ix][j] = gridLUB[ix][j] * amount;
#endif
  }
}

//---------------------------------------------------------------
// Procedure: scaleBounds
//   Purpose: Useful when the grid is associated with an objective
//            function, and that function is being normalized.

void IvPGrid::moveBounds(double amount)
{
  for(int ix=0; ix<total_grids; ix++) {
    if(!gridUBFresh[ix])
      gridUB[ix] += amount;
  }
}

//---------------------------------------------------------------
// Procedure: setIXBOX
//   Purpose: o Determine set of grids that intersect given box
//            o The settings of IX_BOX[] indicate a unique grid
//              contained in the set of grids indicated by the
//              setting of IX_BOX_BOUND[].
//            o This function sets the values of IX_BOX_BOUND[]
//              to indicate the grids that intersect with the
//              given box. IX_BOX[] is set to highest grid.

void IvPGrid::setIXBOX(const IvPBox* b)
{
  long relPT = 0;
  for(int d=0; d<dim; d++) {
    if(b->bd(d,0) == 1)
      relPT = max(0, b->pt(d, LOW)-DOMAIN_LOW[d]);
    else
      relPT = max(0, 1 + b->pt(d, LOW)-DOMAIN_LOW[d]);
    IX_BOX_BOUND[d][LOW] = relPT  / PTS_PER_GEL[d];
    relPT = min(DOMAIN_HIGH[d]-DOMAIN_LOW[d],
		b->pt(d, HIGH)-DOMAIN_LOW[d]);
    IX_BOX_BOUND[d][HIGH] = relPT / PTS_PER_GEL[d];
    IX_BOX[d] = IX_BOX_BOUND[d][HIGH];
  }
}

//---------------------------------------------------------------
// Procedure: moveToNextGrid
//   Purpose: o Advance the current grid to the next one.
//            o The settings of IX_BOX[] indicate a unique grid
//              contained in the set of grids indicated by the
//              setting of IX_BOX_BOUND[].
//            o This function moves the IX_BOX[] setting to
//              the "next" grid in the IX_BOX_BOUND[] set.

inline bool IvPGrid::moveToNextGrid()
{
  bool moreGrids = false;
  for(int d=dim-1; (d>=0)&&(!moreGrids); d--) {
    if(IX_BOX[d] > IX_BOX_BOUND[d][LOW]) {
      IX_BOX[d]--;
      moreGrids = true;
    }
    else
      if(d != 0) IX_BOX[d] = IX_BOX_BOUND[d][HIGH];
  }
  return(moreGrids);
}

//---------------------------------------------------------------
// Procedure: calcBoxesPerGEL
//   Purpose: Prints general info on grid construction

double IvPGrid::calcBoxesPerGEL()
{
  //  cout << "PTS_PER_GEL: " << PTS_PER_GEL[0] << endl;
  double total = 0;
  for(int i=0; i< total_grids; i++)
    total += (double)(grid[i]->getSize());
  return(total / (double)total_grids);
}

//---------------------------------------------------------------
// Procedure: print_1
//   Purpose: Prints general info on grid construction

void IvPGrid::print_1(int flag)
{
  int i;
  int totalunused = 0;
  cout << "Dimensions: " << dim << endl;
  cout << "Domain Size: " << endl;
  cout << "Grid edge points: " << endl;
  for(i=0; i<dim; i++)
    cout << "d[" << i << "]: " << GELS_PER_DIM[i] << endl;
  cout << "Points Per Edge: " << endl;
  for(i=0; i<dim; i++)
    cout << "d[" << i << "]: " << PTS_PER_GEL[i] << endl;
  cout << "Weight Per Dimension: " << endl;
  for(i=0; i<dim; i++)
    cout << "d[" << i << "]: " << DIM_WT[i] << endl;
  cout << "Total number of Grids: " << total_grids << endl;
  cout << "Total number of Boxes: ";
  if(flag) {
    int total = 0;
    for(i=0; i<total_grids; i++) {
      total += grid[i]->getSize();
      cout << "Testing" << endl;
      if(grid[i]->getSize()==0)
	totalunused++;
    }
    cout << "Total unused grid squares: " << totalunused;
    float pct = (float)totalunused / (float)total_grids;
    cout << "  pct: " << pct << endl;
    cout << total << endl << endl;
  }
}

//---------------------------------------------------------------
// Procedure: print_2
//   Purpose: Prints general info on grid construction

void IvPGrid::print_2()
{
  int i;
  int totalunused = 0;
  for(i=0; i<total_grids; i++)
    if(grid[i]->getSize() == 0)
      totalunused++;

  cout << "Grid --> GELS:" << total_grids;
  cout << "  PCS/GEL:" << calcBoxesPerGEL();
  cout << "  EMPTY-GELS:" << totalunused << endl;

  cout << "   PTS/GEL:"; 
  for(i=0; i<dim; i++)
    cout << " [" << i << "]:" << PTS_PER_GEL[i];
  cout << endl;
  cout << "   GELS/DIM:"; 
  for(i=0; i<dim; i++)
    cout << "[" << i << "]: " << GELS_PER_DIM[i];
  cout << endl;
}

//---------------------------------------------------------------
// Procedure: printBoxIXS
//   Purpose: o Print out the indexes for each grid that intersects
//              the given box.

void IvPGrid::printBoxIXS(const IvPBox *b)
{
  setIXBOX(b);                      // Set IX_BOX array.
  cout << "{";
  bool  moreGrids = true;
  while(moreGrids) {
    double ix = 0;                  // March thru each grid that
    for(int d=dim-1; d>=0; d--)     // intersects given box. IX_BOX[]
      ix += IX_BOX[d] * DIM_WT[d];  // set in setIXBOX(b) call above.
    cout << ix << ", ";
    moreGrids = moveToNextGrid();
  }
  cout << "}" << endl << flush;
}

//---------------------------------------------------------------

bool IvPGrid::grid_ok() 
{
  for(int i=0; i<total_grids; i++) {
    BoxSet *bs = grid[i];
    BoxSetNode *bsn = bs->retBSN();
    while(bsn!=0) {
      if(bsn->getBox()->getDim() != dim) {
	cout << "i:" << i << " of total: " << total_grids << endl;
	cout << " " << bsn->getBox()->getDim();
	return(false);
      }
      bsn = bsn->getNext();
    }
  }
  cout << "Grid is OK!!!!" << endl;
  return(true);
}









