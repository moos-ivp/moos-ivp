/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_BHVFile.cpp                                */
/*    DATE: July 4th, 2025                                       */
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
#include <iostream>
#include "FileBuffer.h" 
#include "Populator_BHVFile.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor()

Populator_BHVFile::Populator_BHVFile(bool verbose)
{
  m_verbose = verbose;
}

//----------------------------------------------------------
// Procedure: setFileBHV()

bool Populator_BHVFile::setFileBHV(string filename) 
{
  m_files_bhv.clear();
  return(addFileBHV(filename));
}

//----------------------------------------------------------
// Procedure: addFileBHV()

bool Populator_BHVFile::addFileBHV(string filename) 
{
  if(!okFileToRead(filename))
    return(false);
  
  m_files_bhv.push_back(filename);
  return(true);
}

//----------------------------------------------------------
// Procedure: populate()

bool Populator_BHVFile::populate() 
{
  bool all_ok = true;
  for(unsigned int i=0; i<m_files_bhv.size(); i++)
    all_ok = all_ok && populate(m_files_bhv[i]);

  return(all_ok);
}

//----------------------------------------------------------
// Procedure: populate(filename)

bool Populator_BHVFile::populate(string file_bhv)
{
  if(m_verbose)
    cout << "Looking for: " << file_bhv << endl;

  // =======================================================
  // Part 1A: Find and Read the .bhv file
  vector<string> lines = fileBuffer(file_bhv);
  if(lines.size() == 0) {
    cout << "Not found or empty file: " << file_bhv << endl;
    return(false);
  }

  string curr_block = "global";
  
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = lines[i];
    string lcopy = stripBlankEnds(line);

    if(curr_block == "global") {

      if(strBegins(tolower(lcopy), "behavior")) {
	curr_block = rbiteString(lcopy, '=');
	continue;
      }
      if(strContains(lcopy, "::")) {
	string bhv = nibbleString(lcopy, "::");
	string pair = lcopy;
	m_bhv_file.addPatchLine(bhv, pair);
	continue;
      }
      
      if(strBegins(lcopy, "initialize")) {
	m_bhv_file.addInitLine(lcopy);
	continue;
      }      

      if(strBegins(lcopy, "set") && strContains(lcopy, "MODE")) {
	m_bhv_file.addModeLine(line);
	curr_block = "modes";
	continue;
      }

      m_bhv_file.addLine("global", line);
      continue;
    }

    if((strBegins(lcopy, "}")) && (curr_block == "modes")) {
      m_bhv_file.addModeLine(line);
      curr_block = "global";
      continue;
    }

    if(lcopy == "}") {
      //m_bhv_file.addLine(curr_block, line);
      curr_block = "global";
      continue;
    }

    if(curr_block == "modes") {
      m_bhv_file.addModeLine(line);
      continue;
    }
    
    curr_block = m_bhv_file.addLine(curr_block, line);
  }

  return(true);
}

