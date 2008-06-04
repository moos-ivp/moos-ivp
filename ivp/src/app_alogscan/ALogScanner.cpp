/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ALogScanner.cpp                                      */
/*    DATE: June 3rd, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include "MBUtils.h"
#include "ALogScanner.h"

using namespace std;

//--------------------------------------------------------
// Constructor

ALogScanner::ALogScanner()
{
  m_file = 0;
}

//--------------------------------------------------------
// Procedure: scan
//     Notes: 

bool ALogScanner::scan()
{
  while(m_file) {
    string str = getNextLine();

    string timestr  = biteString(str, ' ');
    string variable = biteString(str, ' ');
    string wsource  = biteString(str, ' ');
    string varvalue = biteString(str, ' ');
    
    double timestamp = atof(timestr.c_str());
    
    if((timestr != "") && (timestr[0] != '%')) 
      m_report.addLine(timestamp, variable, wsource, varvalue);
  }

  int line_digits  = (int)(log10(m_report.getMaxLines())) + 2;
  int char_digits  = (int)(log10(m_report.getMaxChars())) + 2;
  int start_digits = (int)(log10(m_report.getMaxStartTime())) + 4;
  int stop_digits  = (int)(log10(m_report.getMaxStopTime())) + 4;

  cout << "line_digits:  " << line_digits  << endl;
  cout << "char_digits:  " << char_digits  << endl;
  cout << "start_digits: " << start_digits << endl;
  cout << "stop_digits:  " << stop_digits  << endl;


  if(line_digits  < 5)  line_digits  = 5;
  if(char_digits  < 5)  char_digits  = 5;
  if(start_digits < 6)  start_digits = 6;
  if(stop_digits  < 6)  stop_digits  = 6;

  string sline_digits  = intToString(line_digits);
  string schar_digits  = intToString(char_digits);
  string svname_digits = intToString(m_report.getVarNameMaxLength());
  string sstart_digits = intToString(start_digits);
  string sstop_digits  = intToString(stop_digits);

  string hformat_string = ("%-" + svname_digits + "s ");
  hformat_string += ("%" + sline_digits + "s ");
  hformat_string += ("%" + schar_digits + "s  ");
  hformat_string += ("%" + sstart_digits + "s  ");
  hformat_string += ("%" + sstop_digits  + "s\n");

  string bformat_string = ("%-" + svname_digits + "s ");
  bformat_string += ("%" + sline_digits + "d ");
  bformat_string += ("%" + schar_digits + "d  ");
  bformat_string += ("%" + sstart_digits + "s  ");
  bformat_string += ("%" + sstop_digits  + "s \n");

  printf("\n");
  printf(hformat_string.c_str(),
	 "Variable Name", "Lines", "Chars", " Start", "  Stop");
  printf(hformat_string.c_str(),
	 "-------------", "-----", "-----", "------", "------");

  unsigned int vsize = m_report.size();
  for(unsigned int i=0; i<vsize; i++) {
    string varname     = m_report.getVarName(i);
    string varsources  = m_report.getVarSources(i);
    double first       = m_report.getVarFirstTime(i);
    double last        = m_report.getVarLastTime(i);
    unsigned int lcnt  = m_report.getVarCount(i);
    unsigned int chars = m_report.getVarChars(i);

    string sfirst = doubleToString(first, 2);
    string slast  = doubleToString(last,  2);
    
    printf(bformat_string.c_str(),  varname.c_str(), lcnt, 
	   chars, sfirst.c_str(), slast.c_str());
  }


  return(true);
}

//--------------------------------------------------------
// Procedure: getNextLine
//     Notes: 

string ALogScanner::getNextLine()
{
  if(!m_file)
    return("");
  
  const int MAX_LINE_LENGTH = 50000;

  bool last_char_was_white = false;
  bool nonwhites_read = false;
  int  myint   = '\0';
  int  buffix  = 0;
  int  buffwix = 0;
  bool EOL     = false;
  char buff[MAX_LINE_LENGTH];

  // The buffix and buffwix indices are kept separate so that
  // when an EOL/EOF is reached the terminating NULL character
  // can be placed directly after the last non-whitespace char.

  // The first_nonwhite_read bool is kept so that leading white
  // space is not included in the returned line.

  // The buffwix, first_nonwhite_read, and last_char_was_white
  // tricks are done to ensure there are no whitespace chars
  // on either end of the line, and there are not back-to-back
  // white-spaces in the returned line. Thus it is ready for 
  // parsing on white space by the user.

  while((!EOL) && (buffix < MAX_LINE_LENGTH)) {
    myint = fgetc(m_file);
    unsigned char mychar = myint;
    switch(myint) {
    case EOF:
      fclose(m_file);
      m_file = 0;
    case '\n':
      buff[buffwix] = '\0';  // attach terminating NULL
      EOL = true;
      break;
    case ' ':               // blank char
      if((!last_char_was_white) && (nonwhites_read)) {
	buff[buffix] = ' ';  
	buffix++;
      }
      last_char_was_white = true;
      break;
    case '\t':
      if((!last_char_was_white) && (nonwhites_read)) {
	buff[buffix] = ' ';  // Convert tabs to blank chars
	buffix++;
      }
      last_char_was_white = true;
      break;
    default:
      last_char_was_white = false;
      nonwhites_read = true;
      buff[buffix] = mychar;
      buffix++;
      buffwix = buffix;;
    }
  }
  string str = buff;
  return(str);
}

//--------------------------------------------------------
// Procedure: openALogFile

bool ALogScanner::openALogFile(string alogfile)
{
  m_file = fopen(alogfile.c_str(), "r");
  if(!m_file)
    return(false);
  else
    return(true);
}

