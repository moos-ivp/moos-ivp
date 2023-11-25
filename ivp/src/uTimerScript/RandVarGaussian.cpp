/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RandVarGaussian.cpp                                  */
/*    DATE: May 20th 2012                                        */
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
#include "RandVarGaussian.h"
#include "MBUtils.h"
#include "MOOS/libMOOS/MOOSLib.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RandVarGaussian::RandVarGaussian() : RandomVariable()
{
  m_sigma = 1;
  m_mu    = 0;
}


//---------------------------------------------------------
// Procedure: setParam

bool RandVarGaussian::setParam(string param, double value)
{
  bool handled = RandomVariable::setParam(param, value);
  if(handled)
    return(true);

  if(param == "sigma")
    m_sigma = value;
  else if(param == "mu") 
    m_mu = value;
  else
    return(false);
  
  return(true);
}

//---------------------------------------------------------
// Procedure: reset

void RandVarGaussian::reset()
{
  double noise = MOOSWhiteNoise(m_sigma);
  m_value = m_mu + noise;
  m_value_str = "";

  m_value = snapToStep(m_value, m_snap);
  if(m_value > m_max_val)
    m_value = m_max_val;
  if(m_value < m_min_val)
    m_value = m_min_val;

  m_value_str = doubleToStringX(m_value,5);
}
  
//---------------------------------------------------------
// Procedure: getStringSummary

string RandVarGaussian::getStringSummary() const
{
  string str = RandomVariable::getStringSummary();
  if(str != "")
    str += ",";

  str += "sigma=" + doubleToStringX(m_sigma);
  str += ",mu=" + doubleToStringX(m_mu);
  str += ",type=gaussian";
  return(str);
}
  
//---------------------------------------------------------
// Procedure: getParams

string RandVarGaussian::getParams() const
{
  string str = "sigma=" + doubleToStringX(m_sigma);
  str += ", mu=" + doubleToStringX(m_mu);
  return(str);
}
  










