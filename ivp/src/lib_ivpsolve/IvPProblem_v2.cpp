/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPProblem_v2.cpp                                    */
/*    DATE: Too long ago to remember (1999-2001)                 */
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
#include "IvPProblem_v2.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: solve

bool IvPProblem_v2::solve(const IvPBox *b)
{
  solvePrior(0);

  PDMap *pdmap = m_ofs[0]->getPDMap();
  int boxCount = pdmap->size();

  for(int i=0; i<boxCount; i++) {
    nodeBox[1]->copy(pdmap->bx(i));
    solveRecurse(1);
  }    

  solvePost();
  if(!m_silent) {
    if(m_full_tree) 
      cout << "******* DONE IvPProblem::solveV1()" << endl;
    else
      cout << "******* DONE IvPProblem::solveV2()" << endl;
  }
  return(true);
}

//---------------------------------------------------------------
// Procedure: solveRecurse

void IvPProblem_v2::solveRecurse(int level)
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

  PDMap *pdmap = m_ofs[level]->getPDMap();

  int amt_level_plus1 = pdmap->size();
  for(int i=0; i<amt_level_plus1; i++) {
    result = nodeBox[level]->intersect(pdmap->bx(i), nodeBox[level+1]);
    if(m_full_tree || result)
      solveRecurse(level+1);
  }
}

