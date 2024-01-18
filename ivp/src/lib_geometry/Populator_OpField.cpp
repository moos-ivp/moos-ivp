/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / MIT Cambridge MA            */
/*    FILE: Populator_OpField.cpp                                */
/*    DATE: Nov 23rd, 2023                                       */
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

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "Populator_OpField.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: addALogFile()

bool Populator_OpField::addFileOPF(string filename)
{
  if(!okFileToRead(filename))
    return(false);
  if(vectorContains(m_opf_files, filename))
    return(false);

  m_opf_files.push_back(filename);

  return(true);
}

//---------------------------------------------------------------
// Procedure: populate()

bool Populator_OpField::populate()
{
  for(unsigned int i=0; i<m_opf_files.size(); i++) {
    bool ok = populateFromFile(m_opf_files[i]);
    if(!ok)
      return(false);
  }
  
  if(m_opfield.size() == 0)
    return(false);
  
  return(true);
}


//---------------------------------------------------------------
// Procedure: populateFromFile()

bool Populator_OpField::populateFromFile(string filename)
{
  vector<string> lines = fileBuffer(filename);
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);
    if(line == "")
      continue;
    if(strBegins(line, "#") || strBegins(line, "//"))
      continue;
    
    string alias = biteStringX(line, '=');
    string value = line;

    // Ignore lines of the form AAT=3,4 preferring instead
    // only lines like AA=x=3,y=4
    //if(!strContains(value, "x="))
    //continue;
    
    bool ok = m_opfield.addPoint(alias, value);
    if(!ok) {
      cout << "Populator_OpField Bad Line: " << alias << endl;
      return(false);
    }
  }
  
  return(true);
}

