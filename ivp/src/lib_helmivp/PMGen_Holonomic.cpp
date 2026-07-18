/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen_Holonomic.cpp                                  */
/*    DATE: Oct 24th, 2023                                       */
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

#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "PMGen_Holonomic.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: generate()

PlatModel PMGen_Holonomic::generate(double osx, double osy,
				    double osh, double osv)
{
  PlatModel pmodel("holo");

  pmodel.setPose(osx, osy, osh, osv);
  pmodel.setModelType("holo");
  
  return(pmodel);
}
