/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogicUtils.cpp                                       */
/*    DATE: October 12th 2006                                    */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <string.h>
#include "LogicUtils.h"
#include "MBUtils.h"

using namespace std;

#ifdef _WIN32
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

//----------------------------------------------------------------
// Procedure: parseRelation()

vector<string> parseRelation(string gstr)
{
  vector<string> rvector;
  
  gstr = findReplace(gstr, '\t', ' ');
  gstr = findReplace(gstr, ")", ") ");
  gstr = findReplace(gstr, "(", " (");
  gstr = findReplace(gstr, "$ (", "$(");

  gstr = stripBlankEnds(gstr);

  if(!legalParens(gstr))
    return(rvector);
  gstr = pruneParens(gstr);
  string left  = gstr;
  string right = gstr;
  
  const char *str = gstr.c_str();

  if(str[0] == '!') {
    if(globalParens(str+1)) {
      rvector.push_back("not");
      rvector.push_back(str+1);
      return(rvector);
    }
  }

  int  i;
  int  len   = gstr.length();
  int  depth = 0;
  bool done  = false;

  for(i=0; (i<len)&&!done; i++) {
    if(str[i] == '(')
      depth++;
    if(str[i] == ')')
      depth--;
 
    if(depth == 0) {
      if(((len-i) >= 3) && (i!=0) &&
	 (!strncasecmp(" OR ", str+i-1, 4))) {
	left.replace(i, len-i, "");
	right.replace(0, i+2, "");
	rvector.push_back("or");
	done = true;
      }
      else if(((len-i) >= 4) && (i!=0) && 
	 (!strncasecmp(" AND ", str+i-1, 4))) {
	left.replace(i, len-i, "");
	right.replace(0, i+3, "");
	rvector.push_back("and");
	done = true;
      }
      else if(((len-i) >= 1) && (!strncmp("=", str+i, 1))) {
	left.replace(i, len-i, "");
	right.replace(0, i+1, "");
	rvector.push_back("=");
	done = true;
      }
      else if(((len-i) >= 2) && (!strncmp("!=", str+i, 2))) {
	left.replace(i, len-i, "");
	right.replace(0, i+2, "");
	rvector.push_back("!=");
	done = true;
      }
      else if(((len-i) >= 2) && (!strncmp(">=", str+i, 2))) {
	left.replace(i, len-i, "");
	right.replace(0, i+2, "");
	rvector.push_back(">=");
	done = true;
      }
      else if(((len-i) >= 2) && (!strncmp("<=", str+i, 2))) {
	left.replace(i, len-i, "");
	right.replace(0, i+2, "");
	rvector.push_back("<=");
	done = true;
      }
      else if(((len-i) >= 1) && (!strncmp(">", str+i, 1))) {
	left.replace(i, len-i, "");
	right.replace(0, i+1, "");
	rvector.push_back(">");
	done = true;
      }
      else if(((len-i) >= 1) && (!strncmp("<", str+i, 1))) {
	left.replace(i, len-i, "");
	right.replace(0, i+1, "");
	rvector.push_back("<");
	done = true;
      }
    }
  }

  // If we didnt find any relational connector, just return an 
  // empty vector. Indicates a leaf/atom has been reached.
  if(done) {
    rvector.push_back(stripBlankEnds(left));
    rvector.push_back(stripBlankEnds(right));
  }
  return(rvector);

}

//----------------------------------------------------------------
// Procedure: pruneParens()
//   Purpose: Prunes parentheses from the end of a string so long
//            as the resulting string logically remains the same
//            and syntactically correct.
//   Example: (A=2)    gives A=2
//            ((A=2))  gives A=2
//            (A=2)or(B=3)     gives (A=2)or(B=3)
//            (((A=2))or(B=3)) gives ((A=2))or(B=3)

string pruneParens(string gstr)
{
  gstr = stripBlankEnds(gstr);

  bool done = false;
  while(!done) {
    int len = gstr.length();
    
    if(globalParens(gstr)) {  
      gstr.replace(len-1, 1, "");
      gstr.replace(0, 1, "");
    }
    else
      done = true;
  }
  return(gstr);
}

//----------------------------------------------------------------
// Procedure: globalParens()
//   Purpose: Returns TRUE if the string begins and ends with an 
//            open and close parens, and the close parens closes the
//            the open parens in the first character.
//  Examples: (A=1)             true
//            (A=2) and (C<4)   false
//            ((A=2) and (C<4)) true
//            A=1               false

