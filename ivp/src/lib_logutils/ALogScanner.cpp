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

#include <math.h>
#include "MBUtils.h"
#include "ALogScanner.h"

using namespace std;

//--------------------------------------------------------
// Procedure: scan
//     Notes: 

ScanReport ALogScanner::scan()
{
  ScanReport report;
  while(m_file) {
    string str = getNextLine();

    string timestr  = biteString(str, ' ');
    string variable = biteString(str, ' ');
    string wsource  = biteString(str, ' ');
    string varvalue = biteString(str, ' ');
    
    double timestamp = atof(timestr.c_str());
    
    if((timestr != "") && (timestr[0] != '%')) 
      report.addLine(timestamp, variable, wsource, varvalue);
  }

  return(report);
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

