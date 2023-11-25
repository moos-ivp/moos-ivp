/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IvPProblem_v2.h                                      */
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
 
#ifndef IVPPROBLEM_V2_HEADER
#define IVPPROBLEM_V2_HEADER

#include "IvPProblem.h"

class IvPProblem_v2: public IvPProblem {
public:
  IvPProblem_v2() {m_full_tree=false;}
  ~IvPProblem_v2() {}

  bool solve(const IvPBox *isolbox=0);
  void solveRecurse(int);

  void setFullTreeTraversal() {m_full_tree=true;}

 private:
  bool m_full_tree;
  
};  

#endif




















