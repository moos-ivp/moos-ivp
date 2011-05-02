/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: IPF_Entry.cpp                                        */
/*    DATE: Apr 20th, 2011                                       */
/*****************************************************************/

#include <iostream>
#include "IPF_Entry.h"
#include "FunctionEncoder.h"
#include "IPFViewUtils.h"

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

QuadSet IPF_Entry::getQuadSet(IvPDomain ivp_domain)
{
  if(m_quadset.size() == 0) {
    IvPFunction *new_ipf = StringToIvPFunction(m_ipf_str);
    if(new_ipf) {
      m_ivp_domain = new_ipf->getPDMap()->getDomain();
      m_pieces     = new_ipf->getPDMap()->size();
      m_priority   = new_ipf->getPWT();
      new_ipf = expandHdgSpdIPF(new_ipf, ivp_domain);
      m_quadset.applyIPF(new_ipf);
      delete(new_ipf);
    }
  }
  return(m_quadset);
}

