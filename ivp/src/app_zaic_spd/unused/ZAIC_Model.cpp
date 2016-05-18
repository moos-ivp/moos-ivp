/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_Model.cpp                                       */
/*    DATE: May 9th, 2016                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "ZAIC_Model.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: getIvPDomain()

IvPDomain ZAIC_Model::getIvPDomain()
{
  IvPDomain null_domain;
  IvPFunction *ipf = getIvPFunction();

  if(!ipf) 
    return(null_domain);
  if(ipf->getPDMap() == 0)
    return(null_domain);
  if(ipf->getPDMap()->getDomain().size() == 0)
    return(null_domain);

  return(ipf->getPDMap()->getDomain());
}
