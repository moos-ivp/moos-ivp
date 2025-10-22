/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT Dept of Mechanical Eng                           */
/*    FILE: Plucker.cpp                                          */
/*    DATE: Mach 8th, 2024                                       */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <sys/types.h>
#include <unistd.h>

#include "Plucker.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

Plucker::Plucker()
{
  m_lnum = 0;
  m_comment = "#";
  m_index = 0;
  m_fld = "";
  m_gsep = ',';
  m_lsep = '=';
}


//---------------------------------------------------------
// Procedure: setFile()

bool Plucker::setFile(string filestr)
{
  if(!okFileToRead(filestr))
    return(false);

  m_file = filestr;
  return(true);
}

//---------------------------------------------------------
// Procedure: setLNum()

bool Plucker::setLNum(string str)
{
  return(setUIntOnString(m_lnum, str));
}

//---------------------------------------------------------
// Procedure: setIndex()

bool Plucker::setIndex(string str)
{
  return(setUIntOnString(m_index, str));
}

//---------------------------------------------------------
// Procedure: setComment()

bool Plucker::setComment(string str)
{
  m_comment = str;
  return(true);
}

//---------------------------------------------------------
// Procedure: setFld()

bool Plucker::setFld(string str)
{
  m_fld = str;
  return(true);
}

//---------------------------------------------------------
// Procedure: setGrep()

bool Plucker::setGrep(string str)
{
  m_grep = str;
  return(true);
}

//---------------------------------------------------------
// Procedure: setGSep()

bool Plucker::setGSep(string str)
{
  if(str.length() != 1)
    return(false);

  m_gsep = str.at(0);
  return(true);
}

//---------------------------------------------------------
// Procedure: setLSep()

bool Plucker::setLSep(string str)
{
  if(str.length() != 1)
    return(false);

  m_gsep = str.at(0);
  return(true);
}



//---------------------------------------------------------
// Procedure: pluck()

string Plucker::pluck()
{
  vector<string> lines = fileBuffer(m_file);
  if(lines.size() == 0)
    return("empty file");

  unsigned int line_num = 0;
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);
    // Skip over lines that do not "count"
    if(line == "")
      continue;
    if((m_grep != "") && !strContains(line, m_grep))
      continue;
    if((m_fld != "") && !strContains(line, m_fld))
      continue;
    if(strBegins(line, m_comment))
      continue;

    // Only increment if this is a line that counts
    line_num++;
    
    if(m_lnum != line_num)
      continue;

    // If plucking based on field name
    if(m_fld != "") {
      string result = tokStringParse(line, m_fld, m_gsep, m_lsep);
      return(result);
    }

    // If plucking based on position
    string result;
    for(unsigned int i=0; i<m_index; i++)
      result = biteStringX(line, ' ');
    return(result);
          
    line_num++;
  }

  return("");
}

