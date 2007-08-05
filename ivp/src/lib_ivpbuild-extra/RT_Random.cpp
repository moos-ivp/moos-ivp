/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_Random.cpp                                        */
/*    DATE:                                                      */
/*    NOTE: "RT_" stands for "Reflector Tool"                    */
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

#include "RT_Random.h"
#include "BuildUtils.h"
#include "PDMap.h"
#include "PQueue.h"
#include "Regressor.h"
#include "OF_Rater.h"

#define min(x, y) ((x)<(y)?(x):(y))
#define max(x, y) ((x)>(y)?(x):(y))

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor
//     Notes: g_pcwise=1 makes piecewise linear functions.
//            g_pcwise=0 makes piecewise constant functions.
//            g_aof is the underlying function to be approximated.

RT_Random::RT_Random(const AOF *g_aof, Regressor *g_reg) 
{
  aof       = g_aof;
  regressor = g_reg;
  pdmap     = 0;
  of_rater  = 0;

  // Set default values for the four different selection policies.
  // These policies determine, in part, which piece gets selected
  // for refinement at every iteration in the refine-loop.

  selPol1      = 10;
  selPol2      = 10;
  selPol3      = 10;
  selPol4      = 10;
  quesamp      =  5;
  pqueue       = new PQueue(7, true);
  targWorstErr = 0.0;
  targAvgErr   = 0.0;
}

//-------------------------------------------------------------
// Procedure: Destructor
//      Note: the member variable "pdmap" is not freed since it 
//            it is common convention to use the pdmap as the 
//            product of the creation process, typically in a new
//            objective function - see RT_Random::pruneToOF()

RT_Random::~RT_Random()
{
  if(of_rater)  
    delete(of_rater);
}

//-------------------------------------------------------------
// Procedure: create
//   Purpose: Make a uniform IvP function based on the given box.
//            Each uniform piece will have the same size as the
//            given box.
//            After the uniform pieces are created, an additional
//            set of pieces up to "targPieces" will be created.
//  ****Note: If targPieces is greater than zero, and the request
//            for uniform pieces is greater than this amount, the
//            building of uniform pieces is aborted entirely.
//            If targPieces is less than or equal to zero, the
//            request for uniform pieces is not bounded and no
//            additional pieces are created afterwards.

PDMap* RT_Random::create(PDMap *g_pdmap, 
			 int totalPieces, const IvPBox *g_gelbox)
{
  pdmap = g_pdmap;

  if(of_rater)
    delete(of_rater);
  of_rater = new OF_Rater(pdmap, aof);


  if(g_gelbox) {
    pdmap->setGelBox(*g_gelbox);
    pdmap->updateGrid(1,1);
  }

#if 0
  if(pdmap->getGelBox() == 0) {
    pdmap->setGelBox(universe);
    cout << endl << "WARNING: ";
    cout << "RT_Random creation w/out hint of gel size" << endl;
    cout << "Will continue universe as gel - inefficient" << endl;
    pdmap->updateGrid(1,1);
  }
#endif

  if(totalPieces > pdmap->size()) {
    int nonUnifPieces = totalPieces - pdmap->size();
    createNonUniform(nonUnifPieces);
  }

  for(int j=0; j<pdmap->size(); j++)
    pdmap->bx(j)->ofindex() = j;

  if(pdmap->getGrid()==0)
    pdmap->updateGrid(1,1);

  return(pdmap);
}

//-------------------------------------------------------------
// Procedure: createNonUniform
//   Purpose: o Create the piecewise defined representation of
//              associated "aof" i.e. analytical function.


int RT_Random::createNonUniform(int newBoxAmt)
{
  if(aof==NULL) return(0);

  int boxCount   = pdmap->size();
  int targBoxCnt = boxCount + newBoxAmt;

  pdmap->growBoxArray(newBoxAmt);

  if(boxCount==0) {
    IvPBox newbox = domainToBox(aof->getDomain());    
    pdmap->bx(0) = newbox.copy();
    pdmap->getGrid()->addBox(pdmap->bx(0));
    pdmap->growBoxCount(1);
  }

  qualCheckCtr = 0;
  while(!createdEnough(targBoxCnt)) {
    int selectIX = selectBox();
    refineBox(selectIX, targBoxCnt);
  }

  for(int j=0; j<boxCount; j++)
    pdmap->bx(j)->ofindex() = j;

  return(1);
}

