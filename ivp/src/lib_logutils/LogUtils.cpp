/*****************************************************************/
/*    NAME: Michael Benjamin and Henrik Schmidt                  */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogUtils.cpp                                         */
/*    DATE: August 7th, 2008                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "MBUtils.h"
#include "LogUtils.h"

using namespace std;

//--------------------------------------------------------
// Procedure: getVarName
//     Notes: Syntax:  "TIMESTAMP  VAR  SOURCE  DATA"


string getVarName(const string& line)
{
  int  len   = line.length();
  bool done  = false;
  int  state = 0;


  const int MAX_LINE_LENGTH = 50000;

  int  buffix  = 0;
  char buff[MAX_LINE_LENGTH];

  for(int i=0; ((i<len) && !done); i++) {
    if((line[i] == ' ') || (line[i] == '\t')) {
      if(state == 0)
	state = 1;
      else if(state == 2) {
	buff[buffix] = '\0';
	done = true;
      }
    }
    else {
      if(state == 1)
	state = 2;
      if(state == 2) {
	buff[buffix] = line[i];
	buffix++;
      }
    }
  }
  string str = buff;
  return(str);
}

//--------------------------------------------------------
// Procedure: getTimeStamp
//     Notes: Syntax:  "TIMESTAMP  VAR  SOURCE  DATA"

string getTimeStamp(const string& line)
{
  int  len   = line.length();
  bool done  = false;

  const int MAX_LINE_LENGTH = 50000;

  int  buffix  = 0;
  char buff[MAX_LINE_LENGTH];

  for(int i=0; ((i<len) && !done); i++) {
    if((line[i] == ' ') || (line[i] == '\t')) {
      buff[buffix] = '\0';
      done = true;
    }
    else {
      buff[buffix] = line[i];
      buffix++;
    }
  }
  string str = buff;
  return(str);
}

//--------------------------------------------------------
// Procedure: getDataEntry
//     Notes: Syntax:  "TIMESTAMP  VAR  SOURCE  DATA"

string getDataEntry(const string& line)
{
  int len = line.length();
  
  bool done = false;
  
  int state = 0;

  const int MAX_LINE_LENGTH = 50000;

  int  buffix  = 0;
  char buff[MAX_LINE_LENGTH];

  for(int i=0; (i<len); i++) {
    if((line[i] == ' ') || (line[i] == '\t')) {
      if(state == 0)
	state = 1;
      else if(state == 2)
	state = 3;
      else if(state == 4)
	state = 5;
      if(state == 6) {
	buff[buffix] = line[i];
	buffix++;
      }
    }
    else {
      if(state == 1)
	state = 2;
      else if(state == 3)
	state = 4;
      else if(state == 5)
	state = 6;
      if(state == 6) {
	buff[buffix] = line[i];
	buffix++;
      }
    }
  }
  buff[buffix] = '\0';
  string str = buff;
  return(str);
}


//--------------------------------------------------------
// Procedure: stripInsigDigits
//     Notes: 

void stripInsigDigits(string& line)
{
  bool seen_zero = false;

  bool done  = false;
  int  endix = line.length() - 1;

  while(!done) {
    if(line.at(endix) == '0') {
      seen_zero = true;
      // Check to make sure this zero isn't just a solitary zero, in which
      // case we want to leave it there.
      if((endix > 0)&&(line.at(endix-1)!=' ') && (line.at(endix-1)!='\t'))
	line[endix] = '\0';
      else
	done = true;
    }
    else if(line.at(endix) == '.') {
      if(seen_zero == true)
	line[endix] = '\0';
      done = true;
    }
    else
      done = true;
    endix--;
  }
}


//--------------------------------------------------------
// Procedure: shiftTimeStamp
//     Notes: 

void shiftTimeStamp(string& line, double logstart)
{
  string timestamp = getTimeStamp(line);
  if(!isNumber(timestamp))
    return;

  double dtime = atof(timestamp.c_str());

  if((dtime+1000) > logstart)
    dtime -= logstart;

  string stime = doubleToString(dtime, 3);

  line = findReplace(line, timestamp, stime);
}

//--------------------------------------------------------
// Procedure: getLogStart
//     Notes: Syntax "%% LOGSTART  TIMESTAMP"

double getLogStart(const string& line)
{
  int  len   = line.length();
  bool done  = false;
  int  state = 0;

  const int MAX_LINE_LENGTH = 5000;

  int  buffix  = 0;
  char buff[MAX_LINE_LENGTH];

  for(int i=0; ((i<len) && !done); i++) {
    if((line[i] == ' ') || (line[i] == '\t')) {
      if(state == 0)
	state = 1;
      else if(state == 2)
	state = 3;
      else if(state == 4) {
	buff[buffix] = '\0';
	done = true;
      }
    }
    else {
      if(state == 1)
	state = 2;
      else if(state == 3)
	state = 4;
      if(state == 4) {
	buff[buffix] = line[i];
	buffix++;
      }
    }
  }
  return(atof(buff));
}


//--------------------------------------------------------
// Procedure: addVectorKey
//     Notes: 

void addVectorKey(vector<string>& v_keys, vector<bool>& v_pmatch,
		  string key)
{
  bool pmatch = false;
  int len = key.length();
  if(key.at(len-1) == '*') {
    pmatch = true;
    key.erase(len-1, 1);
  }
  
  int  ksize = v_keys.size();
  bool prior = false;
  int  prior_ix = -1;
  for(int i=0; i<ksize; i++) {
    if(key == v_keys[i]) {
      prior = true;
      prior_ix = i;
    }
  }
  
  if(!prior) {
    v_keys.push_back(key);
    v_pmatch.push_back(pmatch);
  }

  if(prior && pmatch && !v_pmatch[prior_ix])
    v_pmatch[prior_ix] = true;
}


//--------------------------------------------------------
// Procedure: getNextRawLine
//     Notes: 

string getNextRawLine(FILE *fileptr)
{
  if(!fileptr) {
    cout << "failed getNextLine() - null file pointer" << endl;
    return("err");
  }
  
  const int MAX_LINE_LENGTH = 50000;

  bool EOL     = false;
  int  buffix  = 0;
  int  myint   = '\0';
  char buff[MAX_LINE_LENGTH];

  while((!EOL) && (buffix < MAX_LINE_LENGTH)) {
    myint = fgetc(fileptr);
    unsigned char mychar = myint;
    switch(myint) {
    case EOF:
      fclose(fileptr);
      fileptr = 0;
      return("eof");
    case '\n':
      buff[buffix] = '\0';  // attach terminating NULL
      EOL = true;
      break;
    default:
      buff[buffix] = mychar;
      buffix++;
    }
  }
  string str = buff;  
  return(str);
}

