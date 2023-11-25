/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IvPProblem_v3.cpp                                    */
/*    DATE: Too long ago to remember (1999-2001)                 */
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
#include <cassert>
#include "IvPProblem_v3.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: solve

bool IvPProblem_v3::solve(const IvPBox *b)
{
  if(!m_silent)
    cout << "******* Entering IvPProblem::solveV3()" << endl;
  
  solvePrior(0);

  PDMap *pdmap = m_ofs[0]->getPDMap();
  int boxCount = pdmap->size();

  for(int i=0; i<boxCount; i++) {
    nodeBox[1]->copy(pdmap->bx(i));
    solveRecurse(1);
  }    

  solvePost();

  if(!m_silent)
    cout << "******* DONE IvPProblem::solveV3()" << endl;
    
  return(true);
}

//---------------------------------------------------------------
// Procedure: solveRecurse

void IvPProblem_v3::solveRecurse(int level)
{
  int result;

  if(level == m_ofnum) {                       // boundary condition
    m_leafs_visited++;
    bool ok = false;
    float currWT = compactor->maxVal(nodeBox[level], &ok);
    if((m_maxbox==NULL) || (currWT > m_maxwt))
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



















