/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MacroUtils.cpp                                       */
/*    DATE: June 24th, 2020                                      */
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

#include "MBUtils.h"
#include "MacroUtils.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: macroExpand()

string macroExpand(const string& str, const string& macro, const string& repl)
{
  string macro1 = "$(" + macro + ")";
  string rstr = findReplace(str, macro1, repl);

  string macro2 = "$[" + macro + "]";
  rstr = findReplace(rstr, macro2, repl);

  return(rstr);
}

//----------------------------------------------------------------
// Procedure: macroExpand()

string macroExpand(const string& str, const string& macro, bool bool_repl)
{
  string macro1 = "$(" + macro + ")";
  string repl = boolToString(bool_repl);
  
  string rstr = findReplace(str, macro1, repl);

  string macro2 = "$[" + macro + "]";
  rstr = findReplace(rstr, macro2, repl);

  return(rstr);
}

//----------------------------------------------------------------
// Procedure: macroExpand()

string macroExpand(const string& str, const string& macro, double double_repl)
{
  string macro1 = "$(" + macro + ")";
  string repl = doubleToStringX(double_repl,3);
  
  string rstr = findReplace(str, macro1, repl);

  string macro2 = "$[" + macro + "]";
  rstr = findReplace(rstr, macro2, repl);

  return(rstr);
}


//----------------------------------------------------------------
// Procedure: macroExpand()

string macroExpand(const string& str, const string& macro, int int_repl)
{
  string macro1 = "$(" + macro + ")";
  string repl = intToString(int_repl);
  
  string rstr = findReplace(str, macro1, repl);

  string macro2 = "$[" + macro + "]";
  rstr = findReplace(rstr, macro2, repl);

  return(rstr);
}


//----------------------------------------------------------------
// Procedure: macroExpand()

string macroExpand(const string& str, const string& macro, unsigned int int_repl)
{
  string macro1 = "$(" + macro + ")";
  string repl = uintToString(int_repl);
  
  string rstr = findReplace(str, macro1, repl);

  string macro2 = "$[" + macro + "]";
  rstr = findReplace(rstr, macro2, repl);

  return(rstr);
}

