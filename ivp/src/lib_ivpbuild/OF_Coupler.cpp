/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OF_Coupler.cpp                                       */
/*    DATE: July 1st 2006                                        */
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

#include <vector>
#include "OF_Coupler.h"
#include "IvPFunction.h"
#include "BuildUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: couple

IvPFunction *OF_Coupler::couple(IvPFunction* ipf1, 
				IvPFunction* ipf2)
{
  if((ipf1==0) || (ipf2==0))
    return(0);

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
    








