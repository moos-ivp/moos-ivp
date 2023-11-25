/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WatchCast.cpp                                        */
/*    DATE: Dec 19th 2020                                        */
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

#include <cstdlib>
#include "MBUtils.h"
#include "WatchCast.h"

using namespace std;

//---------------------------------------------------------
// Constructor

WatchCast::WatchCast()
{
  m_loc_time = 0;
  m_utc_time = 0;
  m_dval = 0;
  m_sval_set = false;
  m_dval_set = false;
}

//---------------------------------------------------------
// Procedure: get_spec()

string WatchCast::get_spec() const
{
  string sep = "!X!"; // field separator
  
  string msg = "node=" + m_node;
  msg += sep +"var=" + m_varname;
  
  if(m_source != "")
    msg += sep + "src=" + m_source;
  if(m_community != "")
    msg += sep + "comm=" + m_community;

  msg += sep + "loc_time=" + doubleToStringX(m_loc_time,2);
  msg += sep + "utc_time=" + doubleToStringX(m_utc_time,2);

  if(m_dval_set)
    msg += sep + "dval=" + doubleToStringX(m_dval,3);
  else
    msg += sep + "sval=" + m_sval;

  return(msg);
}

//---------------------------------------------------------
// Procedure: valid()

bool WatchCast::valid() const
{
  if(m_sval_set == m_dval_set)
    return(false);
  if(m_node == "")
    return(false);
  if(m_source == "")
    return(false);
  if(m_community == "")
    return(false);

  return(true);
}


//---------------------------------------------------------
// Procedure: string2WatchCast()

WatchCast string2WatchCast(std::string str)
{
  string sep = "!X!"; // field separator
  char c_sep = 30;    // ASCII 30 is "Record Separator"
  string str_sep(1,c_sep);
  
  str = findReplace(str, sep, str_sep);
  
  WatchCast null_rcvar;
  WatchCast good_rcvar;

  vector<string> svector = parseString(str, c_sep);
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if(param == "node") 
      good_rcvar.setNode(value);
    else if(param == "var") 
      good_rcvar.setVarName(value);
    else if(param == "src") 
      good_rcvar.setSource(value);
    else if(param == "comm") 
      good_rcvar.setCommunity(value);
    else if(param == "sval") 
      good_rcvar.setSVal(value);
    else if(param == "dval") 
      good_rcvar.setDVal(atof(value.c_str()));
    else if(param == "loc_time") 
      good_rcvar.setLocTime(atof(value.c_str()));
    else if(param == "utc_time") 
      good_rcvar.setUtcTime(atof(value.c_str()));
    else
      return(null_rcvar);
  }
			 
  if(!good_rcvar.valid())
    return(null_rcvar);
    
  return(good_rcvar);
}

