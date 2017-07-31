/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_Model.cpp                                       */
/*    DATE: May 9th, 2016                                        */
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




