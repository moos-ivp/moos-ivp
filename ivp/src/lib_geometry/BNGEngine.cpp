/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BNGEngine.cpp                                        */
/*    DATE: Feb 29th 2016                                        */
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

#include <cmath> 
#include "BNGEngine.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

BNGEngine::BNGEngine()
{
  m_cnx = 0;
  m_cny = 0;
  m_cnv = 0;
  m_cnh = 0;
  m_osx = 0;
  m_osy = 0;
}

//----------------------------------------------------------
// Procedure: Constructor

BNGEngine::BNGEngine(double cnx, double cny, double cnh, double cnv,
		     double osx, double osy)
{
  m_cnx = cnx;
  m_cny = cny;
  m_cnh = cnh;
  m_cnv = cnv;
  m_osx = osx;
  m_osy = osy;
}


//----------------------------------------------------------
// Procedure: setStatic

void BNGEngine::setStatic()
{
}

//----------------------------------------------------------------
// Procedure: evalBNG()

double BNGEngine::evalBNG(double osh, double osv) const
{
  return(0);
}


//----------------------------------------------------------------
// Procedure: evalBNGRate()

double BNGEngine::evalBNGRate(double osh, double osv) const
{
  return(0);
}





