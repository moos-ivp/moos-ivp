/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Problem.cpp                                          */
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
#include <string.h> 
#include <assert.h>
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
  m_epsilon   = 0.0;
  m_silent    = true;
}

//---------------------------------------------------------------
// Procedure: Destructor

Problem::~Problem()
{
  if(m_maxbox)
    delete(m_maxbox);
  if(m_ofs) {
    for(int i=0; (i < m_ofnum); i++)
      delete(m_ofs[i]);
    delete[] m_ofs;
  }
}

//---------------------------------------------------------------
// Procedure: setDomain
//   Purpose: 

void Problem::setDomain(IvPDomain g_domain)
{
  m_domain = g_domain;
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
//   Purpose: 

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
//   Purpose: 

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
// Procedure: getDim
//   Purpose: 

int Problem::getDim() 
{
  if(m_ofnum == 0) 
    return(0);
  return(m_ofs[0]->getPDMap()->getDim());
}

//---------------------------------------------------------------
// Procedure: initialSolution1
//   Purpose: 

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
//   Purpose: 

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
    if(m_maxbox==0 || (weight > (m_maxwt + m_epsilon)))
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












