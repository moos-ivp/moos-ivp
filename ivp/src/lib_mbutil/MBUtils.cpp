/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MBUtils.cpp                                          */
/*    DATE: (1996-2005)                                          */
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

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctype.h>
#include <iostream>
#include "MBUtils.h"

//added for time support in W32 platforms PMN  - 18 July 2005
#ifdef _WIN32
#include "windows.h"
#include "winbase.h"
#include "winnt.h"
#include <conio.h>
#endif

using namespace std;

//----------------------------------------------------------------
// Procedure: parseString(string, char)
//   Example: svector = parseString("apples,  pears,banannas", ',');
//            svector[0] = "apples"
//            svector[1] = "  pears"
//            svector[2] = "bananas"

vector<string> parseString(const string& string_str, char separator)
{
  const char *str = string_str.c_str();

  char *buff = new char[strlen(str)+1];

  vector<string> rvector; 

  while(str[0] != '\0') {    
    int i=0;
    while((str[i]!=separator)&&(str[i]!='\0'))
      i++;
    strncpy(buff, str, i);
    buff[i] = '\0';
    string nstr = buff;

    rvector.push_back(nstr);
    str += i;
    if(str[0]==separator)
      str++;
  }
  delete [] buff;
  return(rvector);
}

//----------------------------------------------------------------
// Procedure: parseStringQ(string, char)
//     Notes: Differs from "parseString" in that the separator is 
//            ignored if it is found in an open-quote mode. That is
//            after an odd # of double-quote characters have appeared.
//   Example: str = "left=apples # left=pears # left=\"bana#nas\""
//   w/out Q: svector = parseString(str, '#');
//            svector[0] = [left=apples ]
//            svector[1] = [ left=pears ]
//            svector[2] = [ left="bana#]
//            svector[3] = [nas"]
//  Compare:  svector = parseStringQ(str, '#');
//            svector[0] = [left=apples ]
//            svector[1] = [ left=pears ]
//            svector[2] = [ left="bana#nas"]

vector<string> parseStringQ(const string& string_str, char separator)
{
  bool in_quotes = false;

  unsigned int  brace_count = 0;

  const char *str = string_str.c_str();
  char *buff = new char[strlen(str)+1];
  vector<string> rvector;
  while(str[0] != '\0') {    
    int i=0;
    while(((str[i]!=separator) || in_quotes || (brace_count>0)) && (str[i]!='\0')) {
      if(str[i]=='"')
	in_quotes = !in_quotes;
      else if(str[i]=='{')
	brace_count++;
      else if(str[i]=='}') {
	if(brace_count > 0)
	  brace_count--;
      }
      i++;
    }
    strncpy(buff, str, i);
    buff[i] = '\0';
    string nstr = buff;

    rvector.push_back(nstr);
    str += i;
    if(str[0]==separator)
      str++;
  }
  delete [] buff;
  return(rvector);
}

//----------------------------------------------------------------
// Procedure: parseStringZ(string, char)
//     Notes: Differs from "parseString" in that the separator is 
//            ignored if it is found in an open-quote mode. That is
//            after an odd # of double-quote characters have appeared.

vector<string> parseStringZ(const string& string_str, char separator,
			    const string&protectors)
{
  bool quote_protect = strContains(protectors, '"');
  bool brace_protect = strContains(protectors, '{');
  bool brack_protect = strContains(protectors, '[');
  bool paren_protect = strContains(protectors, '(');

  unsigned int  quote_count = 0;
  unsigned int  brace_count = 0;
  unsigned int  brack_count = 0;
  unsigned int  paren_count = 0;

  const char *str = string_str.c_str();
  char *buff = new char[strlen(str)+1];
  vector<string> rvector;
  while(str[0] != '\0') {    
    int i=0;
    
    while(((str[i]!=separator) || 
	   (quote_count > 0) || (brace_count>0) ||
	   (brack_count > 0) || (paren_count>0)) && 
	  (str[i]!='\0')) {
      if(quote_protect && str[i]=='"') {
	if(quote_count == 0)
	  quote_count = 1;
	else
	  quote_count = 0;
      }

      if(brace_protect) {
	if(str[i]=='{')
	  brace_count++;
	else if(str[i]=='}') {
	  if(brace_count > 0)
	    brace_count--;
	}
      }
      if(paren_protect) {
	if(str[i]=='(')
	  paren_count++;
	else if(str[i]==')') {
	  if(paren_count > 0)
	    paren_count--;
	}
      }
      if(brack_protect) {
	if(str[i]=='[')
	  brack_count++;
	else if(str[i]==']') {
	  if(brack_count > 0)
	    brack_count--;
	}
      }

      i++;
    }
    strncpy(buff, str, i);
    buff[i] = '\0';
    string nstr = buff;

    rvector.push_back(nstr);
    str += i;
    if(str[0]==separator)
      str++;
  }
  delete [] buff;
  return(rvector);
}

//----------------------------------------------------------------
// Procedure: parseStringQ(string, char, unsigned int)
//   Purpose: Same as parseStringQ, but bundle component into strings
//            with length no greater than maxlen
//   Example: string_str = community=shoreside,hostip=128.30.24.232,foo=bar
//            maxlen     = 40
//    Result: rvector[0]="community=shoreside,hostip=128.30.24.232,"
//            rvector[0]="foo=bar"
 
vector<string> parseStringQ(const string& string_str, char separator,
			    unsigned int maxlen)
{
  vector<string> rvector;

  vector<string> svector = parseStringQ(string_str, separator);
  unsigned int i, vsize = svector.size();
  string buffer;
  for(i=0; i<vsize; i++) {
    if(buffer == "")
      buffer = svector[i];
    else {
      if((buffer.length() + 1 + svector[i].length()) <= maxlen)
	buffer = buffer + separator + svector[i];
      else {
	rvector.push_back(buffer + ",");
	buffer = svector[i];
      }
    }
  }
  if(buffer != "")
    rvector.push_back(buffer);

  return(rvector);
}

//----------------------------------------------------------------
// Procedure: parseString(string, string)
//   Example: svector = parseString("apples $@$ pears $@$ banannas", "$@$");
//            svector[0] = "apples "
//            svector[1] = " pears "
//            svector[2] = " bananas"

vector<string> parseString(const string& string_str, 
			   const string& separator)
{
  string new_separator;
  char   unique_char = (char)129;
  new_separator += unique_char;

  string new_string = findReplace(string_str, separator, new_separator);

  vector<string> rvector = parseString(new_string, unique_char);

  return(rvector);
}


//----------------------------------------------------------------
// Procedure: parseQuotedString(string, char)
//   Example: svector = parseQuotedString(""apples,pears",banannas", ',');
//            svector[0] = "apples,pears"
//            svector[1] = "bananas"

vector<string> parseQuotedString(const string& string_str, char separator)
{
  const char *str = string_str.c_str();

  char *buff = new char[strlen(str)+1];

  vector<string> rvector;

  // First count the number of double-quotes. If not an even number, 
  // return an empty vector.  
  string::size_type len = string_str.length();
  int quote_counter = 0;
  for(string::size_type i=0; i<len; i++)
    if(string_str.at(i) == '"')
      quote_counter++;
  
  if((quote_counter % 2) != 0) {
    delete [] buff;
    return(rvector);
  }

  bool even_quote = true;
  while(str[0] != '\0') {    
    string::size_type i=0;
    while(((str[i]!=separator) || !even_quote) && (str[i]!='\0')) {
      if(str[i]=='"')
	even_quote = !even_quote;
      i++;
    }
    strncpy(buff, str, i);
    buff[i] = '\0';
    string nstr = buff;

    rvector.push_back(nstr);
    str += i;
    if(str[0]==separator)
      str++;
  }
  delete [] buff;
  return(rvector);
}

//----------------------------------------------------------------
// Procedure: parseStringToWords
//     Notes: Break up the string into a group of white space separated
//            chunks

vector<string> parseStringToWords(const string& str, char grouping_char)
{
  vector<string> rvector;
  string word;

  int grouping_char_count = 0;

  char rgc = 0;  // right grouping char
  if(grouping_char == '(')
    rgc = ')';
  else if(grouping_char == '{')
    rgc = '}';
  else if(grouping_char == '[')
    rgc = ']';
  else if(grouping_char == '\"')
    rgc = '\"';
    

  unsigned int i, len = str.length();
  for(i=0; i<len; i++) {
    char c = str.at(i);
    if((grouping_char != 0) && (c == grouping_char)) {
      grouping_char_count++;
      word.push_back(c);
    }
    else if((rgc != 0) && (c == rgc)) {
      grouping_char_count--;
      word.push_back(c);
    }
    else if(((c != '\t') && (c != ' ')) || (grouping_char_count > 0))
      word.push_back(c);
    else {
      if(word.length() > 0) {
	rvector.push_back(word);
	word.clear();
      }
    }
  }

  if(word.length() != 0)
    rvector.push_back(word);

  return(rvector);
}