//-------------------------------------------------------------
// Procedure: selectBox
//   Purpose: Choose index of crush box

int RT_Random::selectBox()
{
  int policy, selectIX = 0;
  int twt = selPol1 + selPol2 + selPol3 + selPol4;
  int b1  = twt - selPol1;
  int b2  = b1  - selPol2;
  int b3  = b2  - selPol3;

  int rix = rand() % twt;
  if((rix >= b1) && (rix <= twt)) policy = 1;  // Random box
  if((rix >= b2) && (rix <   b1)) policy = 2;  // Big box
  if((rix >= b3) && (rix <   b2)) policy = 3;  // Worst box !Q
  if((rix >=  0) && (rix <   b3)) policy = 4;  // Worst box  Q

  if(policy==1) {
    int boxCount = pdmap->size();
    selectIX = rand() % boxCount;       // Policy1: Random box
  }

  if(policy==2) {                       // Policy2: Favor Big
    IvPDomain domain = aof->getDomain();
    IvPBox pt_box = makeRand(domain);
    selectIX = pdmap->getIX(&pt_box);
  }

  if(policy==3) {                       // Policy3: Worst NoQ
    double val1, val2, diff;
    int    i, numPts = 8;
    double worstVal  = -1.0;
    int    worstIX   = 0;
    vector<IvPBox> test_boxes;

    IvPDomain domain = aof->getDomain();
    for(i=0; i<numPts; i++) {
      IvPBox test_box = makeRand(domain);
      test_boxes.push_back(test_box);
      val1 = aof->evalBox(&test_box);
      val2 = pdmap->evalPoint(&test_box);
      diff = (val1-val2);
      if(diff<0)
	diff = (diff * -1.0);
      if(diff>worstVal) {
	worstVal = diff;
	worstIX  = i;
      }
    }
    selectIX = pdmap->getIX(&(test_boxes[worstIX]));
  }

  if(policy==4) {                       // Policy4: Worst (Q)
    sampleWorst(quesamp);
    selectIX = pqueue->removeBest();
  }

  return(selectIX);
}


//-------------------------------------------------------------
// Procedure: refineBox
//   Purpose:

void RT_Random::refineBox(int selectIX, int targBoxCnt)
{
  int boxCount = pdmap->size();

  IvPBox *clone_box = pdmap->bx(selectIX)->copy();
  IvPBox *split_box = splitBox(clone_box, 1);
  
  if(split_box) {
    pdmap->bx(selectIX)->copy(clone_box);
    regressor->setWeight(split_box);
    regressor->setWeight(pdmap->bx(selectIX));
    pdmap->bx(boxCount) = split_box;
    pdmap->bx(boxCount)->ofindex() = boxCount;
    pdmap->getGrid()->addBox(split_box);
    pdmap->growBoxCount(1);
  }

  delete(clone_box);
}

//-------------------------------------------------------------
// Procedure: createdEnough
//   Purpose:

bool RT_Random::createdEnough(int targBoxCnt)
{
  int boxCount = pdmap->size();

  if(targBoxCnt && (boxCount >= targBoxCnt))
    return(true);

  if((targWorstErr > 0) || (targAvgErr > 0)) {
    int qualCheckInc = 1000;
    if(((boxCount+1) - qualCheckCtr) > qualCheckInc) {
      qualCheckCtr += qualCheckInc;
      of_rater->resetSamples();
      of_rater->takeSamples(30000, targWorstErr);
      double avgErr   = of_rater->getAvgErr();
      double worstErr = of_rater->getWorstErr();
      int okAvg = (!targAvgErr   || (avgErr   < targAvgErr));
      int okWor = (!targWorstErr || (worstErr < targWorstErr));
      if(okAvg && okWor) return(true);
    }
  }
  return(false);
}

//-------------------------------------------------------------
// Procedure: setParams
//   Purpose:

bool RT_Random::setParams(vector<string> param, vector<double> val)
{
  int psize = param.size();
  int vsize = val.size();
  if(psize != vsize)
    return(false);

  for(int i=0; i<psize; i++) {
    if(param[i] == "SEL_RAND")
      selPol1 = (int)(val[i]);
    else if(param[i] == "SEL_SIZE")
      selPol2 = (int)(val[i]);
    else if(param[i] == "SEL_WORST_Q")
      selPol3 = (int)(val[i]);
    else if(param[i] == "SEL_WORST_NQ")
      selPol4 = (int)(val[i]);
    else if(param[i] == "ERR_WORST")
      targWorstErr = val[i];
    else if(param[i] == "ERR_AVG")
      targAvgErr = val[i];
    else if(param[i] == "QUEUE_SAMP")
      quesamp = max(1, (int)(val[i]));
    else if(param[i] == "QUEUE_LEVELS") {
      int qsize = (int)(val[i]);
      if(pqueue) delete(pqueue);
      pqueue = new PQueue(qsize, 1);
    }
    else
      return(false);
  }
  return(true);
}

//-------------------------------------------------------------
// Procedure: sampleWorst
//   Purpose:
//    rexAmt: The amount of times that the top element in the
//            pqueue is re-examined (and pushed back into the
//            queue).

void RT_Random::sampleWorst(int sampAmt)
{
  int i, rexAmt = min((sampAmt/3), pqueue->size());

  rexAmt=0;
  for(i=0; i<rexAmt; i++) {
    int     testix  = pqueue->removeBest();
    IvPBox* testBox = pdmap->bx(testix);
    double  val1 = aof->evalBox(testBox);
    double  val2 = pdmap->evalPoint(testBox);
    double  diff = (val1-val2);
    if(diff<0) 
      diff = (diff * -1.0);
    pqueue->insert(testix, diff);
  }

  for(i=rexAmt; i<sampAmt; i++) {
    IvPDomain domain = aof->getDomain();
    IvPBox test_box = makeRand(domain);
    int    test_ix  = pdmap->getIX(&test_box);
    // could we make the above faster by associating a test box
    // with the pqueue that is always the last one just bumped from
    // the pqueue?
    double val1 = aof->evalBox(&test_box);
    double val2 = pdmap->evalPoint(&test_box);
    double diff = (val1-val2);
    if(diff<0)
        diff = (diff * -1.0);
    pqueue->insert(test_ix, diff);
  }
}

//-------------------------------------------------------------
// Procedure: getSmallestSide

int RT_Random::getSmallestSide(const IvPBox *box)
{
  int dim = box->getDim();
  int res = 1 + (box->pt(0,1) - box->pt(0,0));
  for(int d=1; d<dim; d++) {
    int edge_length = 1+ (box->pt(d,1) - box->pt(d,0));
    res = min(res, edge_length);
  }
  return(res);
}



//---------------------------------------------------------------
// Procedure: splitBox
//   Purpose: Split off a new box from THIS box. Return new box.
//            Policy Zero: split on a random edge.
//            Policy One:  split on the longest edge
//            Policy Two:  split on the given dimension
//     Notes: Nothing is done with the WEIGHT of either box.
//   Returns: A new box, or NULL if cannot be split. Box cannot
//            be split if ALL sides have edge length of one, or
//            if the demanded side has an edge length of one.

IvPBox* RT_Random::splitBox(IvPBox *gbox, int policy)
{
  // First determine the longest box edge. If the longest edge is
  // only 1, we cannot split, and return NULL. Otherwise, we may
  // use the longest edge information to split based on policy.

  int dim = gbox->getDim();
  int longest_edge = 1;
  int longest_dim  = 0;
  for(int i=0; i<dim; i++) {
    int ilength = gbox->pt(i, 1) - gbox->pt(i, 0) + 1;
    if(ilength > longest_edge) {
      longest_dim  = i;
      longest_edge = ilength;
    }
  }
  if(longest_edge == 1)
    return(0);

  int split_dim;
  int split_length=0;

  if(policy == 0) {        // Random edge
    while(split_length < 2) {
      split_dim = rand() % dim;
      split_length = gbox->pt(split_dim, 1) - 
	gbox->pt(split_dim, 0) + 1;
    }
  }
  else if(policy == 1) {        // Longest edge
    split_dim    = longest_dim;
    split_length = longest_edge;
  }
  else
    return(0);

  if(split_length <= 1)
    return(0);

  IvPBox *newHalf = gbox->copy();
  int  newHigh = gbox->pt(split_dim, 0);

  newHigh += (int)((split_length+1)/2);

  gbox->pt(split_dim, 1) = newHigh-1;
  newHalf->pt(split_dim, 0) = newHigh;
  gbox->bd(split_dim, 1) = 1;
  newHalf->bd(split_dim, 0) = 1;

  return(newHalf);
}