bool globalParens(string str)
{
  str = stripBlankEnds(str);

  if(!legalParens(str)) {
    return(false);
  }

  int len = str.length();
  if(len<2)
    return(false);

  if((str.at(0) != '(') || (str.at(len-1) != ')')) {
    return(false);
  }
          
  str.replace(len-1, 1, "");
  str.replace(0, 1, "");
  
  return(legalParens(str));
}


//----------------------------------------------------------------
// Procedure: globalNotParens()
//      Note: returns true if of the form !(SOMETHING), false otherwise.

bool globalNotParens(string str)
{
  str = stripBlankEnds(str);

  if(str.length() < 3)
    return(false);

  if(str.at(0) == '!')
    if(globalParens(str.c_str()+1))
      return(true);

  return(false);
}


//----------------------------------------------------------------
// Procedure: isValidVariable
//   Purpose: To determine if a given string is an allowable variable.
//      Note: Disallowed characters due to logic syntax:
//            >, <, =, !, ", ), (

bool isValidVariable(string str)
{
  str = stripBlankEnds(str);
  int len = str.length();

  if(len==0)
    return(false);

  for(int i=0; i<len; i++) {
    char c = str.at(i);
    if((c=='"') || (c=='>') || (c=='<') || 
       (c=='=') || (c=='!') || (c==')') || (c=='('))
      return(false);
  }
   
  if(!strcasecmp("OR", str.c_str()))
    return(false);
  if(!strcasecmp("AND", str.c_str()))
    return(false);
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: isValidLiteral
//   Purpose: To determine if a given string is an allowable literal.
//      Note: Disallowed characters due to logic syntax:
//            >, <, =, !, ), (

bool isValidLiteral(string str)
{
  str = stripBlankEnds(str);
  int len = str.length();

  if(len==0)
    return(false);

  if(len==1) {
    char c = str.at(0);
    if((c=='"') || (c=='>') || (c=='<') || 
       (c=='=') || (c=='!') || (c==')') || (c=='('))
      return(false);
    else
      return(true);
  }
  
  // Do the following check before we strip off any potential quotes
  // because "or", "and", with quotes around it, is an acceptable 
  // literal, but without quotes, they are not acceptable.
  
  if(!strcasecmp("OR", str.c_str()))
    return(false);
  if(!strcasecmp("AND", str.c_str()))
    return(false);
  
  if((str.at(0)=='"') && (str.at(len-1)=='"')) {
    str.replace(len-1, 1, "");
    str.replace(0, 1, "");
    len = str.length();
  }

  for(int i=0; i<len; i++) {
    char c = str.at(i);
    if((c=='"') || (c=='>') || (c=='<') || (c=='=') || 
       (c=='!') || (c==')') || (c=='('))
      return(false);
  }
   
  return(true);
}

//----------------------------------------------------------------
// Procedure: isValidRightVariable

bool isValidRightVariable(string str)
{
  str = stripBlankEnds(str);

  int len = str.length();
  if(len < 3)
    return(false);

  if(str.at(0) != '$')
    return(false);
  if(!globalParens(str.c_str()+1))
    return(false);
  
  str.replace(len-1, 1, "");
  str.replace(0, 2, "");
  
  return(isValidVariable(str));
}


//----------------------------------------------------------------
// Procedure: legalParens()

bool legalParens(string gstr)
{
  const char *str = gstr.c_str();
  
  int i;
  int len   = gstr.length();
  
  // First just check for balance of parentheses.
  int depth = 0;
  for(i=0; i<len; i++) {
    if(str[i] == '(')
      depth++;
    if(str[i] == ')')
      depth--;
    if(depth < 0)
      return(false);
  }
  if(depth != 0)
    return(false);
  return(true);
}

//----------------------------------------------------------------
// Procedure: isConditionalParamString

//     true:  "abcdedfg [1234]"
//     true:  "abcdedfg [1234]   "
//     true:  "abcd[90] [1234]   "
//     false: "abcd [sdfb] sdf "
//     true:  "abcd [1234] // comment "

bool isConditionalParamString(const string& giv_str,
			      const string& com_str)
{
  string str;

  if(com_str == "")
    str = giv_str;
  else
    str = stripComment(giv_str, com_str);

  str = stripBlankEnds(str);

  int len = str.length();
  int pos = str.find_last_of('[');
  if(str[len-1] != ']')
    return(false);
  if(pos == -1)
    return(false);

  return(true);
}


