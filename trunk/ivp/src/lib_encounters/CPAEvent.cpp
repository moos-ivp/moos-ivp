/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPAEvent.cpp                                         */
/*    DATE: Jan 11th 2016                                        */
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

#include <cstdlib>
#include "CPAEvent.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CPAEvent::CPAEvent()
{
  m_cpa = 0;
  m_eff = 0;
  m_x   = 0;
  m_y   = 0;
  m_id  = -1;

  m_alpha = -1;
  m_beta  = -1;
}

//---------------------------------------------------------
// Constructor

CPAEvent::CPAEvent(string v1, string v2, double cpa)
{
  m_vname1 = v1;
  m_vname2 = v2;

  m_cpa = cpa;
  m_eff = 0;
  m_x   = 0;
  m_y   = 0;
  m_id  = -1;

  m_alpha = -1;
  m_beta  = -1;
}

//---------------------------------------------------------
// Constructor (deserialize)

CPAEvent::CPAEvent(string spec)
{
  vector<string> svector = parseString(spec, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "v1")
      m_vname1 = value;
    else if(param == "v2")
      m_vname2 = value;
    else if(param == "cpa")
      m_cpa = atof(value.c_str());
    else if(param == "eff")
      m_eff = atof(value.c_str());
    else if(param == "x")
      m_x = atof(value.c_str());
    else if(param == "y")
      m_y = atof(value.c_str());
    else if(param == "id")
      m_id = atoi(value.c_str());
    else if(param == "alpha")
      m_alpha = atof(value.c_str());
    else if(param == "beta")
      m_beta = atof(value.c_str());
  }
}

//---------------------------------------------------------
// Procedure: getSpec()

string CPAEvent::getSpec() const
{
  string str = "cpa=" + doubleToStringX(m_cpa,2);
  if((m_x != 0) || (m_y != 0)) {
    str += ",x=" + doubleToStringX(m_x,2);
    str += ",y=" + doubleToStringX(m_y,2);
  }
  if(m_vname1 != "")
    str += ",v1=" + m_vname1;
  if(m_vname2 != "")
    str += ",v2=" + m_vname2;
  if(m_eff != 0)
    str += ",eff=" + doubleToStringX(m_eff,2);
  
  if(m_id != -1)
    str += ",id=" + uintToString(m_id);
  if(m_alpha > 0)
    str += ",alpha=" + doubleToStringX(m_alpha,3);
  if(m_alpha > 0)
    str += ",beta=" + doubleToStringX(m_beta,3);

  return(str);
}





