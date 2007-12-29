/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_TOL.cpp                                          */
/*    DATE: March 27th, 2005                                     */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include "IO_Utilities.h"
#include "BHV_TOL.h"
#include "AOF_TOL.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_TOL::BHV_TOL(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_tol");
  this->setParam("build_info", "uniform_box=tol:2");
  this->setParam("build_info", "uniform_grid=tol:4");

  m_domain = subDomain(m_domain, "tol");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_TOL::setParam(string g_param, string g_val) 
{
  if(IvPBehavior::setParam(g_param, g_val))
    return(true);
  else
    return(false);
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_TOL::onRunState() 
{
  AOF_TOL aof_tol(m_domain);
  OF_Reflector reflector(&aof_tol, 1);

  reflector.create(m_build_info);

  IvPFunction *of = reflector.extractOF();

  of->setPWT(m_priority_wt);

  return(of);
}