//----------------------------------------------------------------
// Procedure: chompString(const string&, char)
//   Example: svector = chompString("apples, pears, bananas", ',')
//            svector[0] = "apples"
//            svector[1] = " pears, bananas"

vector<string> chompString(const string& string_str, char separator)
{
  const char *str = string_str.c_str();

  char* buff = new char [strlen(str)+1];

  vector<string> rvector;

  string::size_type i=0;
  while((str[i]!=separator)&&(str[i]!='\0'))
    i++;

  strncpy(buff, str, i);
  buff[i] = '\0';
  string str_front = buff;
  
  rvector.push_back(str_front);
  str += i;
  if(str[0]==separator)
    str++;

  string str_back = str;
  rvector.push_back(str_back);

  delete [] buff;

  return(rvector);
}

//----------------------------------------------------------------
// Procedure: augmentSpec

string augmentSpec(const string& orig, const string& new_part, char sep)
{
  if(new_part == "")
    return(orig);

  string return_str = orig;
  if(orig != "")
    return_str += sep;

  return(return_str + new_part);
}

//----------------------------------------------------------------
// Procedure: removeWhite(const string&)
//   Example: input_str = "apples, pears, bananas "
//            str = removeWhite(input_str)
//            str = "apples,pears,bananas"

string removeWhite(const string& str)
{
  string return_string;
  unsigned int i, vsize = str.length();
  for(i=0; i<vsize; i++) {
    int c = (int)(str.at(i));
    if((c != 9) && (c != 32))
      return_string += str.at(i);
  }
  return(return_string);
}

//----------------------------------------------------------------
// Procedure: removeWhiteEnd(const string&)
//   Example: input_str = "apples, pears, bananas     "
//            str = removeWhiteEnd(input_str)
//            str = "apples, pears, bananas"

string removeWhiteEnd(const string& str)
{
  unsigned int vsize = str.size();
  
  for(unsigned int i=0; i<vsize; i++) {
    unsigned int index = vsize-1-i;
    
    int c = (int)(str.at(index));
    if((c != 9) && (c != 32))
      return(str.substr(0,index+1));
  }
  return("");
}

//----------------------------------------------------------------
// Procedure: removeWhiteLN(const string&)
//      Note: Same as removeWhite(), but also looks for CR and LF
//   Example: input_str = "apples, pears, bananas "
//            str = removeWhite(input_str)
//            str = "apples,pears,bananas"

string removeWhiteNL(const string& str)
{
  string return_string;
  unsigned int i, vsize = str.length();
  for(i=0; i<vsize; i++) {
    int c = (int)(str.at(i));
    if((c != 9) && (c != 32) && (c != 10) && (c != 13))
      return_string += str.at(i);
  }
  return(return_string);
}

//----------------------------------------------------------------
// Procedure: biteString(string&, char)
//   Example: input_str = "apples, pears, bananas"
//            str = biteString(input_str, ',')
//            str = "apples"
//            input_str = " pears, bananas"

string biteString(string& str, char separator)
{
  string::size_type len = str.length();
  if(len == 0)
    return("");

  bool found = false;
  string::size_type ix=0;
  string::size_type i=0;
  for(i=0; (!found && i<len); i++) {
    if(str[i] == separator) {
      found = true;
      ix = i;
    }
  }

  if(!found) {
    string str_front = str;
    str = "";
    return(str_front);
  }

  string str_front(str.c_str(), ix);
  string str_back;
  if((ix+1) < len)
    str_back = str.substr(ix+1);
  str = str_back;

  return(str_front);
}

//----------------------------------------------------------------
// Procedure: biteStringX(string&, char)
//      Note: Same as biteString except blank ends will be removed
//            from both the returned and remaining value.

string biteStringX(string& str, char separator)
{
  string front = stripBlankEnds(biteString(str, separator));
  str = stripBlankEnds(str);
  return(front);
}

//----------------------------------------------------------------
// Procedure: rbiteStringX(string&, char)
//      Note: Same as rbiteString except blank ends will be removed
//            from both the returned and remaining value.

string rbiteStringX(string& str, char separator)
{
  string str_back = stripBlankEnds(rbiteString(str, separator));
  str = stripBlankEnds(str);
  return(str_back);
}

//----------------------------------------------------------------
// Procedure: biteString(string&, char, char)
//      Note: Same as biteString(string,char) except the bite will
//            occur at the point where either of the two given
//            characters occur.

string biteString(string& str, char sep1, char sep2)
{
  if(str.length() == 0)
    return("");

  string::size_type i=0;
  while((str[i]!=sep1) && (str[i]!=sep2) && (str[i]!='\0'))
    i++;

  string str_front(str.c_str(), i);

  if(str[i] == '\0')
    str = "";
  else {  
    string str_back(str.c_str()+i+1);
    str = str_back;
  }

  return(str_front);
}

//----------------------------------------------------------------
// Procedure: rbiteString(string&, char)
//   Example: input_str = "apples, pears, bananas"
//            str = rbiteString(input_str, ',')
//            str = " bananas"
//            input_str = "apples, pears"

string rbiteString(string& str, char separator)
{
  string::size_type len = str.length();
  if(len == 0)
    return("");

  bool found = false;
  string::size_type ix=0;
  string::size_type i=0;
  for(i=len-1; (!found && i!=0); i--) {
    if(str[i] == separator) {
      found = true;
      ix = i;
    }
  }

  if(!found) {
    string str_back = str;
    str = "";
    return(str_back);
  }

  string str_front(str.c_str(), ix);
  string str_back;
  if((ix+1) < len)
    str_back = str.substr(ix+1);
  str = str_front;

  return(str_back);
}

//----------------------------------------------------------------
// Procedure: sortStrings
//      Note: O(n^2) simple bubble-sort algorithm

vector<string> sortStrings(vector<string> svector)
{
  vector<string>::size_type vsize = svector.size();
  
  for(vector<string>::size_type i=0; i<vsize; i++) {
    for(vector<string>::size_type j=0; j<(vsize-1-i); j++) {
      if(svector[j+1] < svector[j]) { // compare the two neighbors
	string tmp = svector[j];
	svector[j] = svector[j+1];
	svector[j+1] = tmp;
      }
    }
  }
  return(svector);
}

//----------------------------------------------------------------
// Procedure: mergeVectors
//      Note: 

vector<string> mergeVectors(vector<string> vector1, 
			    vector<string> vector2)
{
  vector<string>::size_type vsize = vector2.size();
  for(vector<string>::size_type i=0; i<vsize; i++)
    vector1.push_back(vector2[i]);
  return(vector1);
}

//----------------------------------------------------------------
// Procedure: removeDuplicates
//      Note: Return a vector of strings such that no string is in
//            the vector more than once.

vector<string> removeDuplicates(const vector<string>& svector)
{
  vector<string> rvector;
  
  vector<string>::size_type vsize = svector.size();
  for(vector<string>::size_type i=0; i<vsize; i++)
    if(!vectorContains(rvector, svector[i]))
      rvector.push_back(svector[i]);
  
  return(rvector);
}

//----------------------------------------------------------------
// Procedure: vectorContains

bool vectorContains(const vector<string>& svector,
		    const string& str,
		    bool case_sensitive)
{
  vector<string>::size_type vsize = svector.size();
  if(case_sensitive) {
    for(vector<string>::size_type i=0; i<vsize; i++)
      if(svector[i] == str)
	return(true);
  }
  else {
    string lower_str = tolower(str);
    for(vector<string>::size_type i=0; i<vsize; i++)
      if(tolower(svector[i]) == lower_str)
	return(true);
  }

  return(false);
}

//----------------------------------------------------------------
// Procedure: stripBlankEnds(string)

