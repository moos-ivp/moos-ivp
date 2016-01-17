/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: EPlotEngine.cpp                                      */
/*    DATE: Jnauary 16th, 2016                                   */
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
#include "MBUtils.h"
#include "EPlotEngine.h"
#include "LogUtils.h"

using namespace std;

//--------------------------------------------------------
// Constructor

EPlotEngine::EPlotEngine()
{
  m_verbose = false;
}

//--------------------------------------------------------
// Procedure: setALogFile()

bool EPlotEngine::addALogFile(string alog_file)
{
  if(!okFileToRead(alog_file)) {
    cout << "Unable to read from: " << alog_file << endl;
    return(false);
  }
  
  m_alog_files.push_back(alog_file);
  return(true);
}

//--------------------------------------------------------
// Procedure: generate()

void EPlotEngine::generate()
{
  m_community_name = "";
  m_cpa_events.clear();
  m_collision_range = 0;
  m_near_miss_range = 0;
  m_encounter_range = 0;

  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    bool ok = handleALogFile(m_alog_files[i]);
    if(!ok) {
      cout << "Could not process file: " << m_alog_files[i] << endl;
      return;
    }
  }

  cout << "Total encounters: " << m_cpa_events.size() << endl;
  cout << "Collision Range:  " << m_collision_range << endl;
  cout << "Near Miss Range:  " << m_near_miss_range << endl;
  cout << "Encounter Range:  " << m_encounter_range << endl;
  cout << "Community Name:   " << m_community_name  << endl;
}

//--------------------------------------------------------
// Procedure: handleALogFile()

bool EPlotEngine::handleALogFile(string alog_file)
{
  FILE *file_ptr = fopen(alog_file.c_str(), "r");
  if(!file_ptr)
    return(false);
  
  if(m_verbose)
    cout << "Processing: " << alog_file << endl;

  unsigned int line_count  = 0;
  while(1) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr);

    if(m_verbose) {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 100000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }
    
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    if(line_raw == "eof") 
      break;
    
    string var = getVarName(line_raw);
    if((m_community_name == "") && (var == "DB_TIME")) {
      string src = getSourceName(line_raw);
      m_community_name = rbiteString(src, '_');
    }
    else if(var == "ENCOUNTER_SUMMARY") {
      string spec = getDataEntry(line_raw);
      CPAEvent event(spec);
      m_cpa_events.push_back(event);
    }
    else if(var == "COLLISION_DETECT_PARAMS") {
      string param_list = getDataEntry(line_raw);
      vector<string> svector = parseString(param_list, ',');
      for(unsigned int i=0; i<svector.size(); i++) {
	string param = biteStringX(svector[i], '=');
	string value = svector[i];
	if(param == "m_collision_range")
	  m_collision_range = atof(value.c_str());
	else if(param == "m_near_missrange")
	  m_near_miss_range = atof(value.c_str());
	else if(param == "m_encounter_range")
	  m_encounter_range = atof(value.c_str());
      }
    }
  }

  if(m_verbose) 
    cout << endl << uintToCommaString(line_count) << " lines total." << endl;
  
  if(file_ptr)
    fclose(file_ptr);

  return(true);
}


//--------------------------------------------------------
// Procedure: writeLine

void EPlotEngine::writeLine(FILE* f, const string& line) const
{
  if(f)
    fprintf(f, "%s\n", line.c_str());
  else
    cout << line << endl;
}
  
