/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPAEngineB.cpp                                       */
/*    DATE: June 12th 2017                                       */
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

#include <iostream>
#include <cmath> 
#include "CPAEngineRoot.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor
//      args: cny  Given Contact Y Position
//      args: cnx  Given Contact X Position
//      args: cnh  Given Contact Course
//      args: cnv  Given Contact Speed
//      args: osy  Given Ownship Y Position
//      args: osx  Given Ownship X Position

CPAEngineRoot::CPAEngineRoot(double cny, double cnx, double cnh,
			     double cnv, double osy, double osx)
{
  CPAEngineRoot::reset(cny, cnx, cnh, cnv, osy, osx);
  m_openings = 0;
  m_openings_early = 0;
}

//----------------------------------------------------------
// Procedure: reset

void CPAEngineRoot::reset(double cny, double cnx, double cnh,
			  double cnv, double osy, double osx)
{
  m_cny = cny; 
  m_cnx = cnx;
  m_cnv = cnv;
  m_cnh = angle360(cnh);
  m_osx = osx;
  m_osy = osy;

  if(m_cnv < 0)
    m_cnv = 0;
}