string stripBlankEnds(const string& str)
{
  if(str.length() == 0)
    return("");

  const char *sPtr = str.c_str();
  int length = strlen(sPtr);

  int i; 
  int startIX=length; // assume all blanks

#if 0
  // Added Dec 23rd 2007 (mikerb)
  // Quick check to see if the string has already been stripped
  // of blank ends. Just return the original in that case.
  int s_char = (int)(sPtr[0]);
  int e_char = (int)(sPtr[length-1]);
  if((s_char != 9) && (s_char != 32) &&
     (e_char != 9) && (e_char != 32))
    return(str);
#endif

  for(i=0; i<length; i++) {
    int cval = (int)(sPtr[i]);
    if(!((cval==9) ||     // 09:tab 
	 (cval==32))) {   // 32:blank
      startIX = i;
      i = length;
    }
  }

  if(sPtr[startIX]==0)   // If first non-blank is NULL term,
    startIX=length;      // treat as if blank, empty line.


  if(startIX==length) {      // Handle case where the
    string ret_string = "";  // whole string was blanks
    return(ret_string);      // and/or tabs    
  }

  int endIX=-1; 

  for(i=length-1; i>=0; i--) {
    int cval = (int)(sPtr[i]);
    if(!((cval==9)  ||    // 09:tab        
	 (cval==0)  ||    // 00:NULL terminator
	 (cval==32) ||    // 32:blank
	 (cval==13) ||    // 13:car-return
	 (cval==10))) {   // 10:newline
      endIX = i;
      i = -1;
    }
  }

  if(startIX > endIX) {      // Handle case where the
    string ret_string = "";  // whole string was blanks,
    return(ret_string);      // tabs, newlines, or car-return
  }

  length = (endIX - startIX) + 1;
  
  char *buff = new char [length+1];
  strncpy(buff, sPtr+startIX, length);
  buff[length] = '\0';

  string ret_string = buff;
  delete [] buff;
  return(ret_string);
}


//----------------------------------------------------------------
// Procedure: tolower(string)

string tolower(const string& str)
{
  string rstr = str;
  string::size_type len = str.length();
  for(string::size_type i=0; i<len; i++)
    rstr[i] = tolower(str[i]);
  return(rstr);
}

//----------------------------------------------------------------
// Procedure: toupper(string)

string toupper(const string& str)
{
  string rstr = str;
  string::size_type len = str.length();
  for(string::size_type i=0; i<len; i++)
    rstr[i] = toupper(str[i]);
  return(rstr);
}

//----------------------------------------------------------------
// Procedure: truncString

string truncString(const string& str, unsigned int newlen, string style)
{
  unsigned int len = str.length();
  if(newlen > len)
    return(str);

  if((style == "") || (style == "basic") || (style == "front"))
    return(str.substr(0, newlen));

  if(style == "back")
    return(str.substr(len-newlen, newlen));

  // Otherwise the style is request is assumed to be "middle"

  // It doesnt' make sense to middle down to a string less than 4 since
  // the two periods in the middle should have at least one character
  // on each side. If this is requested, just return trunc-front.
  if(newlen < 4)
    return(str.substr(0, newlen));

  unsigned int back = newlen / 2;
  unsigned int front = newlen - back;
  back--;
  front--;
  string new_str = str.substr(0,front) + ".." + str.substr(len-back,len);
  return(new_str);
}

//----------------------------------------------------------------
// Procedure: xxxToString(value)

string boolToString(bool val)
{
  if(val)
    return("true");
  else
    return("false");
}

string intToString(int val)
{
  char buff[500];
  sprintf(buff, "%d", val);
  string str = buff;
  return(str);
}

string uintToString(unsigned int val)
{
  char buff[500];
  sprintf(buff, "%u", val);
  string str = buff;
  return(str);
}

string ulintToString(unsigned long int val)
{
  char buff[500];
  sprintf(buff, "%lu", val);
  string str = buff;
  return(str);
}

string floatToString(float val, int digits)
{
  char format[10] = "%.5f\0";
  if((digits>=0)&&(digits<=9))
    format[2] = digits+48;
  
  if(val > (float)(pow((float)(2),(float)(64))))
    format[3] = 'e';

  char buff[1000];
  sprintf(buff, format, val);
  string str = buff;
  return(str);
}

string doubleToString(double val, int digits)
{
  char format[10] = "%.5f\0";
  if((digits>=0)&&(digits<=9))
    format[2] = digits+48;
  else if((digits >= 10) && (digits <= 19)) {
    format[2] = '1';
    format[3] = (digits+38);
    format[4] = 'f';
    format[5] = '\0';
  }
  else {
    format[2] = '2';
    format[3] = '0';
    format[4] = 'f';
    format[5] = '\0';
  }

  if(val > (double)(pow((double)(2),(double)(128))))
    format[3] = 'e';

  char buff[1000];
  sprintf(buff, format, val);
  string str = buff;
  return(str);
}

string doubleToStringX(double val, int digits)
{
  string rstr = dstringCompact(doubleToString(val, digits));
  if(rstr == "-0")
    return("0");
  return(rstr);
}

string setToString(const set<string>& str_set)
{
  string rstr;
  set<string>::iterator p;
  for(p=str_set.begin(); p!=str_set.end(); p++) {
    if(rstr != "")
      rstr += ",";
    rstr += *p;
  }
  return(rstr);
}

//----------------------------------------------------------------
// Procedure: intToCommaString

string intToCommaString(int ival)
{
  string str = intToString(ival);
  string new_str;

  unsigned int i, len = str.length();
  for(i=0; i<len; i++) {
    new_str += str.at(i);
    if((((len-(i+1))%3)==0) && (i!=len-1))
      new_str += ',';
  }
  return(new_str);
}


//----------------------------------------------------------------
// Procedure: uintToCommaString

string uintToCommaString(unsigned int ival)
{
  string str = uintToString(ival);
  string new_str;

  unsigned int i, len = str.length();
  for(i=0; i<len; i++) {
    new_str += str.at(i);
    if((((len-(i+1))%3)==0) && (i!=len-1))
      new_str += ',';
  }
  return(new_str);
}


//----------------------------------------------------------------
// Procedure: ulintToCommaString

string ulintToCommaString(unsigned long int ival)
{
  string str = ulintToString(ival);
  string new_str;

  unsigned int i, len = str.length();
  for(i=0; i<len; i++) {
    new_str += str.at(i);
    if((((len-(i+1))%3)==0) && (i!=len-1))
      new_str += ',';
  }
  return(new_str);
}


//----------------------------------------------------------------
// Procedure: dstringCompact
//      Note: Convert 6.43000 to 6.43
//                    6.00000 to 6
//                    6.      to 6

string dstringCompact(const string& str)
{
  if(str=="0") 
    return("0");

  string::size_type ssize = str.size();
  if(ssize == 0)
    return("");

  bool has_decimal = false;

  for(string::size_type i=0; i<ssize; i++)
    if(str[i] == '.')
      has_decimal = true;
  
  if(!has_decimal)
    return(str);

  char *buff = new char[ssize+1];
  strcpy(buff, str.c_str());
  buff[ssize] = '\0';

  for(string::size_type j=ssize-1; ; j--) {
    if(buff[j] == '0')
      buff[j] = '\0';
    else {
      if(buff[j] == '.')
	buff[j] = '\0';
      break;
    }
  }

  string ret_str = buff;
  delete [] buff;
  return(ret_str);
}

//----------------------------------------------------------------
// Procedure: compactConsecutive
//      Note: f("Apple______Pear", '_') returns "Apple_Pear"
//            f("Too         Bad", ' ') return  "Too Bad"
// 

string compactConsecutive(const string& str, char dchar)
{
  string::size_type len = strlen(str.c_str());

  if(len==0)
    return("");

  char *buffer = new char [len+1];

  buffer[0] = str[0];

  string::size_type index = 1;
  for(string::size_type i=1; i<len; i++) {
    if((str[i] != dchar) || (str[i-1] != dchar)) {
      buffer[index] = str[i]; 
      index++;
    }
  }
  buffer[index] = '\0';
  string return_string = buffer;
  delete [] buffer;
  return(return_string);
}

//----------------------------------------------------------------
// Procedure: findReplace
//      Note: Added May 29, 2005
//            Replace all occurances of fchar with rchar in str

string findReplace(const string& str, char fchar, char rchar)
{
  string rstr = str;
  string::size_type str_size = str.size();
  for(string::size_type i=0; i<str_size; i++)
    if(str[i] == fchar)
      rstr[i] = rchar;
  
  return(rstr);
}
    
//----------------------------------------------------------------
// Procedure: padString
//      Note: Added Aug 1405
//   Purpose: Pad the given string with enough blanks to reach the
//            length given by target_size. If front is true, pad on
//            to the front of the string. To the end otherwise. 

string padString(const string& str, 
		 std::string::size_type target_size, bool front)
{
  string rstr  = str;
  string::size_type str_size = str.size();
  
  while(str_size < target_size) {
    if(front)
      rstr = " " + rstr;
    else
      rstr = rstr + " ";
    str_size++;
  }
  
  return(rstr);
}
    

//----------------------------------------------------------------
// Procedure: padVector
//      Note: Added Apr 2219
//   Purpose: Pad all strings in the given vector such that they 
//            are all of equal length, determined by the one with 
//            the maximum length. If front is true, pad on to the
//            front of the string. To the end otherwise. 

