/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogicUtils.cpp                                       */
/*    DATE: October 12th 2006                                    */
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

#include <cstring>
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
      else if(((len-i) >= 2) && (!strncmp("==", str+i, 2))) {
	left.replace(i, len-i, "");
	right.replace(0, i+2, "");
	rvector.push_back("==");
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
  unsigned int i, len = str.length();

  if(len==0)
    return(false);

  for(i=0; i<len; i++) {
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
  unsigned int len = str.length();

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

  unsigned int i;
  for(i=0; i<len; i++) {
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


//----------------------------------------------------------------
// Procedure: strFieldMatch
//   Purpose: Performs a certain kind of string match. One string 
//            will match the other if it exactly matches a component
//            of the other string. Components are separated by a
//            a single separator character - by default ':'.
//  Examples: "alpha:bravo:charlie" matches "charlie"
//  Examples: "alpha"               matches "alpha:bravo:charlie"

bool strFieldMatch(const string& str1, 
		   const string& str2, char separator)
{
  // First we can eliminate pairs where one side is not a substring
  // of the other, regardless of separators.

  bool contains12 = strContains(str1, str2);
  bool contains21 = strContains(str2, str1);
  if(!contains12 && !contains21)
    return(false);

  // Now check the more complex case where one side is a proper sub-
  // component of the other. For example:
  // For Example: "alpha:bravo" == "alpha:bravo:charlie"
  // But Not:     "pha:bra" == "alpha:bravo:charlie"
  
  vector<string> svector1 = parseString(str1, separator);
  vector<string> svector2 = parseString(str2, separator);
  unsigned vsize1 = svector1.size();
  unsigned vsize2 = svector2.size();
  if((vsize1 == 0) || (vsize2 == 0))
    return(false);

  unsigned int k;
  for(k=0; k<vsize1; k++)
    svector1[k] = stripBlankEnds(svector1[k]);
  for(k=0; k<vsize2; k++)
    svector2[k] = stripBlankEnds(svector2[k]);

  vector<string> short_vector, long_vector;
  if(vsize1 < vsize2) {
    short_vector = svector1;
    long_vector  = svector2;
  }
  else {
    short_vector = svector2;
    long_vector  = svector1;
  }
  unsigned int short_size = short_vector.size();
  unsigned int long_size  = long_vector.size();

  unsigned int i, j;
  for(j=0; j<long_size; j++) {
    if(short_vector[0] == long_vector[j]) {
      bool possible_match = true;
      for(i=1; i<short_size; i++) {
	j = j+i; 
	if((j>=long_size) || (short_vector[i] != long_vector[j]))
	  possible_match = false;
      }
      if(possible_match == true)
	return(true);
    }
  }

  return(false);
}
    


//----------------------------------------------------------------
// Procedure: getUniqueVars()

vector<string> getUniqueVars(const vector<LogicCondition>& conditions)
{
  vector<string> all_vars;
  for(unsigned int i=0; i<conditions.size(); i++) {
    vector<string> svector = conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }

  vector<string> unique_vars = removeDuplicates(all_vars);

  return(unique_vars);
}




