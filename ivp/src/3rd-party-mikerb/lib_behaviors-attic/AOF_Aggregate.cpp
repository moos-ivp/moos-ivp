/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Aggregate.cpp                                    */
/*    DATE: May 10th, 2005                                       */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include <assert.h>
#include "AOF_Aggregate.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: addAOF
//   Purpose: 

bool AOF_Aggregate::addAOF(AOF* new_aof, float new_weight) 
{
  // Special case - when handling the first AOF
  if(aofs.size() == 0) {
    aofs.push_back(new_aof);
    weights.push_back(new_weight);
    norm_weights.push_back(1.0);
    domain = new_aof->getDomain();
    return(true);
  }

  // When adding additional AOF's first make sure that the
  // universes are identical with each other.

  assert(aofs[0]->getDim() == new_aof->getDim());
  
  aofs.push_back(new_aof);
  weights.push_back(new_weight);
  norm_weights.push_back(0.0);

  double total_weight = 0;
  for(int i=0; i<weights.size(); i++)
    total_weight += weights[i];

  for(int j=0; j<weights.size(); j++)
    norm_weights[j] = weights[j] / total_weight;

  return(true);
}







