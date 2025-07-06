/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_MOOSFile.cpp                               */
/*    DATE: May 7th, 2025                                        */
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
#include "Populator_MOOSFile.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor()

Populator_MOOSFile::Populator_MOOSFile(bool verbose)
{
  m_verbose = verbose;
}

//----------------------------------------------------------
// Procedure: setFileMOOS()

bool Populator_MOOSFile::setFileMOOS(string filename) 
{
  m_files_moos.clear();
  return(addFileMOOS(filename));
}

//----------------------------------------------------------
// Procedure: addFileMOOS()

bool Populator_MOOSFile::addFileMOOS(string filename) 
{
  if(!okFileToRead(filename))
    return(false);
  
  m_files_moos.push_back(filename);
  return(true);
}

//----------------------------------------------------------
// Procedure: populate()

bool Populator_MOOSFile::populate() 
{
  bool all_ok = true;
  for(unsigned int i=0; i<m_files_moos.size(); i++)
    all_ok = all_ok && populate(m_files_moos[i]);

  return(all_ok);
}

//----------------------------------------------------------
// Procedure: populate(filename)

bool Populator_MOOSFile::populate(string file_moos)
{
  if(m_verbose)
    cout << "Looking for: " << file_moos << endl;

  // =======================================================
  // Part 1A: Find and Read the .moos file
  vector<string> lines = fileBuffer(file_moos);
  if(lines.size() == 0) {
    cout << "Not found or empty file: " << file_moos << endl;
    return(false);
  }

  string curr_block = "global";
  
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = lines[i];
    string line_copy = stripBlankEnds(line);

    if(curr_block == "global") {

      if(strBegins(tolower(line_copy), "processconfig")) {
	curr_block = rbiteString(line_copy, '=');
	continue;
      }
      if(strContains(line_copy, "::")) {
	string app = nibbleString(line_copy, "::");
	string pair = line_copy;
	m_moos_file.addPatchLine(app, pair);
	continue;
      }
      m_moos_file.addLine("global", line);
      continue;
    }

    if(line_copy == "}") {
      //m_moos_file.addLine(curr_block, line);
      curr_block = "global";
      continue;
    }

    m_moos_file.addLine(curr_block, line);
  
  }

  if(m_verbose) {
    cout << "MOOSFile: " << file_moos << endl;
    cout << "-----------------------" << endl;
    m_moos_file.print();
  }
  
  return(true);
}

