/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

#include <iostream>
#include "MBUtils.h"
#include "HashUtils.h"
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
  string repl = boolToString(bool_repl);
  
  string rstr = findReplace(str, macro1, repl);

  string macro2 = "$[" + macro + "]";
  rstr = findReplace(rstr, macro2, repl);

  return(rstr);
}

//----------------------------------------------------------------
// Procedure: macroExpand()

string macroExpand(string str, string macro, double double_repl,
		   int digits)
{
  if(digits < 0)
    digits = 0;
  else if(digits > 8)
    digits = 8;
  
  string macro1 = "$(" + macro + ")";
  string repl = doubleToStringX(double_repl, digits);
  
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
// Procedure: macroHashExpand()

string macroHashExpand(string str, string macro)
{
  string repl;
  if((macro == "HASH") || (macro == "HASH6"))
    repl = hashAlphaNum(6);
  else if(macro == "HASH2")
    repl = hashAlphaNum(2);
  else if(macro == "HASH3")
    repl = hashAlphaNum(3);
  else if(macro == "HASH4")
    repl = hashAlphaNum(4);
  else if(macro == "HASH5")
    repl = hashAlphaNum(5);
  else if(macro == "HASH7")
    repl = hashAlphaNum(7);
  else if(macro == "HASH8")
    repl = hashAlphaNum(8);
  else if(macro == "HASH9")
    repl = hashAlphaNum(9);

  if(repl == "")
    return(str);
    
  string macro1 = "$(" + macro + ")";
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


//----------------------------------------------------------------
// Procedure: hasMacro()

bool hasMacro(string str, string macro)
{
  string macro1 = "$(" + macro + ")";
  if(strContains(str, macro1))
    return(true);

  string macro2 = "$[" + macro + "]";
  if(strContains(str, macro2))
    return(true);

  return(false);
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
//   Returns: empty vector if no macros found
//      Note: Possible macro types:
//            $[FOO] $(FOO) %[FOO] %(FOO) ${FOO} %{FOO}
//

vector<string> getMacrosFromString(string str, char fchar, char lchar)
{
  // Defaults: fchar='$', lchar='[', rchar=']'

  char rchar = ']';
  if(lchar == '(')
    rchar = ')';
  else if(lchar == '{')
    rchar = '}';
  
  vector<string> macros;
  //if(!strContains(str, '$'))
  if(!strContains(str, fchar))
    return(macros);

  bool done = false;
  while(!done) {
    //biteStringX(str, '$');
    biteStringX(str, fchar);
    if(str.size() > 2) {    // At least [X]
      //if((str[0] == '[') && strContains(str, ']')) {
      if((str[0] == lchar) && strContains(str, rchar)) {
	str = str.substr(1);
	//string macro = biteStringX(str, ']');
	string macro = biteStringX(str, rchar);
	if(macro.size() != 0)
	  macros.push_back(macro);
      }
    }
    //if((str.size() == 0) || !strContains(str, '$'))
    if((str.size() == 0) || !strContains(str, fchar))
      done = true;
  }

  return(macros);
}


//---------------------------------------------------------
// Procedure: macroDefault()
//   Purpose: For macros with defaults, e.g., $[FOO:=23], given
//            just the internal, e.g., "FOO:=23", return the
//            part just to the right of the ":=".
//   Example: macroDefault("FOOBAR:=green");
//            returns: "green"
//   Example: macroDefault("FOOBAR=green");
//            returns: "green"
//   Example: macroDefault("FOOBAR");
//            returns: ""

string macroDefault(string str)
{
  if(strContains(str, ":=")) {
    nibbleString(str, ":=");
    return(str);
  }
  
  if(strContains(str, "=")) {
    nibbleString(str, "=");
    return(str);
  }

  return("");
}

//--------------------------------------------------------
// Procedure: expandMacrosWithDefault()
//   example: color=$(COLOR:=green) --> color=green

string expandMacrosWithDefault(string line)
{
  vector<string> macros1,macros2;
  macros1 = getMacrosFromString(line, '$', '(');
  macros2 = getMacrosFromString(line, '%', '(');

  for(unsigned int i=0; i<macros1.size(); i++) {
    // Check if macro is of the form "VAR:=VAL"
    string repl = macroDefault(macros1[i]);
    if(repl != "") {
      string macro = "$(" + macros1[i] + ")";
      line = findReplace(line, macro, repl);
    }
  }

  for(unsigned int i=0; i<macros2.size(); i++) {
    // Check if macro is of the form "VAR:=VAL"
    string repl = macroDefault(macros2[i]);
    if(repl != "") {
      string macro = "%(" + macros2[i] + ")";
      line = findReplace(line, macro, toupper(repl));
    }
  }

  return(line);
}


//---------------------------------------------------------
// Procedure: macroBase()
//   Purpose: FOO:=23  ==> FOO
//   Purpose: FOO*=23  ==> FOO
//            FOO ==> FOO

string macroBase(string str, string patsep)
{
  return(nibbleString(str, patsep));
}


//--------------------------------------------------------
// Procedure: reduceMacrosToBase()
//      Note: The string sep is usually ":=" or "*="
//   Example: line: "color=$(COLOR=red), name=$(NAME)"
//            sep:  "="
//            Result: "color=$(COLOR), name=$(NAME)"

string reduceMacrosToBase(string line, string sep, string macro)
{
  if((sep == "") || (macro == ""))
    return(line);

  vector<string> macros1,macros2;
  macros1 = getMacrosFromString(line, '$', '(');
  macros2 = getMacrosFromString(line, '%', '(');

  for(unsigned int i=0; i<macros1.size(); i++) {
    string base = macroBase(macros1[i], sep);
    if(base == macro) {
      string macro = "$(" + macros1[i] + ")";
      string macro_base = "$(" + base + ")";
      line = findReplace(line, macro, macro_base);
    }
  }

  for(unsigned int i=0; i<macros2.size(); i++) {
    string base = macroBase(macros2[i], sep);
    if(base == macro) {
      string macro = "%(" + macros2[i] + ")";
      string macro_base = "%(" + base + ")";
      line = findReplace(line, macro, macro_base);
    }
  }

  return(line);
}