vector<string> padVector(const vector<string>& svector, bool front)
{
  unsigned int maxlen = 0;
  for(unsigned int i=0 ; i<svector.size(); i++)
    if((i==0) || (svector[i].length() > maxlen))
      maxlen = svector[i].length();

  vector<string> return_vector;
  for(unsigned int i=0 ; i<svector.size(); i++)
    return_vector.push_back(padString(svector[i], maxlen, front));

  return(return_vector);
}
    

//----------------------------------------------------------------
// Procedure: findReplace
//      Note: Added Jun1405
//            Replace all occurances of fstr with rstr in str

string findReplace(const string& str, const string& fstr, 
		   const string& rstr)
{
  string::size_type posn = 0;

  string newstr = str;
  while(posn != string::npos) {
    posn = newstr.find(fstr, posn);
    if(posn != string::npos) {
      newstr.replace(posn, fstr.length(), rstr);
      posn += rstr.length();
    }
  }
  return(newstr);
}
    

//----------------------------------------------------------------
// Procedure: stripComment
//      Note: Added July 14th 05 (on the flight to Oxford, pre-Pianosa)
//            Treat cstr as a comment indicator, e.g. "//". Return the
//            portion of the string to the left of the comment
//   Example: stripComment("Hello World // note", "//")
//            returns "Hello World "

string stripComment(const string& str, const string& cstr)
{
  string::size_type posn = str.find(cstr, 0);
  if(posn == string::npos)
    return(str);

  string::size_type ssize = str.size();
  char *buff = new char[ssize+1];
  strcpy(buff, str.c_str());
  buff[ssize] = '\0';
  buff[posn]  = '\0';

  string return_string = buff;
  delete [] buff;
  return(return_string);
}
    

//---------------------------------------------------------
// Procedure: isValidIPAddress
//   Purpose: Determine if the string is a valid IP address: 
//            - has four numerical fields separated by a decimal point.
//            - each field is in the range 0-255

bool isValidIPAddress(const string& ipstring)
{
  if(tolower(ipstring) == "localhost")
    return(true);

  vector<string> svector = parseString(ipstring, '.');
  unsigned int i, vsize  = svector.size();
  if(vsize != 4)
    return(false);
  for(i=0; i<vsize; i++) {
    string part = stripBlankEnds(svector[i]);
    if(!isNumber(part))
      return(false);
    double dval = atof(svector[i].c_str());
    if((dval < 0) || (dval > 255))
      return(false);
  }
  return(true);    
}

//----------------------------------------------------------------
// Procedure: strContains
//      Note: Added July 14 05 (on the flight to Oxford)

bool strContains(const string& str, const string& qstr)
{
  string::size_type posn = str.find(qstr, 0);
  if(posn == string::npos)
    return(false);
  else
    return(true);
}
    
//----------------------------------------------------------------
// Procedure: strContains

bool strContains(const string& str, const char c)
{
  string::size_type posn = str.find_first_of(c, 0);
  if(posn == string::npos)
    return(false);
  else
    return(true);
}
    
//----------------------------------------------------------------
// Procedure: strBegins
//      Note: Added Nov 2nd 09 (on the flight to DC)

bool strBegins(const string& str, const string& qstr, bool case_matters)
{
  string::size_type i, qlen = qstr.length();
  if((str.length() < qlen) || (qlen == 0))
    return(false);
  
  if(case_matters) {
    for(i=0; i<qlen; i++)
      if(str[i] != qstr[i])
	return(false);
  }
  else {
    for(i=0; i<qlen; i++)
      if(tolower(str[i]) != tolower(qstr[i]))
	return(false);
  }
  
  return(true);
}
    
//----------------------------------------------------------------
// Procedure: strBegins
//      Note: Added Nov 2nd 09 (on the flight to DC)

bool strEnds(const string& str, const string& qstr, bool case_matters)
{
  string::size_type qlen = qstr.length();
  string::size_type slen = str.length();
  if((slen < qlen) || (qlen == 0))
    return(false);
  
  string::size_type i, start_ix = slen-qlen;

  if(case_matters) {
    for(i=0; i<qlen; i++)
      if(str[i+start_ix] != qstr[i])
	return(false);
  }
  else {
    for(i=0; i<qlen; i++)
      if(tolower(str[i+start_ix]) != tolower(qstr[i]))
	return(false);
  }
  
  return(true);
}
    
//----------------------------------------------------------------
// Procedure: isBoolean

bool isBoolean(const string& str)
{
  string s = tolower(str);
  if((s == "true") || (s == "false"))
    return(true);
  return(false);
}
    
//----------------------------------------------------------------
// Procedure: stringIsFalse

bool stringIsFalse(const string& str)
{
  string s = tolower(str);
  if((s == "false") || (s == "0") || (s == "no"))
    return(true);
  return(false);
}
    
//----------------------------------------------------------------
// Procedure: strContainsWhite
//      Note: Returns true if the given string contains either a 
//            blank or tab character.

bool strContainsWhite(const string& str)
{
  string::size_type posn = str.find_first_of(' ', 0);
  if(posn != string::npos)
    return(true);
  
  posn = str.find_first_of('\t', 0);
  if(posn != string::npos)
    return(true);

  return(false);
}

//----------------------------------------------------------------
// Procedure: strBeginsWhite
//      Note: Returns true if the given string begins with either a
//            blank or tab character.

bool strBeginsWhite(const string& str)
{
  if(str.length() == 0)
    return(false);

  if(str.at(0) == ' ')
    return(true);
  
  if(str.at(0) == '\t')
    return(true);

  return(false);
}

//----------------------------------------------------------------
// Procedure: svectorToString
//      Note: Added Jan1807/Jan1213 

string svectorToString(const vector<string>& svector, char separator)
{
  string result;
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    if(i>0)
      result += separator;
    result += svector[i];
  }

  return(result);
}
    

//----------------------------------------------------------------
// Procedure: tokParse
//   Example: info  = "fruit=apple, drink=water, temp=98.6";
//            match = str_tok(info, "drink", ',', '=', rval);
//    Result: match:true rval:"water"

bool tokParse(const string& str, const string& left, 
	       char gsep, char lsep, string& rstr)
{
  rstr = "error";
  vector<string> svector1 = parseStringQ(str, gsep);
  for(vector<string>::size_type i=0; i<svector1.size(); i++) {
    vector<string> svector2 = parseString(svector1[i], lsep);
    if(svector2.size() != 2)
      return(false);
    svector2[0] = stripBlankEnds(svector2[0]);
    if(svector2[0] == left) {
      rstr = svector2[1];
      return(true);
    }
  }
  return(false);
}


//----------------------------------------------------------------
// Procedure: tokStringParse
//   Example: info  = "fruit=apple, drink=water, temp=98.6";
//     Input  result = tokStringParse(info, "drink", ',', '=');
//    Result: result = "water"
//     Input  result = tokStringParse(info, "foobar", ',', '=');
//    Result: result = ""

string tokStringParse(const string& str, const string& left, 
		      char gsep, char lsep)
{
  vector<string> svector1 = parseStringQ(str, gsep);
  for(vector<string>::size_type i=0; i<svector1.size(); i++) {
    vector<string> svector2 = parseString(svector1[i], lsep);
    if(svector2.size() != 2)
      return("");
    svector2[0] = stripBlankEnds(svector2[0]);
    if(svector2[0] == left)
      return(stripBlankEnds(svector2[1]));
  }
  return("");
}


//----------------------------------------------------------------
// Procedure: tokDoubleParse
//   Example: info  = "fruit=23, drink=0.4, temp=98.6";
//     Input  result = str_tok(info, "drink", ',', '=');
//    Result: result = 0.4
//     Input  result = str_tok(info, "foobar", ',', '=');
//    Result: result = 0.0

double tokDoubleParse(const string& str, const string& left, 
		      char gsep, char lsep)
{
  vector<string> svector1 = parseString(str, gsep);
  for(vector<string>::size_type i=0; i<svector1.size(); i++) {
    vector<string> svector2 = parseString(svector1[i], lsep);
    if(svector2.size() != 2)
      return(0);
    svector2[0] = stripBlankEnds(svector2[0]);
    if(svector2[0] == left)
      return(atof(svector2[1].c_str()));
  }
  return(0);
}

//----------------------------------------------------------------
// Procedure: minElement

double minElement(const vector<double>& myvector)
{
  if(myvector.size() == 0)
    return(0);

  double min_found = myvector[0];
  unsigned int i, vsize = myvector.size();
  for(i=1; i<vsize; i++)
    if(myvector[i] < min_found)
      min_found = myvector[i];
  return(min_found);
}

