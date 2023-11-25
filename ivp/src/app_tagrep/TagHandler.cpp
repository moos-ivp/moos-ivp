/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TagHandler.cpp                                       */
/*    DATE: September 28th, 2020                                 */
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

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "MBUtils.h"
#include "TagHandler.h"
#include "LogUtils.h"
#include "FileBuffer.h"

using namespace std;

//--------------------------------------------------------
// Constructor

TagHandler::TagHandler()
{
  m_verbose       = false;
  m_keep_blanks   = true;
  m_keep_tag_line = false;
  m_first_line_only = false;
}

//--------------------------------------------------------
// Procedure: setInputFile()

bool TagHandler::setInputFile(string filename)
{
  if(m_input_file != "") {
    if(m_verbose)
      cout << "Input file previously set, cannot set to: " << filename << endl;
    return(false);
  }

  if(!okFileToRead(filename)) {
    if(m_verbose)
      cout << "Input file [" << filename << "] is not readable" << endl;
    return(false);
  }
  
  m_input_file = filename;
  return(true);
}

//--------------------------------------------------------
// Procedure: setTag()

bool TagHandler::setTag(string tag_str)
{
  if(m_tag != "") {
    if(m_verbose)
      cout << "Tag was previously set, cannot set to: " << tag_str << endl;
    return(false);
  }

  tag_str = stripBlankEnds(tag_str);
  tag_str = stripChevrons(tag_str);
  
  m_tag = tag_str;
  return(true);
}


//--------------------------------------------------------
// Procedure: setHeader()

bool TagHandler::setHeader(string hdr_str)
{
  if(m_header != "") {
    if(m_verbose)
      cout << "Header was previously set, cannot set to: " << hdr_str << endl;
    return(false);
  }

  m_header = hdr_str;
  return(true);
}


//--------------------------------------------------------
// Procedure: handle

bool TagHandler::handle()
{
  // Sanity check on the tag setting
  if(m_tag == "") {
    if(m_verbose)
      cout << "Error: No tag provided" << endl;
    return(false);
  }
  
  // Sanity check on the input file
  vector<string> slines = fileBuffer(m_input_file);
  if(slines.size() == 0) {
    if(m_verbose)
      cout << "Error: Empty or unreadble file: " << m_input_file << endl;
    return(false);
  }

  // Before processing, check if the header was meant to be the same
  // as the tag.
  if(m_header == "same_as_tag")
    m_header = m_tag;

  // Process the file
  bool ever_active = false;
  bool active = false;
  for(unsigned int i=0; i<slines.size(); i++) {
    string orig = slines[i];
    string line = stripBlankEnds(slines[i]);
    if(strBegins(line, "<tag>")) {
      biteStringX(line, '>');
      string tagname = stripChevrons(line);
      if(tagname == m_tag) {
	if(m_keep_tag_line)
	  cout << orig << endl;
	active = true;
	ever_active = true;
	if(m_header != "")
	  cout << m_header << "=";
      }
      else
	active = false;
      continue;
    }

    if(!active)
      continue;
    if((line.size() == 0) && !m_keep_blanks)
      continue;
    if(strBegins(line, "#"))
      continue;

    cout << orig << endl;
    if(m_first_line_only)
      return(true);
  }

  if(!ever_active)
    return(false);
  
  return(true);
}

