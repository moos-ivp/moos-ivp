/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeRider.cpp                                        */
/*    DATE: May 7th 2022                                         */
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

#include "NodeRider.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

NodeRider::NodeRider()
{
  m_frequency = 0;
  m_last_utc  = 0;
  m_fresh     = false;

  m_total_updates = 0;
}


//------------------------------------------------------------
// Procedure: setVar()

bool NodeRider::setVar(std::string var)
{
  if((var == "NAME")   || (var == "X")    || (var == "Y")    ||
     (var == "SPD")    || (var == "HDG")  || (var == "DEP")  ||
     (var == "LAT")    || (var == "LON")  || (var == "TYPE") ||
     (var == "COLOR")  || (var == "MODE") || (var == "ALLSTOP") ||
     (var == "INDEX")  || (var == "YAW")  || (var == "TIME")   ||
     (var == "LENGTH"))
    return(false);
  
  m_moosvar = var;
  return(true);
}

//------------------------------------------------------------
// Procedure: setRiderField()

bool NodeRider::setRiderFld(string fld)
{
  if(!isAlphaNum(fld))
    return(false);

  m_rider_fld = fld;
  return(true);
}

//------------------------------------------------------------
// Procedure: setPolicyConfig()

bool NodeRider::setPolicyConfig(std::string config_str)
{
  if(config_str == "always") {
    m_policy = "always";
    return(true);
  }

  if(config_str == "updated") {
    m_policy = "updated";
    m_frequency = -1;
    return(true);
  }

  if(strBegins(config_str, "updated+")) {
    biteStringX(config_str, '+');
    if(isNumber(config_str)) {
      double freq = atof(config_str.c_str());
      if(freq >= 0) {
	m_policy = "updated";
	m_frequency = freq;
	return(true);
      }
    }
  }

  if(isNumber(config_str)) {
    double freq = atof(config_str.c_str());
    if(freq >= 0) {
      m_policy = "freq";
      m_frequency = freq;
      return(true);
    }
  }

  return(false);
}


//---------------------------------------------------------------
// Procedure: updateValue()

bool NodeRider::updateValue(string curr_value, double utc)
{
  m_curr_value = curr_value;
  m_last_utc   = utc;
  m_fresh      = true;

  m_total_updates++;
  
  return(true);
}

//---------------------------------------------------------------
// Procedure: setPolicy()

bool NodeRider::setPolicy(string str)
{
  if((str == "always") || (str == "updated") || (str == "freq"))
    m_policy = str;
  else
    return(false);

  return(true);
}

//---------------------------------------------------------------
// Procedure: setFrequency()

bool NodeRider::setFrequency(double dval)
{
  if(dval >= 0)
    m_frequency = dval;
  else
    return(false);

  return(true);
}

//---------------------------------------------------------------
// Procedure: valid()

bool NodeRider::valid() const
{
  if((m_moosvar == "") || (m_policy == ""))
    return(false);

  return(true);
}


//---------------------------------------------------------------
// Procedure: getSpec()
//      Note: always, udpated, updated+10, 10

string NodeRider::getSpec() const
{
  if(!valid())
    return("");

  string str = "var=" + m_moosvar;

  if(m_policy == "freq")
    str += ", policy=" + doubleToStringX(m_frequency,2);
  else if((m_policy == "updated") && (m_frequency > 0))
    str += ", policy=updated+" + doubleToStringX(m_frequency,2);
  else
    str += ", policy=" + m_policy;

  if(m_rider_fld != "")
    str += ", rfld=" + m_rider_fld;
  
  return(str);
}

