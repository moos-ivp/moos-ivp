/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPProblem.h                                         */
/*    DATE: Too long ago to remember (1996-1999)                 */
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
 
#ifndef IVPPROBLEM_HEADER
#define IVPPROBLEM_HEADER

#include "Problem.h"
#include "Compactor.h"

class IvPProblem: public Problem {
public:
  IvPProblem(Compactor *c=0);
  ~IvPProblem();

  void   preCompact();
  bool   solve(const IvPBox *isolbox=0);
  double getLeafsVisited() const {return(m_leafs_visited);}

protected:
  void   solvePrior(const IvPBox *b=0);
  void   solveRecurse(int);
  void   solvePost();
  double upperTightBound(int, IvPBox*);
  double upperCheapBound(int, IvPBox*);
  
protected:  
  IvPBox**   nodeBox;
  Compactor* compactor;
  bool       ownCompactor;

  double     m_leafs_visited;
};  

#endif