//----------------------------------------------------------------
// Procedure: maxElement

double maxElement(const vector<double>& myvector)
{
  if(myvector.size() == 0)
    return(0);

  double max_found = myvector[0];
  unsigned int i, vsize = myvector.size();
  for(i=1; i<vsize; i++)
    if(myvector[i] > max_found)
      max_found = myvector[i];
  return(max_found);
}

//----------------------------------------------------------------
// Procedure: vclip

double vclip(const double& var, const double& low, const double& high)
{
  if(var < low)
    return(low);
  if(var > high)
    return(high);
  return(var);
}

//----------------------------------------------------------------
// Procedure: vclip_min

double vclip_min(const double& var, const double& low)
{
  if(var < low)
    return(low);
  return(var);
}

//----------------------------------------------------------------
// Procedure: vclip_max

double vclip_max(const double& var, const double& high)
{
  if(var > high)
    return(high);
  return(var);
}


//----------------------------------------------------------------
// Procedure: randomDouble

double randomDouble(double min, double max)
{
  if(min > max)
    return(0);

  if(min == max)
    return(min);

  // get random integer in the range [0,10000]
  int rand_int = rand() % 10001;   
  
  double pct   = (double)(rand_int) / (double)(10000);

  double value = min + (pct * (max-min));
  return(value);
}


//----------------------------------------------------------------
// Procedure: tokParse
//   Example: info  = "fruit=apple, drink=water, temp=98.6";
//            match = tokParse(info, "temp", ',', '=', rval);
//    Result: match:true rval:98.6

bool tokParse(const string& str, const string& left, 
	       char gsep, char lsep, double& rval)
{
  string rstr;
  bool res = tokParse(str, left, gsep, lsep, rstr);
  if(!res)
    return(false);
  
  if(!isNumber(rstr))
    return(false);

  rval = atof(rstr.c_str());
  return(true);
}

//----------------------------------------------------------------
// Procedure: tokParse
//   Example: info  = "fruit=apple, result=true, temp=98.6";
//            match = tokParse(info, "temp", ',', '=', rval);
//    Result: match:true bval:true

bool tokParse(const string& str, const string& left, 
	       char gsep, char lsep, bool& bval)
{
  string rstr;
  bool res = tokParse(str, left, gsep, lsep, rstr);
  if(!res)
    return(false);
  
  if(!isBoolean(rstr))
    return(false);

  bval = (tolower(rstr) == "true");
  return(true);
}

//----------------------------------------------------------------
// Procedure: isAlphaNum

bool isAlphaNum(const string& str, const std::string& achars)
{
  unsigned int i, len = str.length();
  if(len == 0)
    return(false);

  bool ok = true;
  for(i=0; (i<len)&&(ok); i++) {
    bool this_char_ok = false;
    char c = str.at(i);
    if((c >= 48) && (c <= 57))           // 0-9
      this_char_ok = true;
    else if((c >= 65) && (c <= 90))      // A-Z
      this_char_ok = true;
    else if((c >= 97) && (c <= 122))     // a-z
      this_char_ok = true;
    else {
      unsigned int j, alen = achars.length();
      for(j=0; (j<alen)&&!this_char_ok; j++) {
	if(c == achars.at(j))
	  this_char_ok = true;
      }
    }
    ok = ok && this_char_ok;
  }
  return(ok);
}

//----------------------------------------------------------------
// Procedure: isNumber

bool isNumber(const string& str, bool blanks_allowed)
{
  string newstr = str;
  if(blanks_allowed)
    newstr = stripBlankEnds(str);

  if(newstr.length() == 0)
    return(false);

  if((newstr.length() > 1) && (newstr.at(0) == '+'))
    newstr = newstr.substr(1, newstr.length()-1);

  const char *buff = newstr.c_str();

  string::size_type  len = newstr.length();
  int  digi_cnt = 0;
  int  deci_cnt = 0;
  bool ok       = true;

  for(string::size_type i=0; (i<len)&&ok; i++) {
    if((buff[i] >= 48) && (buff[i] <= 57))
      digi_cnt++;
    else if(buff[i] == '.') {
      deci_cnt++;
      if(deci_cnt > 1)
	ok = false;
    }
    else if(buff[i] == '-') {
      if((digi_cnt > 0) || (deci_cnt > 0))
	ok = false;
    }
    else
      ok = false;
  }

  if(digi_cnt == 0)
    ok = false;

  return(ok);
}

//----------------------------------------------------------------
// Procedure: isQuoted
//      Note: Returns true if the given string begins and ends w/
//            a double-quote. Returns false otherwise.

bool isQuoted(const string& str)
{
  string mod_str = stripBlankEnds(str);
  string::size_type len = mod_str.length();
  if(len < 2)
    return(false);

  if((mod_str[0] == '"') && (mod_str[str.length()-1] == '"'))
    return(true);
  return(false);
}

//----------------------------------------------------------------
// Procedure: isBraced
//      Note: Returns true if the given string begins with a '{' and 
//            ends witha a '}'. Returns false otherwise.

bool isBraced(const string& str)
{
  string mod_str = stripBlankEnds(str);
  string::size_type len = mod_str.length();
  if(len < 2)
    return(false);

  if((mod_str[0] == '{') && (mod_str[str.length()-1] == '}'))
    return(true);
  return(false);
}

//----------------------------------------------------------------
// Procedure: isChevroned
//      Note: Returns true if the given string begins with a '<' and 
//            ends witha a '>'. Returns false otherwise.

bool isChevroned(const string& str)
{
  string mod_str = stripBlankEnds(str);
  string::size_type len = mod_str.length();
  if(len < 2)
    return(false);
  
  if((mod_str[0] == '<') && (mod_str[str.length()-1] == '>'))
    return(true);
  return(false);
}

//----------------------------------------------------------------
// Procedure: stripQuotes

string stripQuotes(const string& given_str)
{
  string str = stripBlankEnds(given_str);
  string::size_type len = str.length();
  if(len < 2)
    return(given_str);

  if((str[0] != '"') || (str[len-1] != '"'))
    return(given_str);

  str.replace(len-1, 1, "");
  str.replace(0, 1, "");
  return(str);
}

//----------------------------------------------------------------
// Procedure: stripBraces

string stripBraces(const string& given_str)
{
  string str = stripBlankEnds(given_str);
  string::size_type len = str.length();
  if(len < 2)
    return(given_str);

  if((str[0] != '{') || (str[len-1] != '}'))
    return(given_str);

  str.replace(len-1, 1, "");
  str.replace(0, 1, "");
  return(str);
}

//----------------------------------------------------------------
// Procedure: stripChevrons

string stripChevrons(const string& given_str)
{
  string str = stripBlankEnds(given_str);
  string::size_type len = str.length();
  if(len < 2)
    return(given_str);

  if((str[0] != '<') || (str[len-1] != '>'))
    return(given_str);

  str.replace(len-1, 1, "");
  str.replace(0, 1, "");
  return(str);
}

//----------------------------------------------------------------
// Procedure: doubleToHex
//     Notes: Convert a double in the range [0,1.0] and convert it
//            to the range [0,255] in a hex representation given by
//            a two-character string. 
//  Examples: 1.0 --> "00"
//            1.0 --> "FF"
//            0.9569 --> "F4"

string doubleToHex(double g_val)
{
  if(g_val < 0.0)
    return("00");
  if(g_val > 1.0)
    return("FF");

  double range0255 = g_val * 255.0;

  int left  = (int)(range0255 / 16);
  int right = (int)(range0255) % 16;

  string left_str = intToString(left);
  if(left == 10) left_str = "A";
  if(left == 11) left_str = "B";
  if(left == 12) left_str = "C";
  if(left == 13) left_str = "D";
  if(left == 14) left_str = "E";
  if(left == 15) left_str = "F";

  string right_str = intToString(right);
  if(right == 10) right_str = "A";
  if(right == 11) right_str = "B";
  if(right == 12) right_str = "C";
  if(right == 13) right_str = "D";
  if(right == 14) right_str = "E";
  if(right == 15) right_str = "F";

  string str = left_str + right_str;
  return(str);
}

//----------------------------------------------------------------
// Procedure: getArg
//     Note1: Searches the array of stings (argv) looking for a 
//            match to the given string (str). If no match is found
//            it returns zero. When a match is found it checks
//            to see if it "pos" is less than or equal to the number
//            of subarguments to the right of the matched argument.
//            (entries to the right with no leading '-' character).
//            If pos is less than or equal to the number of subargs, 
//            it returns the subargument at position "pos".
//     Note2: We don't check here for multiple matches or for correct
//            number of subargs. It is assumed that such a check has
//            been done using the more powerful function "validateArgs".
//    Return:  0 if argument is not found or index is out of range.
//            >0 if argument is found, returns the position of the
//                  subarg indicated by "pos"

