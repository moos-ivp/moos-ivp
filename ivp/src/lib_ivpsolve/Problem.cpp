/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Problem.cpp                                          */
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

#include <iostream>
#include <cstring> 
#include <cassert>
#include "Problem.h"
#include "IvPBox.h"
#include "IvPFunction.h"
#include "IvPDomain.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: Constructor

Problem::Problem()
{
  m_maxbox    = 0;
  m_ofnum     = 0;
  m_ofs       = 0;
  m_silent    = true;
  m_owner_ofs = true;

  m_epsilon   = 0.0;
  m_thresh    = 100.0;  // By default find global max
}

//---------------------------------------------------------------
// Procedure: Destructor

Problem::~Problem()
{
  if(m_maxbox)
    delete(m_maxbox);
  if(m_ofs && m_owner_ofs) {
    for(int i=0; (i < m_ofnum); i++)
      delete(m_ofs[i]);
    delete[] m_ofs;
  }
}

//---------------------------------------------------------------
// Procedure: setDomain

void Problem::setDomain(IvPDomain g_domain)
{
  m_domain = g_domain;
}

//---------------------------------------------------------------
// Procedure: setThresh
//   Purpose: o Set the threshold for branching in terms of guaratees
//              with respect of the globally optimal solution.
//            o By default the threshold is 100, meaning the returned
//              solution is guaranteed to be globally optimal.
//            o A setting of 90 indicates the returned solution is at
//              least 90% of the globally optimal.
//            o The settings for this parameter are in the range [0,100]
//              Out of range values are clipped with no warning.
//            o This value determines the "epsilon" value. The epsilon
//              values is based on the current max solution and thresh.

void Problem::setThresh(double thresh)
{
  m_thresh = thresh;
  if(m_thresh < 0)
    m_thresh = 0;
  if(m_thresh > 100)
    m_thresh = 100;
}

//---------------------------------------------------------------
// Procedure: clearIPFs
//   Purpose: A way to explicitly release the memory used by the 
//            IvP functions. This may also be done by the destructor,
//            but not necessarily, depending on how m_owner_ofs is
//            set.

void Problem::clearIPFs()
{
  if(m_ofs) {
    for(int i=0; (i < m_ofnum); i++)
      delete(m_ofs[i]);
    delete[] m_ofs;
  }
  m_ofs = 0;
}


//---------------------------------------------------------------
// Procedure: addOF
//   Purpose: Key for building Problems - regardless of whether from
//            a file or any other source such as behaviors.
//      NOTE: Applies the priority weight of the given objective 
//            function to itself. Previously done in 
//            Problem::prepPWT().

void Problem::addOF(IvPFunction *gof)
{
  if(gof==0) return;

  // Dont allow OFs to be part of the problem unless they have a 
  // positive priority weight.
  if(gof->getPWT() <= 0) return;

  double range = gof->getPDMap()->getMaxWT() - gof->getPDMap()->getMinWT();
  if(range > 100)
    gof->getPDMap()->normalize(0,100);

  // Apply the priority weight to the OF
  gof->getPDMap()->applyWeight(gof->getPWT());


  IvPFunction** newOFs = new IvPFunction*[m_ofnum+1];
  for(int i=0; (i < m_ofnum); i++)
    newOFs[i] = m_ofs[i];
  newOFs[m_ofnum] = gof;

  if(m_ofs) 
    delete [] m_ofs;
  m_ofs = newOFs;
  m_ofnum++;
}

//---------------------------------------------------------------
// Procedure: alignOFs

bool Problem::alignOFs()
{
  bool ok = false;

  for(int i=0; (i < m_ofnum); i++) {
    ok = m_ofs[i]->transDomain(m_domain);
    if(!ok)
      return(false);
  }

  assert(universesInSync());
  return(ok);
}



//---------------------------------------------------------------
// Procedure: getResult

double Problem::getResult(const string& varname, bool *ok_query)
{
  double result_val = 0;
  bool   result_ok  = false;

  if(m_maxbox) {
    IvPBox maxpt = m_maxbox->maxPt();
    int index = m_domain.getIndex(varname);
    if(index >= 0)
      result_ok = m_domain.getVal(index, maxpt.pt(index,0), result_val);
  }

  if(ok_query)
    *ok_query = result_ok;
  return(result_val);
}
  
//---------------------------------------------------------------
// Procedure: getResultVal

double Problem::getResultVal() const
{
  if(m_maxbox)
    return(m_maxbox->maxVal());
  return(0);
}
  
//---------------------------------------------------------------
// Procedure: getPieceAvg

double Problem::getPieceAvg() const
{
  if(m_ofnum == 0)
    return(0);

  double total = 0;
  for(int i=0; i<m_ofnum; i++) 
    total += (double)(m_ofs[i]->size());

  return(total / (double)(m_ofnum));
}

//---------------------------------------------------------------
// Procedure: getOF
//   Purpose: Return a pointer to the objective function at the
//            given index. Return NULL if out of range.

IvPFunction* Problem::getOF(int ix)
{
  if(ix >= 0)
    if(ix < m_ofnum)
      return(m_ofs[ix]);
  return(0);
}

//---------------------------------------------------------------
// Procedure: getGridConfig
//   Purpose: Get a description of the grid of the first function

