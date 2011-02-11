/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPProblem.cpp                                       */
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
#include <stdio.h>
#include <assert.h>
#include "IvPProblem.h"
#include "IvPGrid.h"
#include "PDMap.h"
#include "CompactorNull.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: Constructor
//      Note: If a compactor is provided, it is assumed that we 
//            don't own it. If none is passed we create a "null" 
//            compactor and declare ownership so it will be 
//            deleted in the destructor.

IvPProblem::IvPProblem(Compactor *g_compactor)
{
  nodeBox = 0;
  if(g_compactor) {
    compactor = g_compactor;
    ownCompactor = false;
  }
  else {
    compactor = new CompactorNull;
    ownCompactor = true;
  }
}

//---------------------------------------------------------------
// Procedure: Destructor

IvPProblem::~IvPProblem() 
{
  if(ownCompactor)
    delete(compactor);
}

//---------------------------------------------------------------
// Procedure: preCompact()
//   Purpose: Compact before inserting into the grid to improve
//            grid efficiency.
//     Notes: When an OF is "expanded" by creating new dimensions
//            for each of its boxes, the range in the new dimen-
//            sion spans the entire domain of that variable.
//            If the boxes were inserted into the grid in this
//            state, each box would reside in many more grids 
//            than necessary, thereby reducing grid efficiency.

void IvPProblem::preCompact()
{
  for(int i=0; (i < m_ofnum); i++) {
    PDMap *pdmap = m_ofs[i]->getPDMap();
    int pieces = pdmap->size();
    for(int j=0; j<pieces; j++) {
      IvPBox* cbox = pdmap->bx(j);
      bool violation = compactor->compact(cbox);
      if(violation) {
	delete(cbox);
      	pdmap->bx(j) = NULL;
      }
    } 
    pdmap->removeNULLs();
  }
}

//---------------------------------------------------------------
// Procedure: solvePrior
//   Purpose: Does the necessary things before starting the branch
//            and bound process.

void IvPProblem::solvePrior(const IvPBox *isolBox)
{
  // Reset timer (may have values from previous solve invocation)
  // Start timer after (perhaps) outputting start message.
  if(!m_silent) cout << "---> entering IvP Solve routine: " << endl;

  // A nodeBox is associated with each level of the tree. 
  // Initialized here rather than in constructor since we need to 
  // know the number of objective functions first.
  nodeBox = new IvPBox*[m_ofnum+1];

  for(int i=0; (i < m_ofnum+1); i++)
    nodeBox[i] = m_ofs[0]->getPDMap()->getUniverse().copy();
  nodeBox[0]->setWT(0.0);
  
  if(isolBox)
    processInitSol(isolBox);

  // Really shouldn't have to take care of the grid here, but will
  // do anyway so we can run the solve process confident that all
  // OF's have a grid, even if it is a one or two piece OF.
  for(int j=0; (j < m_ofnum); j++) {
    PDMap *pdmap = m_ofs[j]->getPDMap();
    if(pdmap->getGrid() == 0) {
      //cout << "Warning: IvPProblem::solve() called with of[" << j;
      //cout << "] having a null grid. A default one was provided" << endl;
      pdmap->updateGrid();
    }
  }

}

//---------------------------------------------------------------
// Procedure: solve
//   Purpose: This is the entry point for the main solve routine of
//            IvP problems. 
//      Note: It utilizes three utility functions: 
//            solvePrior   - stuff done prior to Branch 'n Bound 
//            solveRecurse - search down into the tree
//            solvePost    - stuff done after Branch and Bound

bool IvPProblem::solve(const IvPBox *isolBox)
{
  if(m_ofnum == 0) {
    cout << "IvPProblem::solve() - zero OFS!!!!!" << endl;
    return(false);
  }

  solvePrior(isolBox); 

  if(!m_silent) {
    cout << "*******Entering IvPProblem::solve()" << endl;
    cout << "Ofs:" << m_ofnum << endl;
  }
  
  PDMap *pdmap = m_ofs[0]->getPDMap();
  int boxCount = pdmap->size();
  for(int i=0; i<boxCount; i++) {
    nodeBox[1]->copy(pdmap->bx(i));
    if(!m_maxbox || (upperCheapBound(1, nodeBox[1]) > (m_maxwt + m_epsilon)))
      solveRecurse(1);
  }    
 
  solvePost();

  return(true);
}

//---------------------------------------------------------------
// Procedure: solveRecurse

void IvPProblem::solveRecurse(int level)
{
  int result;
  
  // check for and handle the boundary condition
  if(level == m_ofnum) {
    bool   ok = false;
    double currWT = compactor->maxVal(nodeBox[level], &ok);
    if(ok)
      if((m_maxbox==NULL) || (currWT > (m_maxwt + m_epsilon)))
	newSolution(currWT, nodeBox[level]);
    return;
  }
  
  IvPGrid *grid = m_ofs[level]->getPDMap()->getGrid();
  BoxSet *levelBoxes;
  if(grid)
    levelBoxes = grid->getBS(nodeBox[level]);
  else
    levelBoxes = m_ofs[level]->getPDMap()->getBS(nodeBox[level]);
  BoxSetNode *levBSN = levelBoxes->retBSN(FIRST);

  while(levBSN != NULL) {
    BoxSetNode *nextLevBSN = levBSN->getNext();

    IvPBox *cbox = levBSN->getBox();
    result = nodeBox[level]->intersect(cbox, nodeBox[level+1]);
    
    if(result) {
      double upperBound = upperCheapBound(level+1, nodeBox[level+1]);
      if(!m_maxbox || (upperBound > (m_maxwt + m_epsilon)))
	solveRecurse(level+1);
    }

    levBSN = nextLevBSN;
  }
  delete(levelBoxes);
}


//---------------------------------------------------------------
// Procedure: solvePost

void IvPProblem::solvePost()
{
  // Delete nodeBoxes here since solve may be invoked 
  // again later, and the number of objective functions may be 
  // different then.
  for(int i=0; (i < m_ofnum+1); i++)
    delete(nodeBox[i]);

  delete [] nodeBox;  
  nodeBox = 0;  
}


//---------------------------------------------------------------
// Procedure: upperTightBound
//   Purpose: 


#if 0
double IvPProblem::upperTightBound(int level, IvPBox *box) 
{
  double bound = 0.0;
  for(int i=level; (i < m_ofnum); i++)
    bound = bound + m_ofs[i]->getPDMap()->getGrid()->getTightBound(box);
  return(bound);
}
#endif

//---------------------------------------------------------------
// Procedure: upperCheapBound
//   Purpose: 

double IvPProblem::upperCheapBound(int level, IvPBox *box) 
{
  double bound = box->maxVal();

  for(int i=level; (i < m_ofnum); i++)
    bound += m_ofs[i]->getPDMap()->getGrid()->getCheapBound(box);

  return(bound);
}











