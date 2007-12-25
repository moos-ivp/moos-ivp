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
  this->setParam("build_info", "uniform_box=course:2,speed:2,tol:3");
  this->setParam("build_info", "uniform_grid=course:8,speed:6,tol:6");
}

//-----------------------------------------------------------
// Procedure: produceOF

OF *BHV_NULL3D::produceOF() 
{
  AOF_NULL3D *aof_NULL3D = new AOF_NULL3D(m_domain);
  OF_Reflector *ofrNULL3D = new OF_Reflector(aof_NULL3D, 1);

  ofrNULL3D->create(m_build_info);

  OF *of = ofrNULL3D->extractOF();

  delete(ofrNULL3D);

  of->setDomainName(0, "course");
  of->setDomainName(1, "speed");
  of->setDomainName(2, "tol");
  of->setPWT(m_priority_wt);

  return(of);
}