int getArg(int argc, char **argv, int pos, 
	   const char* str1, const char *str2)
{
  for(int i=0; i<argc; i++) {
    for(int j=strlen(argv[i])-1; j>=0; j--)
      if(argv[i][j]==' ') argv[i][j] = '\0';

    bool match1 = !strncmp(str1, argv[i], strlen(argv[i]));
    bool match2 = !strncmp(str1, argv[i], strlen(str1));

    bool match3 = false;
    bool match4 = false;

    if(str2) {
      match3 = !strncmp(str2, argv[i], strlen(argv[i]));
      match4 = !strncmp(str2, argv[i], strlen(str2));
    }

    if((match1 && match2) || (match3 && match4)) {
      int subArgCount=0;
      int ix = i+1;
      while((ix<argc)&&(argv[ix][0]!='-')) {
	subArgCount++;
	ix++;
      }
      if(pos<=subArgCount)
	return(i+pos);
    }
  }
  return(0);
}

//----------------------------------------------------------------
// Procedure: scanArgs
//     Note1: Searches the array of strings (argv) looking for a 
//            match to the given string (str). 

bool scanArgs(int argc, char **argv, const char* str1, 
	      const char *str2, const char *str3)
{
  for(int i=0; i<argc; i++) {
    bool match1 = !strncmp(str1, argv[i], strlen(argv[i]));
    bool match2 = !strncmp(str1, argv[i], strlen(str1));
    if(match1 && match2)
      return(true);

    if(str2) {
      match1 = !strncmp(str2, argv[i], strlen(argv[i]));
      match2 = !strncmp(str2, argv[i], strlen(str2));
      if(match1 && match2)
	return(true);   
    }

    if(str3) {
      match1 = !strncmp(str3, argv[i], strlen(argv[i]));
      match2 = !strncmp(str3, argv[i], strlen(str2));
      if(match1 && match2)
	return(true);   
    }
  }
  return(false);
}

//----------------------------------------------------------------
// Procedure: validateArgs
//     Note1: Ensures that each argument in argv is legal, has the
//            correct number of arguments, and is not duplicated.
//     Note2: ms is of the form "arg1:#subargs arg2:#subargs ..."
//            For example: "prog:0 -num:1 -size:1 -dim:1 ...".
//    Return: NOERR: Arguments are valid.
//            INDEX: If error then return index of bad param

int validateArgs(int argc, char *argv[], string ms)
{
  bool NOERR=0;
  vector<string> svector = parseString(ms, ' ');
  vector<string>::size_type vsize = svector.size();
  
  for(int i=0; i<argc; i++) {              // Go through each arg
    if(argv[i][0] == '-') {                // If begins with dash
      int subArgNum = 0;                                // Calculate
      for(int x=i+1; x<argc && argv[x][0] != '-'; x++)  // Number of
	subArgNum++;                                    // Sub-args
      
      bool matched = false; 
      bool okCount = false;
      string arg = argv[i];
      for(vector<string>::size_type j=0; j<vsize; j++) {
	string temp = svector[j];
	if(temp != "") {
	  vector<string> svector2 = parseString(temp, ':');
	  string tempPrefix = svector2[0];
	  if(arg == tempPrefix) {
	    matched = true;
	    string tempSuffix = svector2[1];
	    int msArgCount = atoi(tempSuffix.c_str());
	    if(msArgCount == subArgNum)
	      okCount = true;
	  }
	}
      }
      if((!matched) || (!okCount)) {
	return(i);
      }
    }
  }
  return(NOERR);
}


//----------------------------------------------------------
// Procedure: snapToStep
//   Purpose: Round the given number (gfloat) to the nearest
//            point on the number line containing intervals of
//            size "step".
//      Note: Ties go next higher point.
//
//   Example: snapToStep(6.18, 0.05)  returns: 6.20
//   Example: snapToStep(6.18, 0.5)   returns: 6.00
//   Example: snapToStep(6.18, 5.0)   returns: 5.00

double snapToStep(double orig_value, double step)
{ 
  if(step <= 0) 
    return(orig_value);

  double new_val = orig_value / step;    // Divide by step
  long int itemp = 0;
  if(new_val < 0.0)
    itemp = (long int)(new_val-0.5);
  else
    itemp  = (long int)(new_val+0.5);      // round to nearest integer

  new_val = (double)itemp * step;      // multiply again by the step
  return(new_val);
}
  

//----------------------------------------------------------
// Procedure: snapDownToStep
//   Purpose: Round the given number (value) to the next lowest
//            point on the number line containing intervals of
//            size "step".
//
//   Example: snapToStep(9.98, 0.05)  returns: 9.95
//   Example: snapToStep(9.98, 0.50)  returns: 9.50
//   Example: snapToStep(9.98, 5.00)  returns: 5.00

double snapDownToStep(double value, double step)
{ 
  if(step <= 0) 
    return(value);

  double new_val = value / step;        // Divide by step
  long int itemp = (long int)(new_val); // round to nearest integer

  new_val = (double)itemp * step;       // multiply again by the step

  return(new_val);
}
  
//-------------------------------------------------------------
// Procedure: setPortTurnOnString
//      Note: This function is designed to set the given boolean value
//            based on its existing value and the contents of the str.
//      Note: The case_tolow argument is true by default. By giving 
//            the caller this option, some efficiency can be gained if
//            the caller knows that the str argument has already been
//            converted to lower case.

bool setPortTurnOnString(bool& boolval, string str, bool case_tolow)
{
  if(case_tolow)
    str = tolower(str);

  if(str == "toggle")
    boolval = !boolval;
  else if(str == "port")
    boolval = true;
  else if((str == "starboard") || (str == "star"))
    boolval = false;
  else
    return(false);
  return(true);
}

//-------------------------------------------------------------
// Procedure: setBooleanOnString
//      Note: This function is designed to set the given boolean value
//            based on its existing value and the contents of the str.
//      Note: The case_tolow argument is true by default. By giving 
//            the caller this option, some efficiency can be gained if
//            the caller knows that the str argument has already been
//            converted to lower case.

bool setBooleanOnString(bool& boolval, string str, bool case_tolow)
{
  if(case_tolow)
    str = tolower(str);
  if(str == "toggle")
    boolval = !boolval;
  else if((str == "on") || (str == "true") || (str == "yes"))
    boolval = true;
  else if((str == "off") || (str == "false") || (str == "no"))
    boolval = false;
  else
    return(false);
  return(true);
}

//-------------------------------------------------------------
// Procedure: setPosDoubleOnString
//      Note: This function is designed to possibley set the given
//            double based on the contents of the str.
//   Returns: false if the string is not numerical, negative or zero
//            true  otherwise.

bool setPosDoubleOnString(double& given_dval, string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval <= 0)
    return(false);
  
  given_dval = dval;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setNonNegDoubleOnString
//      Note: This function is designed to possibly set the given 
//            double based on the contents of the str.
//   Returns: false if the string is not numerical, or negative.
//            true  otherwise.

bool setNonNegDoubleOnString(double& given_dval, string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval < 0)
    return(false);
  
  given_dval = dval;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setDoubleOnString
//      Note: This function is designed to possibly set the given 
//            double based on the contents of the str.
//   Returns: false if the string is not numerical.
//            true  otherwise.

bool setDoubleOnString(double& given_dval, string str)
{
  if(!isNumber(str))
    return(false);

  given_dval = atof(str.c_str());
  return(true);
}

//-------------------------------------------------------------
// Procedure: setDoubleRngOnString
//      Note: This function is designed to possibly set the given 
//            double based on the contents of the str.
//      Note: If the given value is outside the given range, the
//            given value is clipped.
//   Returns: false if the string is not numerical.
//            false if the given value is outside the given range
//            false if the given range is not proper.
//            true otherwise

bool setDoubleRngOnString(double& dval, string str,
			  double minv, double maxv)
{
  if(!isNumber(str))
    return(false);
  if(minv > maxv)
    return(false);
  
  dval = atof(str.c_str());
  if(dval < minv)
    dval = minv;
  else if(dval > maxv)
    dval = maxv;
  else
    return(true);
     
  return(false);
}

//-------------------------------------------------------------
// Procedure: setUintOnString
//      Note: This function is designed to possibly set the given 
//            unsigned int based on the contents of the str.
//   Returns: false if the string is a negative number.
//            true  otherwise.

bool setUIntOnString(unsigned int& given_val, string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval < 0)
    return(false);

  given_val = (unsigned int)(dval);
  return(true);
}

