/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ProblemIO.cpp                                        */
/*    DATE: June 2nd 2004                                        */
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

#include "ProblemIO.h"
#include "IO_Utilities.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: saveProblem
//   Purpose: 

bool saveProblem(IvPProblem *prob, const string& fileStr)
{
  bool okw = true;  // fprint returns neg # on error, or num chars outp

  for(int i=0; i<prob->getOFNUM(); i++)
    okw = okw && saveFunction(prob->getOF(i), fileStr, true);

  return(okw);
}
