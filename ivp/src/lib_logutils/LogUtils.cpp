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
#include <cstdio>
#include "MBUtils.h"
#include "LogUtils.h"
#include <cstdio>

#define MAX_LINE_LENGTH 50000

using namespace std;


//--------------------------------------------------------
// Procedure: getTimeStamp
//     Notes: Syntax:  "TIMESTAMP  VAR  SOURCE  DATA"

string getTimeStamp(const string& line)
{
  unsigned int i, len = line.length();

  bool   done = false;
  string str;

  for(i=0; ((i<len) && !done); i++) {
    if((line[i] == ' ') || (line[i] == '\t'))
      done = true;
    else
      str.push_back(line[i]);
  }
  return(str);
}

//--------------------------------------------------------
// Procedure: getVarName
//     Notes: Syntax:  "TIMESTAMP   VAR   SOURCE   DATA"
//            States:      0      1  2  3   4    5

string getVarName(const string& line)
{
  unsigned int i, len = line.length();

  bool   done  = false;
  int    state = 0;
  string str;

  for(i=0; ((i<len) && !done); i++) {
    if((line[i] == ' ') || (line[i] == '\t')) {
      if(state == 0)
	state = 1;
      else if(state == 2)
	done = true;
    }
    else {
      if(state == 1) {
	str.push_back(line[i]);
	state = 2;
      }
      else if(state == 2)
	str.push_back(line[i]);
    }
  }
  return(str);
}

//--------------------------------------------------------
// Procedure: getSourceName
//     Notes: Syntax:  "TIMESTAMP   VAR   SOURCE   DATA"
//            States:      0      1  2  3   4    5

string getSourceName(const string& line)
{
  unsigned int i, len = line.length();

  bool   done  = false;
  int    state = 0;
  string str;


  for(i=0; ((i<len) && !done); i++) {
    if((line[i] == ' ') || (line[i] == '\t')) {
      if(state == 0)
	state = 1;
      else if(state == 2)
	state = 3;
      else if(state == 4)
	done = true;
    }
    else {
      if(state == 1)
	state = 2;
      else if(state == 3) {
	str.push_back(line[i]);
	state = 4;
      }
      else if(state == 4)
	str.push_back(line[i]);
    }
  }
  return(str);
}

//--------------------------------------------------------
// Procedure: getDataEntry
//     Notes: Syntax:  "TIMESTAMP   VAR   SOURCE   DATA"
//            States:      0      1  2  3   4    5   6

