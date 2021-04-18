/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPFunction.cpp                                      */
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

#include <cassert>
#include <cstdio>
#include <cstring>
#include "IvPFunction.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

IvPFunction::IvPFunction(PDMap *g_pdmap)
{
  assert(g_pdmap);

  m_pdmap = g_pdmap;
  m_pwt   = 10.0;
}

//-------------------------------------------------------------
// Procedure: Destructor

IvPFunction::~IvPFunction()
{
  if(m_pdmap) 
    delete(m_pdmap);
}

//-------------------------------------------------------------
// Procedure: setPWT
//   Purpose:

void IvPFunction::setPWT(double g_pwt)
{
  if(g_pwt >= 0.0)
    m_pwt = g_pwt;
}

//-------------------------------------------------------------
// Procedure: transDomain

bool IvPFunction::transDomain(IvPDomain gdomain)
{
  if(m_pdmap->getDomain() == gdomain)
    return(true);

  // First measure the sizes of the given domain and the 
  // number of dimensions for this objective function. Ensure
  // the number of dimensions in this function is not greater.
  int dom_dim = gdomain.size();
  int of_dim  = m_pdmap->getDim();
  if(of_dim > dom_dim)
    return(false);

  int i;

  // Now build a translation map. Ensure that all domain names
  // in this function are found in the given IvPDomain.
  bool ok = true;
  int *dmap = new int[of_dim]; 
  for(i=0; i<of_dim; i++) {
    string i_dom = this->getVarName(i);
    dmap[i] = gdomain.getIndex(i_dom);
    if(dmap[i] == -1)
      ok = false;
  }

  // Now perform the domain translation on the pdmap, check
  // the result and quit now if it failed.
  ok = ok && m_pdmap->transDomain(gdomain, dmap);

  // Clean up temp memory from the heap
  delete [] dmap;

  return(ok);
}

//-------------------------------------------------------------
// Procedure: getVarName()

string IvPFunction::getVarName(int i)
{
  if(m_pdmap)
    return(m_pdmap->getDomain().getVarName(i));
  return("");
}

//-------------------------------------------------------------
// Procedure: getGridConfig()

string IvPFunction::getGridConfig() const
{
  if(m_pdmap)
    return(m_pdmap->getGridConfig());
  return("");
}


//-------------------------------------------------------------
// Procedure: copy
//   Purpose: Make a deep copy of the IvP function

IvPFunction *IvPFunction::copy() const
{
  PDMap *pdmap = new PDMap(m_pdmap);
  
  IvPFunction *ipf = new IvPFunction(pdmap);
  ipf->setPWT(m_pwt);
  ipf->setContextStr(m_context_string);

  return(ipf);
}







