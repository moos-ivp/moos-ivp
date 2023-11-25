/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: OF_Coupler.cpp                                       */
/*    DATE: July 1st 2006                                        */
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

#include <vector>
#include "OF_Coupler.h"
#include "IvPFunction.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

OF_Coupler::OF_Coupler()
{
  m_normalize = true;
  m_normalmin = 0;
  m_normalmax = 100;
}

//-------------------------------------------------------------
// Procedure: disableNormalize

void OF_Coupler::disableNormalize()
{
  m_normalize = false;
}

//-------------------------------------------------------------
// Procedure: enableNormalize

void OF_Coupler::enableNormalize(double min, double max)
{
  if(min < max) {
    m_normalize = true;
    m_normalmin = min;
    m_normalmax = max;
  }
}

//-------------------------------------------------------------
// Procedure: couple

IvPFunction *OF_Coupler::couple(IvPFunction* ipf1, IvPFunction* ipf2)
{
  return(couple(ipf1, ipf2, 50, 50));    
}

//-------------------------------------------------------------
// Procedure: couple

IvPFunction *OF_Coupler::couple(IvPFunction* ipf1, IvPFunction* ipf2, 
				double wt1, double wt2)
{
  bool ok = true;
  if((ipf1==0) || (ipf2==0))
    ok = false;
  if((wt1 <= 0) || (wt2 <= 0))
    ok = false;
  if(!ok) {
    if(ipf1)
      delete(ipf1);
    if(ipf2)
      delete(ipf2);
    return(0);
  }
  
  ipf1->getPDMap()->normalize(0, wt1);
  ipf2->getPDMap()->normalize(0, wt2);

  IvPFunction *ipf = coupleRaw(ipf1, ipf2);
  if(ipf && m_normalize)
    ipf->getPDMap()->normalize(m_normalmin, m_normalmax);

  return(ipf);    
}

//-------------------------------------------------------------
// Procedure: coupleRaw

IvPFunction *OF_Coupler::coupleRaw(IvPFunction* ipf1, 
				   IvPFunction* ipf2)
{
  if((ipf1==0) || (ipf2==0)) {
    if(ipf1)
      delete(ipf1);
    if(ipf2)
      delete(ipf2);
    return(0);
  }

  int degree1 = ipf1->getPDMap()->getDegree();
  int degree2 = ipf2->getPDMap()->getDegree();
  if(degree1 != degree2)
    return(0);

  IvPDomain domain1 = ipf1->getPDMap()->getDomain();
  IvPDomain domain2 = ipf2->getPDMap()->getDomain();

  int i,j;

  if(intersectDomain(domain1, domain2))
     return(0);

  IvPDomain coup_domain = unionDomain(domain1, domain2);
  
  ipf1->transDomain(coup_domain);
  ipf2->transDomain(coup_domain);

  int pc1 = ipf1->getPDMap()->size();
  int pc2 = ipf2->getPDMap()->size();

  vector<IvPBox*> pieces;
  
  bool ok = true;
  for(i=0; i<pc1; i++) {
    IvPBox *ipiece = ipf1->getPDMap()->bx(i);

    for(j=0; j<pc2; j++) {
      IvPBox *jpiece = ipf2->getPDMap()->bx(j);
      IvPBox *new_piece = 0;
      ok = ok && ipiece->intersect(jpiece, new_piece);
      if(ok && (new_piece != 0))
	pieces.push_back(new_piece);
    }
  }
  
  int new_cnt = pieces.size();

  PDMap *pdmap = new PDMap(new_cnt, coup_domain, degree1);
  
  for(i=0; i<new_cnt; i++)
    pdmap->bx(i) = pieces[i];
  
  IvPFunction *new_ipf = new IvPFunction(pdmap);
  
  delete(ipf1);
  delete(ipf2);

  return(new_ipf);
}
    