string Problem::getGridConfig(int ix) const
{
  if((ix >= 0) && (ix < m_ofnum))
    return(m_ofs[ix]->getGridConfig());
  return("");
}

//---------------------------------------------------------------
// Procedure: getDim

int Problem::getDim() 
{
  if(m_ofnum == 0) 
    return(0);
  return(m_ofs[0]->getPDMap()->getDim());
}

//---------------------------------------------------------------
// Procedure: initialSolution1

void Problem::initialSolution1()
{
  if(m_ofnum == 0)
    return;

  int    highOF  = 0;
  double highPWT = m_ofs[0]->getPWT();

  for(int i=1; (i < m_ofnum); i++) {
    double iPWT = m_ofs[i]->getPWT();
    if(iPWT > highPWT) {
      highOF  = i;
      highPWT = iPWT;
    }
  }

  IvPGrid *grid = m_ofs[highOF]->getPDMap()->getGrid();
  if(grid && !grid->isEmpty()) {
    IvPBox maxpt = grid->getMaxPt();
    this->processInitSol(&maxpt);
  }
}

//---------------------------------------------------------------
// Procedure: initialSolution2

void Problem::initialSolution2()
{
  if(m_ofnum == 0)
    return;

  for(int i=1; (i < m_ofnum); i++) {
    IvPGrid *grid = m_ofs[i]->getPDMap()->getGrid();
    if(grid && !grid->isEmpty()) {
      IvPBox maxpt = grid->getMaxPt();
      this->processInitSol(&maxpt);
    }
  }
}

//---------------------------------------------------------------
// Procedure: sortOFs
//   Purpose: Sort the objective functions based on priority wt.
//            Higher priority functions should have a lower index.
//            No real effort here on making this fast.

void Problem::sortOFs(bool high_to_low)
{
  for(int i=0; (i < m_ofnum); i++) {
    int    best_ix  = i;
    double best_pwt = m_ofs[i]->getPWT();
    for(int j=i+1; (j < m_ofnum); j++) {
      double j_pwt = m_ofs[j]->getPWT();
      if(high_to_low) {
	if(j_pwt > best_pwt) {
	  best_pwt = j_pwt;
	  best_ix = j;
	}
      }
      else {
	if(j_pwt < best_pwt) {
	  best_pwt = j_pwt;
	  best_ix = j;
	}
      }
    }
    if(best_ix != i) {
      IvPFunction *temp = m_ofs[i];
      m_ofs[i] = m_ofs[best_ix];
      m_ofs[best_ix] = temp;
    }
  }
}

//---------------------------------------------------------------
// Procedure: processInitSol(Box *isolBox)
//   Purpose: Process an initial solution given by the (point) box
//            isolBox. The idea is that this solution, for some 
//            reason known to the IvP user, is believed to be 
//            competitive with the final solution. So its evaluated
//            before the full search begins to start with a better 
//            bound to make pruning more likely in its initial 
//            stages.

void  Problem::processInitSol(const IvPBox *isolBox)
{
  if(isolBox->isPtBox() != true)
    return;

  double weight  = 0.0;
  bool   covered = true;
  for(int i=0; i<m_ofnum; i++)
    if(covered) 
      weight += m_ofs[i]->getPDMap()->evalPoint(isolBox, &covered);
  
  if(!m_silent) 
    cout << "initial solution weight: " << weight << endl;
  if(covered)
    if(m_maxbox==0 || (weight > m_maxwt))
      newSolution(weight, isolBox);
}


//---------------------------------------------------------------
// Procedure: newSolution

void Problem::newSolution(double newMaxWT, const IvPBox *newMaxBox)
{
  assert(newMaxBox);

  if(!m_maxbox) 
    m_maxbox = newMaxBox->copy();
  else
    m_maxbox->copy(newMaxBox);
  m_maxwt = newMaxWT;

  if(!m_silent) {
    cout << "New Max Weight: " << m_maxwt << endl;
    m_maxbox->print();
  }

  // If the threshold is not 100, then we will adjust the epsilon
  // dynamically as the solution changes.
  if(m_thresh != 100) 
    m_epsilon = ((100.0 / m_thresh) * m_maxwt) - m_maxwt;
}

//---------------------------------------------------------------
// Procedure: universesInSync
//   Purpose: Check if universes from each objective function 
//              sufficiently match.
//      Note: The universe box from each of their pdmaps must be
//              have the same number of dimensions.
//      Note: The domain name string associated with each OF, 
//              must also match for each dimension.
//      Note: The domain extents to not need to match. The overall
//              feasible space of the problem is the intersection 
//              of universes. An objective function that does not
//              map a part of the domain is, in effect, declaring
//              that space to be infeasible.

bool Problem::universesInSync()
{
  if(m_ofnum <= 1)     // Need > two OF's to possibly be out of sync
    return(true);    

  int i, d;
  int dim = m_ofs[0]->getPDMap()->getUniverse().getDim();

  for(i=1; (i < m_ofnum); i++) {
    int idim = m_ofs[i]->getPDMap()->getUniverse().getDim();
    if(idim != dim)
      return(false);
    for(d=0; d<dim; d++) {
      if(m_ofs[0]->getVarName(d) !=  m_ofs[i]->getVarName(d))
      	return(false);
    }
  }
  return(true);
}






