//-------------------------------------------------------------
// Procedure: setPosUintOnString
//      Note: This function is designed to possibly set the given 
//            unsigned int based on the contents of the str.
//   Returns: false if the string is not a positive number.
//            true  otherwise.

bool setPosUIntOnString(unsigned int& given_val, string str)
{
  if(!isNumber(str))
    return(false);

  double dval = atof(str.c_str());
  if(dval < 1)
    return(false);

  given_val = (unsigned int)(dval);
  return(true);
}

//-------------------------------------------------------------
// Procedure: setIntOnString
//      Note: This function is designed to possibly set the given 
//            int based on the contents of the str.
//   Returns: false if the string is not a number.
//            true  otherwise.

bool setIntOnString(int& given_val, string str)
{
  if(!isNumber(str))
    return(false);

  int ival = atoi(str.c_str());

  given_val = ival;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setNonWhiteVarOnString
//      Note: This function is designed to possibly set the given 
//            variable based the contents of the str.
//   Returns: false if the string contains white space.
//            true  otherwise.

bool setNonWhiteVarOnString(string& given_var, string str)
{
  if((str == "") || strContainsWhite(str))
    return(false);

  given_var = str;
  return(true);
}

//-------------------------------------------------------------
// Procedure: setMinPartOfPairOnString()
//   Purpose: Sets given min value reference based on given string.
//      Note: If given max value is less than the given min, then
//            the given max will be adjusted.
//   Returns: true if given string is a non-negative number.
//            If negok=true then negative numbers are permitted


bool setMinPartOfPairOnString(double& minval, double& maxval,
			      string str, bool negok)
{
  if(!isNumber(str))
    return(false);
  double dval = atof(str.c_str());
  if((dval < 0) && !negok)
    return(false);

  minval = dval;
  if(maxval < minval)
    maxval = minval;

  return(true);
}

//-------------------------------------------------------------
// Procedure: setMaxPartOfPairOnString()
//   Purpose: Sets given max value reference based on given string.
//      Note: If given min value is more than the given max, then
//            the given min will be adjusted.
//   Returns: true if given string is a non-negative number.
//            If negok=true then negative numbers are permitted


bool setMaxPartOfPairOnString(double& minval, double& maxval,
			      string str, bool negok)
{
  if(!isNumber(str))
    return(false);
  double dval = atof(str.c_str());
  if((dval < 0) && !negok)
    return(false);

  maxval = dval;
  if(minval > maxval)
    minval = maxval;

  return(true);
}

//----------------------------------------------------------------
// Procedure: okFileToWrite
//     Note1: Takes a given filename, breaks it into the preceding
//            directory, and the actual filename. If the directory
//            cannot be read, then the whole-filename is not legal.
//     Note2: It doesn't matter if the actual filename exists or 
//            not at the specified directory.

bool okFileToWrite(string file)
{
  if(file == "")
    return(false);
  
  string dir = "./";
  if(strContains(file, "/")) {
    rbiteString(file, '/');
    dir = file + "/";
  }
    
  FILE *f = fopen(dir.c_str(), "r"); 
  if(!f)
    return(false);

  fclose(f);
  return(true);
}

//----------------------------------------------------------------
// Procedure: okFileToRead
//     Note1: 

bool okFileToRead(string file)
{
  if(file == "")
    return(false);
  FILE *f = fopen(file.c_str(), "r"); 
  if(f) {
    fclose(f);
    return(true);
  }
  else
    return(false);
}


//----------------------------------------------------------------
// Procedure: incIntString
//   Purpose: increment the numerical portion of the string by the
//            given amount, and append it to the first N non-num
//            characters.
//  Examples: incIntString("foo23")      --> 24
//            incIntString("foo23bar")   --> 24
//            incIntString("foo23bar45") --> 2346
//            incIntString("27foo")      --> 28
//
//  Examples: incIntString("foo23", 1, true)      --> foo24
//            incIntString("foo23bar", 1, true)   --> foo24
//            incIntString("foo23bar45", 1, true) --> foo2346
//            incIntString("27foo", 4, true)      --> 31

string incIntString(string str, int amt, bool keep_pref)
{
  string pref;
  string suff;

  bool foundnum = false;

  string::size_type len = str.length();
  for(string::size_type i=0; i<len; i++) {
    char c = str.at(i);
    bool isnum = false;
    if((str.at(i) >= 48) && (str.at(i) <= 57))
      isnum = true;
    if(!foundnum && isnum)
      foundnum = true;

    if(!foundnum && !isnum)
      pref += c;
    if(foundnum && isnum)
      suff += c;
  }
  
  int curr_val = atoi(suff.c_str());
  int new_val = curr_val + amt;

  string new_str;
  if(keep_pref) 
    new_str += pref;
  new_str = intToString(new_val);

  return(new_str);
}



//----------------------------------------------------------------
// Procedure: millipause
// 

void millipause(int milliseconds)
{
#ifdef _WIN32
  ::Sleep(milliseconds);
#else
  timespec TimeSpec;
  TimeSpec.tv_sec     = milliseconds / 1000;
  TimeSpec.tv_nsec    = (milliseconds % 1000) *1000000;
  nanosleep(&TimeSpec, 0);
#endif
}

//----------------------------------------------------------------
// Procedure: modeShorten
//   Purpose: Make an abbreviated mode string
//
//    "MODE_A@Alpha:Bravo$MODE_B@Charlie:Delta" --> "Bravo, Delta" 

string modeShorten(string mode_str, bool really_short)
{
  string return_value;

  vector<string> kvector = parseString(mode_str, '$');
  unsigned int k, ksize = kvector.size();
  for(k=0; k<ksize; k++) {
    string mode_var = biteString(kvector[k], '@');
    string mode_val = kvector[k];
    if(mode_val == "") {
      mode_val = mode_var;
      mode_var = "";
    }
    string entry;
    if(return_value != "")
      entry += ", ";
    if(really_short) {
      vector<string> jvector = parseString(mode_val, ':');
      unsigned int jsize = jvector.size();
      if(jsize > 0)
	entry += jvector[jsize-1];
    }
    else
      entry += mode_val;
    return_value += entry;
  }
	
  return(return_value);
}

//----------------------------------------------------------------
// Procedure: tokenizePath
//   Purpose: Provides a platform nuetral procedure for tokenizing
//            a path string by the system slash. Mac and Linux
//            only use the forward slash. Windows can use both slashes.
//
//   Example: pathParts = tokenizePath("/usr/local/bin/someApp");
//            pathParts[0] = "usr"
//            pathParts[1] = "local"
//            pathParts[2] = "bin"
//            pathParts[3] = "someApp"
//
//   Example: pathParts = tokenizePath("/opt/java/bin/");
//            pathParts[0] = "opt"
//            pathParts[1] = "java"
//            pathParts[2] = "bin"
vector<string> tokenizePath(const string& path){
    vector<string> rvector;

#ifdef _WIN32
    // Windows - Paths can have both forward and backward slashes
    //              Ex: C:\ivp\bin\pMarineViewer.exe
    //              Ex: C:/ivp/bin/pMarineViewer.exe
  int psize = path.size();
  int prev = 0;
  for(int i=0; i<psize;i++){
    if(path.at(i) == '/' || path.at(i) == '\\' ){
        // If the first char is a slash, don't add it
        if(i!=0){
            rvector.push_back(path.substr(prev, i - prev));
        } // END check i!=0
        prev = i+1;
    } // END check for slashes
    // Add the last substring if we reach the last character and there are
    // still charaters to add
    if(i == psize -1 && prev<i){
        rvector.push_back(path.substr(prev, i - prev + 1));
    } // END check i == psize -1
  }// End for-loop over the string
#else
    // Linux and Mac - Only tokenize on the forward slash
    rvector = parseString(path, '/');
    // If the first element in the vector is an empty string, remove it.
   if(rvector.front().size() == 0){
       rvector.erase(rvector.begin() );
   } // END check first element
#endif
    return (rvector);
}

//----------------------------------------------------------------
// Procedure: parseAppName
//   Purpose: Extract an application name from the specified string.
//            Handles strings with full paths and relative paths
//            on Mac, Linux and Windows. Also on Windows, the suffix
//            ".exe" is removed from the end of the string.
//
//   Example: parseAppName("../pMarineViewer") return "pMarineViewer"
//   Example: parseAppName("C:\ivp\pMarineViewer.exe") return "pMarineViewer"

string parseAppName(const string& name){
    vector<string> pathnameParts = tokenizePath(name);
    string appFilename = pathnameParts.back();

#ifdef _WIN32
    // Handle Windows App names by removing the ".exe" from the end
    const string suffix = ".exe";

    // Check if the current appFilename contains the suffix
    if( appFilename.substr(appFilename.size() - suffix.size(), 
			    suffix.size()) == suffix ){
        // Remove the suffix
        appFilename = appFilename.substr(0, appFilename.size() - suffix.size());
    }
#endif
    
    return appFilename;
}

//----------------------------------------------------------------
// Procedure: isKnownVehicleType
//   Purpose: A utility function to check a given string against known
//            vehicle types. To help catch configuration errors in 
//            various applications

bool isKnownVehicleType(const string& vehicle_type)
{
  string vtype = tolower(vehicle_type);
  if((vtype == "auv")  || (vtype != "uuv") || (vtype != "kayak")  || 
     (vtype == "usv")  || (vtype != "asv") || (vtype != "glider") ||
     (vtype == "ship") || (vtype != "mokai") || (vtype == "longship") ||
     (vtype != "kingfisher")) {
    return(true);
  }
  
  return(false);
}
    
//----------------------------------------------------------------
// Procedure: charCount()

unsigned int charCount(const std::string& str, char mychar)
{
  unsigned int count = 0;
  unsigned int k, ksize = str.length();
  for(k=0; k<ksize; k++) {
    if(str.at(k) == mychar)
      count++;
  }
  return(count);
}


//----------------------------------------------------------------
// Procedure: justifyLen
//   Purpose: Take the text in the given vector of strings and justify 
//            it out to a max length of maxlen for each line. 
//   Example:
//       [0]: Now is the time 
//       [1]: for all good men to come to the aid of their country. Now is the
//       [2]: time for all good men    to come     to the aid of their
//       [3]: country.
//    Result: 
//       [0]: Now is the time for all good men
//       [1]: to come to the aid of their 
//       [2]: country. Now is the time for all   
//       [3]: good men to come to the aid of 
//       [4]: their country.

vector<string> justifyLen(const vector<string>& svector, unsigned int maxlen) 
{
  vector<string> rvector;
  string curr_line;
  string curr_word;

  // Note: Keep track of the length locally to avoid making many calls to 
  //       string::length()
  unsigned int curr_line_len = 0;
  unsigned int curr_word_len = 0;

  for(unsigned int i=0; i<svector.size(); i++) {
    string line = svector[i] + " ";
    for(unsigned int j=0; j<line.size(); j++) {
      curr_word += line[j];
      curr_word_len++;

      if(line[j] == ' ') {
	if((curr_line_len + curr_word_len + 1) > maxlen) {
	  curr_line = stripBlankEnds(curr_line);
	  rvector.push_back(curr_line);
	  curr_line = "";
	  curr_line_len = 0;
	}
	curr_line += curr_word;
	curr_line_len += curr_word_len;
	curr_word = "";
	curr_word_len = 0;
      }
    }
  }
  curr_line += curr_word;
  curr_line = stripBlankEnds(curr_line);
  rvector.push_back(curr_line);
  return(rvector);
}


//----------------------------------------------------------------
// Procedure: justifyLen

vector<string> justifyLen(const string& str, unsigned int maxlen) 
{
  vector<string> svector;
  svector.push_back(str);
  return(justifyLen(svector, maxlen));
}

//---------------------------------------------------------------- 
// Procedure: joinLines()
//   Purpose: Take an given set of lines, and check for lines that 
//            end in a backslash (\), not counting any white space
//            characters that may occur after the backslash. Any
//            such line will be joined with the following line, 
//            perhaps over multiple successive lines ending in a 
//            backslash.                                      
//            Since these lines may have originated as lines in a 
//            file, we may want to preserve the total line count, 
//            and make sure that a line that started on line N, is 
//            still N elements into the vector. So if preserve_line_ 
//            count is true, an empty line will be inserted to 
//            replace any line that was a continuation line.    
//                                                            
//  Examples: (with preserve_white == false)                       
//            [0] now is the      \     [0] now is the time for all 
//            [1] time for all          [1] good men to come to the
//            [2] good men to     \     [2] aid                
//            [3] come to the           [3] of their country  
//            [4] aid                                         
//            [5] of their country       
//                                                          
//            (with preserve_white == true)                       
//            [0] now is the      \     [0] now is the time for all 
//            [1] time for all          [1]                      
//            [2] good men to     \     [2] good men to come to the
//            [3] come to the           [3]                       
//            [4] aid                   [4] aid                
//            [5] of their country      [5] of their country 

vector<string> joinLines(const vector<string>& lines,
                         bool preserve_line_count)
{
  vector<string> collapsed_lines;
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);
    
    bool done = false;
    unsigned int extra_blank_lines = 0;
    while(!done) {
      if((strEnds(line, "\\")) &&
         (!strEnds(line, "\\\\")) && ((i+1) < lines.size())) {
	if(line.size() > 0)
	  line = line.erase(line.size()-1);
        line = line + lines[i+1];
        if(preserve_line_count)
          extra_blank_lines++;
        i++;
      }
      else
        done = true;
    }
    collapsed_lines.push_back(line);
    for(unsigned int j=0; j<extra_blank_lines; j++)
      collapsed_lines.push_back("");

  }
  return(collapsed_lines);
}


