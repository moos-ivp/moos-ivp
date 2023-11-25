/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ALogClipper.cpp                                      */
/*    DATE: June 5th, 2008                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cmath>
#include "MBUtils.h"
#include "ALogClipper.h"
#include <cstdlib>
#include <cstdio>

using namespace std;

//--------------------------------------------------------
// Procedure: Constructor

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

  m_preserve_vars.push_back("IVPHELM_DOMAIN");
}

//--------------------------------------------------------
// Procedure: clip
//     Notes: 

unsigned int ALogClipper::clip(double min_time, double max_time)
{
  while(m_infile) {
    string line = getNextLine();

    string linecopy  = line;    
    string timestr   = biteStringX(linecopy, ' ');
    string moosvar   = biteStringX(linecopy, ' ');
    double timestamp = atof(timestr.c_str());
    
    if(timestr[0] == '%')
      writeNextLine(line);
    else if(vectorContains(m_preserve_vars, moosvar)) {
      m_kept_chars += line.length();
      m_kept_lines += 1;
      writeNextLine(line);
    }
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

  if(m_outfile)
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
      buff[buffix] = '\0';  // attach terminating NULL
      EOL = true;
      break;
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
    printf("%s\n", line.c_str());
  else
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

  










