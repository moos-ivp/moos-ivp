/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MBUtils.cpp                                          */
/*    DATE: (1996-2005)                                          */
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

  const char *str = string_str.c_str();
  char *buff = new char[strlen(str)+1];
  vector<string> rvector;
  while(str[0] != '\0') {    
    int i=0;
    while(((str[i]!=separator) || in_quotes) && (str[i]!='\0')) {
      if(str[i]=='"')
	in_quotes = !in_quotes;
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
// Procedure: biteString(const string&, char)
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
// Procedure: biteStringX(const string&, char)
//      Note: Same as biteString except blank ends will be removed
//            from both the returned and remaining value.

string biteStringX(string& str, char separator)
{
  string front = stripBlankEnds(biteString(str, separator));
  str = stripBlankEnds(str);
  return(front);
}

//----------------------------------------------------------------
// Procedure: biteString(const string&, char, char)
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
//      Note: 

bool vectorContains(const vector<string>& svector, const string& str)
{
  vector<string>::size_type vsize = svector.size();
  for(vector<string>::size_type i=0; i<vsize; i++)
    if(svector[i] == str)
      return(true);
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
  
  if(val > (double)(pow((double)(2),(double)(128))))
    format[3] = 'e';

  char buff[1000];
  sprintf(buff, format, val);
  string str = buff;
  return(str);
}

string doubleToStringX(double val, int digits)
{
  return(dstringCompact(doubleToString(val, digits)));
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

  for(string::size_type j=ssize-1; j>=0; j--) {
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
// Procedure: packageToString
//      Note: Added Jan 18 07 

string packageToString(const vector<string>&)
{
  string result;
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
  vector<string> svector1 = parseString(str, gsep);
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
  vector<string> svector1 = parseString(str, gsep);
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
// Procedure: tokParse
//   Example: info  = "fruit=apple, drink=water, temp=98.6";
//            match = str_tok(info, "temp", ',', '=', rval);
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
// Procedure: isAlphaNum

bool isAlphaNum(const string& str, const std::string& achars)
{
  unsigned int i, len = str.length();
  bool ok = false;
  for(i=0; i<len; i++) {
    char c = str.at(i);
    if((c >= 48) && (c <= 57))
      ok = true;
    else if((c >= 65) && (c <= 90))
      ok = true;
    else if((c >= 97) && (c <= 122))
      ok = true;
    else {
      unsigned int j, alen = achars.length();
      for(j=0; (j<alen)&&!ok; j++) {
	if(c == achars.at(j))
	  ok = true;
      }
    }
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
  if((mod_str[0] == '{') && (mod_str[str.length()-1] == '}'))
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

float snapToStep(float gfloat, float step)
{ 
  if(step <= 0) 
    return(gfloat);
  float fval   = gfloat / step;    // Divide by step
  int   itemp;
  if(fval < 0.0)
    itemp = (int)(fval-0.5);
  else
    itemp  = (int)(fval+0.5);      // round to nearest integer
  fval = (float)itemp * step;      // multiply again by the step
  return(fval);
}
  

//----------------------------------------------------------
// Procedure: snapDownToStep
//   Purpose: Round the given number (gfloat) to the next lowest
//            point on the number line containing intervals of
//            size "step".
//
//   Example: snapToStep(9.98, 0.05)  returns: 9.95
//   Example: snapToStep(9.98, 0.50)  returns: 9.50
//   Example: snapToStep(9.98, 5.00)  returns: 5.00

float snapDownToStep(float gfloat, float step)
{ 
  if(step <= 0) return(gfloat);
  float fval   = gfloat / step;    // Divide by step
  int   itemp  = (int)(fval);      // round to nearest integer
  fval = (float)itemp * step;      // multiply again by the step
  return(fval);
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
  else if((str == "on") || (str == "true"))
    boolval = true;
  else if((str == "off") || (str == "false"))
    boolval = false;
  else
    return(false);
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
  string dir = "/";
  vector<string> svector = parseString(file, '/');
  for(vector<string>::size_type i=0; i<svector.size()-1; i++) {
    string temp = svector[i];
    dir += temp;
    dir += "/";
  }
  FILE *f = fopen(dir.c_str(), "r"); 
  if(f) {
    fclose(f);
    return(true);
  }
  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: okFileToRead
//     Note1: 

bool okFileToRead(string file)
{
  FILE *f = fopen(file.c_str(), "r"); 
  if(f) {
    fclose(f);
    return(true);
  }
  else
    return(false);
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
// Procedure: getReleaseInfo
// 

vector<string> getReleaseInfo(const string& app)
{
  string pad = padString("", (16-app.length()));
  vector<string> v;
  v.push_back("************************************************************************");
  v.push_back("* " + app + " - MOOS-IvP Release Bundle - VERSION 4.1+HEAD" + pad + "  r3431 *");
  v.push_back("* M.Benjamin (NUWC/MIT), P.Newman (Oxford), Schmidt and Leonard (MIT)  *");
  v.push_back("* Copyright (C) 2008 Free Software Foundation, Inc.                    *");
  v.push_back("* This is free software; see the source for copying conditions.        *");
  v.push_back("************************************************************************");
  v.push_back("");
  return(v);
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
