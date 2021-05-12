/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarDataPair.cpp                                      */
/*    DATE: Jul 4th 2005 Monday morning at Brueggers             */
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
#include "VarDataPair.h"
#include "MBUtils.h"
#include "MacroUtils.h"

using namespace std;

//------------------------------------------------------------------
// Procedure: constructor

VarDataPair::VarDataPair()
{
  m_var       = "";
  m_sdata     = "";
  m_ddata     = 0;
  m_is_string = false;
  m_is_quoted = false;

  m_var_set   = false;
  m_key_set   = false;
  m_ptype_set = false;
  m_ddata_set = false;
  m_sdata_set = false;
}

//------------------------------------------------------------------
// Procedure: constructor

VarDataPair::VarDataPair(string var, double ddata)
{
  m_var       = stripBlankEnds(var);
  m_sdata     = "";
  m_ddata     = ddata;
  m_is_string = false;
  m_is_quoted = false;

  m_var_set   = true;
  m_key_set   = false;
  m_ptype_set = false;
  m_ddata_set = true;
  m_sdata_set = false;
}

//------------------------------------------------------------------
// Procedure: constructor

VarDataPair::VarDataPair(string var, string sdata)
{
  m_var       = stripBlankEnds(var);
  m_sdata     = stripBlankEnds(sdata);
  m_ddata     = 0;
  m_is_string = true;
  m_is_quoted = false;
  if(isQuoted(sdata))
    m_is_quoted = true;

  m_var_set   = true;
  m_key_set   = false;
  m_ptype_set = false;
  m_ddata_set = false;
  m_sdata_set = true;
}

//------------------------------------------------------------------
// Procedure: constructor
//      Note: For convenience, check whether the given data string is
//            meant to represent a numerical value and do the conversion.

VarDataPair::VarDataPair(string var, string sdata, string hint)
{
  m_var   = stripBlankEnds(var);
  m_ddata = 0;
  m_is_quoted = false;

  string data = stripBlankEnds(sdata);

  if(hint != "auto")
    return;

  if(isNumber(data)) {
    m_ddata = atof(data.c_str());
    m_is_string = false;
    m_ddata_set = true;
    m_sdata_set = false;
  }
  else {
    if(isQuoted(data)) {
      m_sdata = stripQuotes(data);
      m_is_quoted = true;
    }
    else
      m_sdata = data;
    m_is_string = true;
    m_ddata_set = false;
    m_sdata_set = true;
  }

  m_var_set   = true;
  m_key_set   = false;
  m_ptype_set = false;
}

//------------------------------------------------------------------
// Procedure: valid()

bool VarDataPair::valid() const
{
  if((m_var == "") || !m_var_set)
    return(false);

  if(!m_sdata_set && !m_ddata_set)
    return(false);

  return(true);
}

//------------------------------------------------------------------
// Procedure: why_invalid()

int VarDataPair::why_invalid() const
{
  if(m_var == "")
    return(1);
  if(!m_var_set)
    return(2);

  if(!m_sdata_set)
    return(3);
  if(!m_ddata_set)
    return(4);

  return(0);
}

//------------------------------------------------------------------
// Procedure: set_var()

bool VarDataPair::set_var(string var)
{
  if(strContainsWhite(var))
    return(false);
  
  m_var = var;
  m_var_set = true;

  return(true);
}

//------------------------------------------------------------------
// Procedure: set_key()

bool VarDataPair::set_key(string key)
{
  if(m_key_set)
    return(false);
  
  m_key = key;
  m_key_set = true;

  return(true);
}

//------------------------------------------------------------------
// Procedure: set_sdata()

bool VarDataPair::set_sdata(string sval, bool overwrite_ok)
{
  if(m_sdata_set || m_ddata_set)
    if(!overwrite_ok)
      return(false);
  
  m_sdata = sval;
  m_is_string = true;
  m_sdata_set = true;

  return(true);
}