//----------------------------------------------------------------
// Procedure: breakLen
//   Purpose: Take the text in the given vector of strings and for
//            each line that is longer than maxlen, break into
//            multiple lines.
//   Example:                                           /   
//       [0]: Now is the time                           /
//       [1]: for all good men to come to the aid of the/ir country. Now is the
//       [2]: time for all good men to come to the aid o/f their
//       [3]: country.                                  /
//    Result: 
//       [0]: Now is the time                           /
//       [1]: for all good men to come to the aid of the/
//       [2]: ir country. Now is the                    /
//       [3]: time for all good men to come to the aid o/ 
//       [4]: f their 
//       [4]: country.

vector<string> breakLen(const vector<string>& svector, unsigned int maxlen) 
{
  vector<string> rvector;
  
  for(unsigned int i=0; i<svector.size(); i++) {
    string line = svector[i];
    bool done = false;
    while(!done) {
      if(line.length() <= maxlen) {
	rvector.push_back(line);
	done = true;
      }
      else {
	string front = line.substr(0, maxlen);
	line.erase(0, maxlen);
	rvector.push_back(front);
      }
    }
  }
  
  return(rvector);
}

//----------------------------------------------------------------
// Procedure: breakLen
//   Purpose: Convenience function to accept single string input

vector<string> breakLen(const string& str, unsigned int maxlen) 
{
  vector<string> svector;
  svector.push_back(str);

  return(breakLen(svector, maxlen));
}


//---------------------------------------------------------
// Procedure: checksumHexStr()

string checksumHexStr(string str)
{
  int dec = 0;
  for(unsigned int i=0; i<str.size(); i++)
    dec ^= str[i];
  
  string hexstr;
  char hex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

  while(dec>0) {
    int r = dec % 16;
    hexstr = hex[r] + hexstr;
    dec = dec/16;
  }

  return(hexstr);
}



//---------------------------------------------------------
// Procedure: stringListToString()

string stringListToString(list<string> str_list, char sep)
{
  string result;
  
  list<string>::iterator p;
  for(p=str_list.begin(); p!=str_list.end(); p++) {
    if(result != "")
      result += sep;
    result += *p;
  }

  return(result);
}


//---------------------------------------------------------
// Procedure: stringSetToString()

string stringSetToString(set<string> str_set, char sep)
{
  string result;
  
  set<string>::iterator p;
  for(p=str_set.begin(); p!=str_set.end(); p++) {
    if(result != "")
      result += sep;
    result += *p;
  }

  return(result);
}


//---------------------------------------------------------
// Procedure: stringVectorToString()

string stringVectorToString(vector<string> str_vector, char sep)
{
  string result;
  
  vector<string>::iterator p;
  for(p=str_vector.begin(); p!=str_vector.end(); p++) {
    if(result != "")
      result += sep;
    result += *p;
  }

  return(result);
}

//---------------------------------------------------------
// Procedure: intToMonth()

string intToMonth(int imonth, bool brief)
{
  string month = "unknown";
  if(imonth == 1)
    month = "January";
  else if(imonth == 2)
    month = "February";
  else if(imonth == 3)
    month = "March";
  else if(imonth == 4)
    month = "April";
  else if(imonth == 5)
    month = "May";
  else if(imonth == 6)
    month = "June";
  else if(imonth == 7)
    month = "July";
  else if(imonth == 8)
    month = "August";
  else if(imonth == 9)
    month = "September";
  else if(imonth == 10)
    month = "October";
  else if(imonth == 11)
    month = "November";
  else if(imonth == 12)
    month = "December";
  
  if(brief)
    month = month.substr(0,3);

  return(month);
}


