/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPF_Entry.cpp                                        */
/*    DATE: Apr 20th, 2011                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include "IPF_Entry.h"
#include "FunctionEncoder.h"
#include "IPFViewUtils.h"
#include "MBUtils.h"
#include "IPF_Utils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

IPF_Entry::IPF_Entry(const string& ipf_str)
{
  m_ipf_str  = ipf_str;
  m_pieces   = 0;
  m_priority = 0;
}

//-------------------------------------------------------------
// Procedure: getIvPFunction()

IvPFunction* IPF_Entry::getIvPFunction()
{
  IvPFunction *ipf = StringToIvPFunction(m_ipf_str);

  if(ipf) {
    m_pieces = ipf->getPDMap()->size();
    m_priority = ipf->getPWT();
  }

  return(ipf);
}

//-------------------------------------------------------------
// Procedure: getQuadSet()
//      Note: The quadset member variable will remain unpopulated
//            until the first time it is queried for.

QuadSet IPF_Entry::getQuadSet(IvPDomain ivp_domain, bool dense)
{
  if(m_quadset.size() == 0) {
    IvPFunction *new_ipf = StringToIvPFunction(m_ipf_str);
    if(new_ipf) {
      m_ivp_domain = new_ipf->getPDMap()->getDomain();
      m_pieces     = new_ipf->getPDMap()->size();
      m_priority   = new_ipf->getPWT();
      new_ipf = expandHdgSpdIPF(new_ipf, ivp_domain);

      m_quadset = buildQuadSetFromIPF(new_ipf, dense);
      delete(new_ipf);
    }
  }
  return(m_quadset);
}





