/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PickHandler.cpp                                      */
/*    DATE: August 13th, 2022                                    */
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
#include "PickHandler.h"
#include "LogUtils.h"
#include "ACTable.h"

using namespace std;

//--------------------------------------------------------
// Constructor()

PickHandler::PickHandler()
{
  m_file_in  = 0;
  m_verbose  = false;

  m_format_aligned = true;
}

//--------------------------------------------------------
// Procedure: setLogFile()

bool PickHandler::setLogFile(string logfile)
{
  if(m_file_in !=0) {
    if(m_verbose)
      cout << "Only one log file may be provided." << endl;
    return(false);
  }
  
  m_file_in = fopen(logfile.c_str(), "r");
  if(!m_file_in) {
    if(m_verbose)
      cout << "Input file not found or unable to open: " + logfile << endl;
    return(false);
  }
  
  return(true);
}

//--------------------------------------------------------
// Procedure: addField()

bool PickHandler::addField(string field)
{
  if(strContainsWhite(field))
    return(false);
  m_fields.push_back(field);
  return(true);
}

//--------------------------------------------------------
// Procedure: handle()

bool PickHandler::handle()
{
  if(!m_file_in)
    return(false);
  if(m_fields.size() == 0) {
    cout << "No fields identified for selection.";
    return(false);
  }

  // ==============================================
  // Part 1: Handle each line
  // ==============================================
  bool done = false;
  while(!done) {
    string line_raw = getNextRawLine(m_file_in);

    // Check for end of file
    if(line_raw == "eof") 
      break;
    if(line_raw.at(0) == '%')
      continue;


    vector<string> line_vals;
    for(unsigned int i=0; i<m_fields.size(); i++) {
      string fld = m_fields[i];
      string val = tokStringParse(line_raw, fld);
      if(val == "")
	val = "-";
      line_vals.push_back(val);
    }
    m_values.push_back(line_vals);
  }

  if(m_file_in)
    fclose(m_file_in);

  // ==============================================
  // Part 2: Make the report
  // ==============================================
  if(m_format_aligned) {
    unsigned int cols = m_fields.size();
    ACTable actab(cols,2);
    
    for(unsigned int i=0; i<m_values.size(); i++) {
      for(unsigned int j=0; j<m_values[i].size(); j++) {
	actab << m_values[i][j];
      }
    }
    vector<string> lines = actab.getTableOutput();
    for(unsigned int i=0; i<lines.size(); i++)
      cout << lines[i] << endl;    
  }
  else {
    for(unsigned int i=0; i<m_values.size(); i++) {
      for(unsigned int j=0; j<m_values[i].size(); j++) {
	if(j != 0)
	  cout << " ";
	cout << m_values[i][j];
      }
      cout << endl;
    }
  }	
  return(true);
}






