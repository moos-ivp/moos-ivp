/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CMAlert.cpp                                          */
/*    DATE: Oct 11th 2017                                        */
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

#include <iostream>
#include "XYFormatUtilsPoly.h"
#include "CMAlert.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

CMAlert::CMAlert()
{
  m_range = 0;
  m_range_far = 0;
}

//---------------------------------------------------------------
// Procedure: setAlertRange

bool CMAlert::setAlertRange(double dval)
{
  if(dval < 0)
    return(false);
  m_range = dval;
  return(true);
}

//---------------------------------------------------------------
// Procedure: setAlertRangeFar

bool CMAlert::setAlertRangeFar(double dval)
{
  if(dval < 0)
    return(false);
  m_range_far = dval;
  return(true);
}

//---------------------------------------------------------------
// Procedure: setAlertRangeColor

bool CMAlert::setAlertRangeColor(string str)
{
  if(!isColor(str))
    return(false);
  m_rng_color = str;
  return(true);
}

//---------------------------------------------------------------
// Procedure: setAlertRangeFarColor

bool CMAlert::setAlertRangeFarColor(string str)
{
  if(!isColor(str))
    return(false);
  m_rng_far_color = str;
  return(true);
}

//---------------------------------------------------------------
// Procedure: setAlertRegion

bool CMAlert::setAlertRegion(string str)
{
  XYPolygon poly = string2Poly(str);
  if(!poly.is_convex())
    return(false);
  
  m_region = poly;
  return(true);
}

//---------------------------------------------------------------
// Procedure: addAlertOnFlag

bool CMAlert::addAlertOnFlag(string str)
{
  string lft = biteStringX(str, '=');
  string rgt = str;

  if((lft == "") || (rgt == ""))
    return(false);

  VarDataPair pair(lft, rgt, "auto");
  m_on_flags.push_back(pair);
  return(true);    
}

//---------------------------------------------------------------
// Procedure: addAlertOffFlag

bool CMAlert::addAlertOffFlag(string str)
{
  string lft = biteStringX(str, '=');
  string rgt = str;

  if((lft == "") || (rgt == ""))
    return(false);
  
  VarDataPair pair(lft, rgt, "auto");
  m_off_flags.push_back(pair);
  return(true);    
}

//---------------------------------------------------------------
// Procedure: getAlertOnFlags

vector<VarDataPair> CMAlert::getAlertOnFlags() const
{
  return(m_on_flags);
}

//---------------------------------------------------------------
// Procedure: getAlertOffFlags

vector<VarDataPair> CMAlert::getAlertOffFlags() const
{
  return(m_off_flags);
}
