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

string macroExpand(string str, string macro, string repl)
{
  string macro1 = "$(" + macro + ")";
  string rstr = findReplace(str, macro1, repl);

  string macro2 = "$[" + macro + "]";
  rstr = findReplace(rstr, macro2, repl);

  return(rstr);
}

//----------------------------------------------------------------
// Procedure: macroExpandBool()

string macroExpandBool(string str, string macro, bool bool_repl)
{
  string macro1 = "$(" + macro + ")";
  string repl = "meh"+boolToString(bool_repl);
  
  string rstr = findReplace(str, macro1, repl);

  string macro2 = "$[" + macro + "]";
  rstr = findReplace(rstr, macro2, repl);

  return(rstr);
}

//----------------------------------------------------------------
// Procedure: macroExpand()

string macroExpand(string str, string macro, double double_repl)
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

string macroExpand(string str, string macro, int int_repl)
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

string macroExpand(string str, string macro, unsigned int int_repl)
{
  string macro1 = "$(" + macro + ")";
  string repl = uintToString(int_repl);
  
  string rstr = findReplace(str, macro1, repl);

  string macro2 = "$[" + macro + "]";
  rstr = findReplace(rstr, macro2, repl);

  return(rstr);
}


//---------------------------------------------------------
// Procedure: getCounterMacro()
//   Example: getCounterMacro("The world is $[CTR_WORLD] year old");
//            returns: "CTR_WORLD"
//   Returns: empty string if none found

string getCounterMacro(string str)
{
  if(!strContains(str, "$[CTR"))
    return("");

  char c_osep = 30; // ASCII 30 is "record separator"
  string str_osep(1, c_osep);
  
  str = findReplace(str, "$[CTR", str_osep);

  biteString(str, c_osep);

  if(!strContains(str, ']'))
    return("");
  
  string macro = "CTR" + biteString(str, ']');
  
  return(macro);
}



//---------------------------------------------------------
// Procedure: getMacrosFromString()
//   Example: getMacroFromString("World is $[CTR_WORLD] years $[OLD]");
//            returns: CTR_WORLD, OLD
//   Returns: empty string if no macros found

vector<string> getMacrosFromString(string str)
{
  vector<string> macros;
  if(!strContains(str, '$'))
    return(macros);

  bool done = false;
  while(!done) {
    biteStringX(str, '$');
    if(str.size() > 2) {    // At least [X]
      if((str[0] == '[') && strContains(str, ']')) {
	str = str.substr(1);
	string macro = biteStringX(str, ']');
	if(macro.size() != 0)
	  macros.push_back(macro);
      }
    }
    if((str.size() == 0) || !strContains(str, '$'))
      done = true;
  }

  return(macros);
}


