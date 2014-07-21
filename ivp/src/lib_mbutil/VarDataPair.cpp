/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarDataPair.h                                        */
/*    DATE: Jul 4th 2005 Monday morning at Brueggers             */
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
#include "VarDataPair.h"
#include "MBUtils.h"

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
}

//------------------------------------------------------------------
// Procedure: constructor

VarDataPair::VarDataPair(const string& var, double ddata)
{
  m_var       = stripBlankEnds(var);
  m_sdata     = "";
  m_ddata     = ddata;
  m_is_string = false;
  m_is_quoted = false;
}

//------------------------------------------------------------------
// Procedure: constructor

VarDataPair::VarDataPair(const string& var, const string& sdata)
{
  m_var       = stripBlankEnds(var);
  m_sdata     = stripBlankEnds(sdata);
  m_ddata     = 0;
  m_is_string = true;
  m_is_quoted = false;
  if(isQuoted(sdata))
    m_is_quoted = true;
}

//------------------------------------------------------------------
// Procedure: constructor
//      Note: For convenience, check whether the given data string is
//            meant to represent a numerical value and do the conversion.

VarDataPair::VarDataPair(const string& var, const string& sdata,
			 const string& hint)
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
  }
  else {
    if(isQuoted(data)) {
      m_sdata = stripQuotes(data);
      m_is_quoted = true;
    }
    else
      m_sdata = data;
    m_is_string = true;
  }
}

//------------------------------------------------------------------
// Procedure: getPrintable
//  Notes: Return a single string representing the pair. The type
//         are expressed as follows.
//         foo = 27  var is of type double since 27 is a number.
//         foo = "27"  var is of type string since 27 is quoted.
//         foo = bar  var is of type string since bar is NOT a number.


string VarDataPair::getPrintable()
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




