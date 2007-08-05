/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_NULL3D.cpp                                       */
/*    DATE: March 22, 2005                                       */
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

#include <assert.h> 
#include <string.h>
#include "IO_Utilities.h"
#include "BHV_NULL3D.h"
#include "AOF_NULL3D.h"
#include "OF_Reflector.h"
#include "IvPDomain.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_NULL3D::BHV_NULL3D(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setDescriptor("NULL3D");
  this->setParam("unifbox", "course=2, speed=2, tol=3");
  this->setParam("gridbox", "course=8, speed=6, tol=6");
}

//-----------------------------------------------------------
// Procedure: produceOF

OF *BHV_NULL3D::produceOF() 
{
  // clear each time produceOF() is called
  messages.clear();

  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }

  AOF_NULL3D *aof_NULL3D = new AOF_NULL3D(domain);
  OF_Reflector *ofrNULL3D = new OF_Reflector(aof_NULL3D, 1);

  ofrNULL3D->createUniform(unif_box, grid_box);

  OF *of = ofrNULL3D->extractOF();

  delete(ofrNULL3D);

  of->setDomainName(0, "course");
  of->setDomainName(1, "speed");
  of->setDomainName(2, "tol");
  of->setPWT(priority_wt);

  return(of);
}