string getDataEntry(const string& line)
{
  unsigned int i, len = line.length();

  int    state = 0;
  string str;

  for(i=0; (i<len); i++) {
    if((line[i] == ' ') || (line[i] == '\t')) {
      if(state == 0)
	state = 1;
      else if(state == 2)
	state = 3;
      else if(state == 4)
	state = 5;
      else if(state == 6) 
	str.push_back(line[i]);
    }
    else {
      if(state == 1)
	state = 2;
      else if(state == 3)
	state = 4;
      else if(state == 5) {
	str.push_back(line[i]);
	state = 6;
      }
      else if(state == 6)
	str.push_back(line[i]);
    }
  }
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
  
  bool   EOL     = false;
  int    buffix  = 0;
  int    myint   = '\0';
  char   buff[MAX_LINE_LENGTH];

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


//--------------------------------------------------------
// Procedure: getNextRawALogLine
//     Notes: 

ALogEntry getNextRawALogEntry(FILE *fileptr)
{
  ALogEntry entry;
  if(!fileptr) {
    cout << "failed getNextRawALogEntry() - null file pointer" << endl;
    entry.setStatus("invalid");
    return(entry);
  }
  
  bool EOLine  = false;
  bool EOFile  = false;
  int  buffix  = 0;
  int  lineix  = 0;
  int  myint   = '\0';
  char buff[MAX_LINE_LENGTH];

  string time, var, src, val;

  // Simple state machine: 
  //   0: time
  //   1: between time and variable
  //   2: variable
  //   3: between variable and source
  //   4: source
  //   5: between source and value
  //   6: value
  int  state = 0;

  while((!EOLine) && (!EOFile) && (lineix < MAX_LINE_LENGTH)) {
    myint = fgetc(fileptr);
    unsigned char mychar = myint;
    switch(myint) {
    case EOF:
      EOFile = true;
      break;
    case ' ':
    case '\t':
      if(state==0) {
	buff[buffix] = '\0';
	time = buff;
	buffix = 0;
	state=1;
      }
      else if(state==2) {
	buff[buffix] = '\0';
	var = buff;
	buffix = 0;
	state=3;
      }
      else if(state==4) {
	buff[buffix] = '\0';
	src = buff;
	buffix = 0;
	state=5;
      }
      break;	  
    case '\n':
      buff[buffix] = '\0';  // attach terminating NULL
      val = buff;
      EOLine = true;
      break;
    default:
      if(state==1) 
	state=2;
      else if(state==3)
	state=4;
      else if(state==5)
	state=6;
      buff[buffix] = mychar;
      buffix++;
    }
    lineix++;
  }
  
  //cout << "t:" << time << " v:" << var << " s:" << src << " v:" << val << endl;

  if((time!="")&&(var!="")&&(src!="")&&(val!="") && isNumber(time)) {
    if(isNumber(val))
      entry.set(atof(time.c_str()), var, src, atof(val.c_str()));
    else
      entry.set(atof(time.c_str()), var, src, val);
  }
  else {
    if(EOFile)
      entry.setStatus("eof");
    else
      entry.setStatus("invalid");
  }
  
  return(entry);
}


//--------------------------------------------------------
// Procedure: getSecsfromTimeOfDay
//     Notes: Date String of form "11:50:04 AM"

double getEpochSecsFromTimeOfDay(string date_str)
{
  string time  = stripBlankEnds(biteString(date_str, ' '));
  string am_pm = tolower(stripBlankEnds(date_str));

  string hstr = biteString(time, ':');
  string mstr = biteString(time, ':');
  string sstr = time;

  double hours = atof(hstr.c_str());
  double minutes = atof(mstr.c_str());
  double seconds = atof(sstr.c_str());

  if(am_pm == "pm")
    hours += 12;

  double total_seconds = seconds + (60*minutes) + (3600*hours);

  return(total_seconds);
}


//--------------------------------------------------------
// Procedure: getSecsfromTimeOfDay

double getEpochSecsFromTimeOfDay(double hrs, double mins, double secs)
{
  double total_seconds = secs + (60*mins) + (3600*hrs);
  return(total_seconds);
}


//--------------------------------------------------------
// Procedure: getEpochSecsfromDayOfYear
//     Notes: Date String of form "7/15/2009"
//            Returns the number of seconds before the start of the
//              given date - since January 01 1972.

double getEpochSecsFromDayOfYear(string date_str, int format)
{
  date_str = stripBlankEnds(date_str);
  
  double d_month, d_day, d_year;
  if(format == 0) {
    string s_month = biteString(date_str, '/');
    string s_day   = biteString(date_str, '/');
    string s_year  = date_str;

    d_month = atof(s_month.c_str());
    d_day   = atof(s_day.c_str());
    d_year  = atof(s_year.c_str());
  }

  double total_seconds = getEpochSecsFromDayOfYear(d_day, d_month, d_year);
  return(total_seconds);
}

//--------------------------------------------------------
// Procedure: getEpochSecsfromDayOfYear
//            Returns the number of seconds before the start of the
//              given date - since January 01 1970.

double getEpochSecsFromDayOfYear(double d_day, double d_month, 
				 double d_year)
{
  double years_elapsed = d_year - 1970;
  if(years_elapsed < 0)
    return(0);

  double total_seconds = years_elapsed * 365 * (3600 * 24);

  double days_elapsed = d_day-1;
  if(d_month >  1) days_elapsed += 31;
  if(d_month >  2) days_elapsed += 28;
  if(d_month >  3) days_elapsed += 31;
  if(d_month >  4) days_elapsed += 30;
  if(d_month >  5) days_elapsed += 31;
  if(d_month >  6) days_elapsed += 30;
  if(d_month >  7) days_elapsed += 31;
  if(d_month >  8) days_elapsed += 31;
  if(d_month >  9) days_elapsed += 30;
  if(d_month > 10) days_elapsed += 31;
  if(d_month > 11) days_elapsed += 30;

  total_seconds += 86400 * days_elapsed;

  // handle leap days
  if((d_year >= 1972) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 1976) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 1980) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 1984) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 1988) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 1992) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 1996) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2000) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2004) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2008) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2012) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2016) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2020) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2024) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2028) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2032) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2036) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2040) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2044) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2048) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2052) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2056) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2060) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2064) && (d_month >= 3)) total_seconds += 86400;
  if((d_year >= 2068) && (d_month >= 3)) total_seconds += 86400;

#if 0
  // handle leap seconds  NOT COUNTED IN UNIX EPOCH TIME
  if((d_year >= 1972) && (d_month >= 7)) total_seconds++;
  if(d_year >= 1973) total_seconds++;
  if(d_year >= 1974) total_seconds++;
  if(d_year >= 1975) total_seconds++;
  if(d_year >= 1976) total_seconds++;
  if(d_year >= 1977) total_seconds++;
  if(d_year >= 1978) total_seconds++;
  if(d_year >= 1979) total_seconds++;
  if(d_year >= 1980) total_seconds++;
  if((d_year >= 1981) && (d_month >= 7)) total_seconds++;
  if((d_year >= 1982) && (d_month >= 7)) total_seconds++;
  if((d_year >= 1983) && (d_month >= 7)) total_seconds++;
  if((d_year >= 1985) && (d_month >= 7)) total_seconds++;
  if(d_year >= 1988) total_seconds++;
  if(d_year >= 1990) total_seconds++;
  if(d_year >= 1991) total_seconds++;
  if((d_year >= 1992) && (d_month >= 7)) total_seconds++;
  if((d_year >= 1993) && (d_month >= 7)) total_seconds++;
  if((d_year >= 1994) && (d_month >= 7)) total_seconds++;
  if(d_year >= 1996) total_seconds++;
  if((d_year >= 1997) && (d_month >= 7)) total_seconds++;
  if(d_year >= 1999) total_seconds++;
  if(d_year >= 2006) total_seconds++;
  if(d_year >= 2009) total_seconds++;
#endif

  return(total_seconds);
}
