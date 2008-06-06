/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ALogClipper.cpp                                      */
/*    DATE: June 5th, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <math.h>
#include "MBUtils.h"
#include "ALogClipper.h"

using namespace std;

//--------------------------------------------------------
// Procedure: Constructor
//     Notes: 

ALogClipper::ALogClipper()
{
  m_infile  = 0;
  m_outfile = 0;

  m_kept_chars          = 0;
  m_clipped_chars_front = 0;
  m_clipped_chars_back  = 0;
  m_kept_lines          = 0;
  m_clipped_lines_front = 0;
  m_clipped_lines_back  = 0;
}

//--------------------------------------------------------
// Procedure: clip
//     Notes: 

unsigned int ALogClipper::clip(double min_time, double max_time)
{
  while(m_infile) {
    string line = getNextLine();

    string linecopy  = line;    
    string timestr   = biteString(linecopy, ' ');
    double timestamp = atof(timestr.c_str());
    
    if(timestr[0] == '%')
      writeNextLine(line);
    else if(timestamp < min_time) {
      m_clipped_chars_front += line.length();
      m_clipped_lines_front += 1;
    }
    else if(timestamp > max_time) {
      m_clipped_chars_back += line.length();
      m_clipped_lines_back += 1;
    }
    else {
      m_kept_chars += line.length();
      m_kept_lines += 1;
      writeNextLine(line);
    }
  }
  
  fclose(m_outfile);
  return(m_clipped_lines_front + m_clipped_lines_back);
}

//--------------------------------------------------------
// Procedure: getNextLine
//     Notes: 

string ALogClipper::getNextLine()
{
  if(!m_infile)
    return("");
  
  const int MAX_LINE_LENGTH = 50000;

  int  myint   = '\0';
  int  buffix  = 0;
  bool EOL     = false;
  char buff[MAX_LINE_LENGTH];

  while((!EOL) && (buffix < MAX_LINE_LENGTH)) {
    myint = fgetc(m_infile);
    unsigned char mychar = myint;
    switch(myint) {
    case EOF:
      fclose(m_infile);
      m_infile = 0;
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
// Procedure: getNextLine
//     Notes: 

bool ALogClipper::writeNextLine(const string& line)
{
  if(!m_outfile)
    return(false);
  
  fprintf(m_outfile, "%s\n", line.c_str());

  return(true);
}

//--------------------------------------------------------
// Procedure: openALogFileRead

bool ALogClipper::openALogFileRead(string alogfile)
{
  if(m_infile)
    fclose(m_infile);

  m_infile = fopen(alogfile.c_str(), "r");
  if(!m_infile)
    return(false);
  else
    return(true);
}

//--------------------------------------------------------
// Procedure: openALogFileWrite

bool ALogClipper::openALogFileWrite(string alogfile)
{
  if(m_outfile)
    fclose(m_outfile);

  m_outfile = fopen(alogfile.c_str(), "w");
  if(!m_outfile)
    return(false);
  else
    return(true);
}


//--------------------------------------------------------
// Procedure: getDetails

unsigned int ALogClipper::getDetails(const string& statevar)
{
  if(statevar == "kept_chars")
    return(m_kept_chars);
  else if(statevar == "clipped_chars_front")
    return(m_clipped_chars_front);
  else if(statevar == "clipped_chars_back")
    return(m_clipped_chars_back);
  else if(statevar == "kept_lines")
    return(m_kept_lines);
  else if(statevar == "clipped_lines_front")
    return(m_clipped_lines_front);
  else if(statevar == "clipped_lines_back")
    return(m_clipped_lines_back);
  else
    return(0);
}

  
