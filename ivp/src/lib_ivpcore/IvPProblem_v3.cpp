/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPProblem_v3.cpp                                    */
/*    DATE: Too long ago to remember (1999-2001)                 */
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
#include "IvPProblem_v3.h"

//---------------------------------------------------------------
// Procedure: solve

bool IvPProblem_v3::solve(const IvPBox *b)
{
  solvePrior(0);

  PDMap *pdmap = m_ofs[0]->getPDMap();
  int boxCount = pdmap->size();

  for(int i=0; i<boxCount; i++) {
    nodeBox[1]->copy(pdmap->bx(i));
    solveRecurse(1);
  }    

  solvePost();
  return(true);
}

//---------------------------------------------------------------
// Procedure: solveRecurse

void IvPProblem_v3::solveRecurse(int level)
{
  int result;

  if(level == m_ofnum) {                       // boundary condition
    bool ok = false;
    float currWT = compactor->maxVal(nodeBox[level], &ok);
    if((m_maxbox==NULL) || (currWT > (m_maxwt + m_epsilon)))
      newSolution(currWT, nodeBox[level]);
    return;
  }

  IvPGrid *grid = m_ofs[level]->getPDMap()->getGrid();
  assert(grid != 0);
  BoxSet *levelBoxes = grid->getBS(nodeBox[level]);
  BoxSetNode *levBSN = levelBoxes->retBSN(FIRST);

  while(levBSN != NULL) {
    BoxSetNode *nextLevBSN = levBSN->getNext();

    IvPBox *cbox = levBSN->getBox();
    result = nodeBox[level]->intersect(cbox, nodeBox[level+1]);
    
    if(result)
      solveRecurse(level+1);

    levBSN = nextLevBSN;
  }
  delete(levelBoxes);
}