//------------------------------------------------------------------
// Procedure: set_ddata

bool VarDataPair::set_ddata(double dval, bool overwrite_ok)
{
  if(m_sdata_set || m_ddata_set)
    if(!overwrite_ok)
      return(false);
  
  m_ddata = dval;
  m_is_string = false;
  m_ddata_set = dval;

  if(m_sdata_set || m_ddata_set)
    return(false);

  return(true);
}

//------------------------------------------------------------------
// Procedure: set_smart_data()

bool VarDataPair::set_smart_data(string sval, bool overwrite_ok)
{
  if(m_sdata_set || m_ddata_set)
    if(!overwrite_ok)
      return(false);

  if(isNumber(sval)) {
    double dval = atof(sval.c_str());
    m_ddata = dval;
    m_is_string = false;
    m_ddata_set = dval;
  }
  else {  
    m_sdata = sval;
    m_is_string = true;
    m_sdata_set = true;
  }
  
  return(true);
}

//------------------------------------------------------------------
// Procedure: set_ptype()

bool VarDataPair::set_ptype(string sval)
{
  if(m_ptype_set)
    return(false);
  
  m_ptype = sval;
  m_ptype_set = true;

  return(true);
}


//------------------------------------------------------------------
// Procedure: is_solo_macro()

bool VarDataPair::is_solo_macro() const
{
  if(!strBegins(m_sdata, "$[") || !strEnds(m_sdata, "]"))
    return(false);

  if((charCount(m_sdata, '[') != 1) || (charCount(m_sdata, ']') != 1))
    return(false);
  
  return(true);
}



//------------------------------------------------------------------
// Procedure: getPrintable
//  Notes: Return a single string representing the pair. The type
//         are expressed as follows.
//         foo = 27  var is of type double since 27 is a number.
//         foo = "27"  var is of type string since 27 is quoted.
//         foo = bar  var is of type string since bar is NOT a number.


string VarDataPair::getPrintable() const
{
  string rstring = m_var;
  rstring += "=";

  if(m_is_string) {
    if(isNumber(m_sdata)) {
      rstring += "\"";
      rstring += m_sdata;
      rstring += "\"";
    }
    else
      rstring += m_sdata;
  }
  else {
    string dstr = doubleToString(m_ddata, 4);
    // Remove trailing zeros after the decimal point
    dstr = dstringCompact(dstr);
    rstring += dstr;
  }
  return(rstring);
}


//------------------------------------------------------------------
// Procedure: getMacroSet()

set<string> VarDataPair::getMacroSet() const
{
  set<string> macro_set;
  vector<string> macros = getMacrosFromString(m_sdata);
  for(unsigned int i=0; i<macros.size(); i++)
    macro_set.insert(macros[i]);
    
  return(macro_set);
}


//------------------------------------------------------------------
// Procedure: getMacroVector()

vector<string> VarDataPair::getMacroVector() const
{
  return(getMacrosFromString(m_sdata));
}


//------------------------------------------------------------------
// Procedure: stringToVarDataPair()
//   Example: var=MSG, sval=hello, key=foo, ptype=a

VarDataPair stringToVarDataPair(string str)
{
  VarDataPair pair;
  bool ok = true;
 
  vector<string> svector = parseStringZ(str, ',', "{");
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    
    if(param == "var") 
      ok = ok && pair.set_var(value);
    else if(param == "sval") {
      if(isBraced(value))
	value = stripBraces(value);
      ok = ok && pair.set_sdata(value);
    }
    else if(param == "dval") 
      ok = ok && isNumber(value) && pair.set_ddata(atof(value.c_str()));
    else if(param == "key") 
      ok = ok && pair.set_key(value);
    else if(param == "ptype") 
      ok = ok && pair.set_ptype(value);
  }

  if(!ok) {
    VarDataPair empty_pair;
    return(empty_pair);
  }

  return(pair);
}

