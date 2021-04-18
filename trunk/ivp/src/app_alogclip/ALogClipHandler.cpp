/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogClipHandler.cpp                                  */
/*    DATE: June 11th, 2015                                      */
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
#include "TermUtils.h"
#include "ALogClipHandler.h"
#include "ALogClipper.h"


using namespace std;

//--------------------------------------------------------
// Procedure: Constructor

ALogClipHandler::ALogClipHandler()
{
  m_min_time = 0;
  m_max_time = 0;
  m_min_time_set = false;
  m_max_time_set = false;

  m_force_overwrite = false;
  m_verbose = false;
  m_batch   = false;
  
  m_suffix  = "_clipped";
}

//--------------------------------------------------------
// Procedure: setTimeStamp()

bool ALogClipHandler::setTimeStamp(double time_stamp)
{
  if(!m_min_time_set) {
    m_min_time_set = true;
    m_min_time = time_stamp;
    return(true);
  }

  if(!m_max_time_set) {
    if(time_stamp < m_min_time) {
      cout << "2nd timestamp, max time, must be >= min_time" << endl;
      return(false);
    }
    m_max_time_set = true;
    m_max_time = time_stamp;
    return(true);
  }
  
  cout << "Only two timestamps may be provided." << endl;
  return(false);
}

//--------------------------------------------------------
// Procedure: setSuffix()

bool ALogClipHandler::setSuffix(string str)
{
  if(strContainsWhite(str)) {
    cout << "Suffix cannot contain white space. Just bad manners." << endl;
    return(false);
  }
  
  if(!strBegins(str, "_")) 
    str = "_" + str;
  m_suffix = str;

  return(true);
}


//--------------------------------------------------------
// Procedure: addALogFile()

bool ALogClipHandler::addALogFile(string alog_file)
{
  if(!strEnds(alog_file, ".alog"))
    return(false);
  
  if(strContains(alog_file, "m_suffix"))  // then just ignore
    return(true);

  if(m_batch) {
    m_batch_files.push_back(alog_file);
    return(true);
  }
  
  if(m_infile == "") {
    m_infile = alog_file;
    return(true);
  }

  if(m_outfile == "") {
    m_outfile = alog_file;
    return(true);
  }
      
  cout << "In non-batch mode, only two alog files may be specified." << endl;  
  cout << "The first is the input file, the 2nd is the output file." << endl;
  cout << "An additional alog file, " << alog_file << ", was given." << endl;
  return(false);
}

//-------------------------------------------------------------------------
// Procedure: process()

bool ALogClipHandler::process()
{
  if(m_verbose) {
    cout << "Batch file mode: " << boolToString(m_batch) << endl;
    cout << "Total batch files: " << m_batch_files.size() << endl;
  }

  bool pre_check_ok = preCheck();
  if(m_verbose) 
    cout << "Pre-Check stage passed: " << boolToString(pre_check_ok) << endl;

  if(!pre_check_ok)
    return(false);

  if(!m_batch)
    return(processFile(m_infile, m_outfile));
  
  for(unsigned int i=0; i<m_batch_files.size(); i++) {
    string outfile = addSuffixToALogFile(m_batch_files[i]);
    bool ok = processFile(m_batch_files[i], outfile);
    if(!ok)
      return(false);
  }

  return(true);
}

//------------------------------------------------------------------------
// Procedure: processBatch()

bool ALogClipHandler::processBatch()
{
  return(true);
}

//------------------------------------------------------------------------
// Procedure: processFile()

bool ALogClipHandler::processFile(string infile, string outfile)
{
  if(m_verbose) {
    cout << "Preparing " << infile << endl;
    cout << "   ---> " << endl;
    cout << "   " << outfile << endl;
  }

  ALogClipper clipper;

  bool ok = clipper.openALogFileRead(outfile);
  if(ok && !m_force_overwrite) {
    bool done = false;
    while(!done) {
      cout << "File " << outfile << " exists. Replace?(y/n)" << endl;
      char answer = getCharNoWait();
      if(answer == 'n') {
        cout << "Aborted: The file " << outfile;
        cout << " will not be overwritten" << endl;
        exit(0);
      }
      if(answer == 'y')
        done = true;
    }
  }

  clipper.openALogFileRead(infile);
  clipper.openALogFileWrite(outfile);

  if(m_verbose) {
    cout << endl << "Processing input file " << infile << " ..." << endl;
    cout << "min_time: " << m_min_time << endl;
    cout << "max_time: " << m_max_time << endl;
  }

  clipper.clip(m_min_time, m_max_time);

  return(true);
}


//------------------------------------------------------------------------
// Procedure: preCheck()

bool ALogClipHandler::preCheck()
{
  // Part 1: Check that min/max time have been set
  if(!m_min_time_set || !m_max_time_set) {
    cout << "A min time AND a max time must be specified." << endl;
    return(false);
  }

  // Part 2A: Check the validity of batched alog files
  if(m_batch) {
    unsigned int total_batch_files = m_batch_files.size();
    if(total_batch_files == 0) {
      cout << "In batch mode, but no batch files." << endl;
      return(false);
    }

    for(unsigned int i=0; i<total_batch_files; i++) {
      if(!okFileToRead(m_batch_files[i])) {
	cout << "Cannot read " << m_batch_files[i] << endl;
	return(false);
      }
      string output_file = addSuffixToALogFile(m_batch_files[i]);
      if(!okFileToWrite(output_file)) {
	cout << "Will not be able to write to: " << output_file << endl;
	return(false);
      }
    }
    return(true);
  }

  // Part 2B: Check the validity of singularly specified alog files
  if(m_infile == "") {
    cout << "No alog input file provided. " << endl;
    return(false);
  }

  if(!okFileToRead(m_infile)) {
    cout << "Input alog file " << m_infile << " not found/readable." << endl;
    return(false);
  }
	 
  if(m_outfile != "") {
    if(!okFileToWrite(m_infile)) {
      cout << "Target new alog file " << m_outfile << " not writeable." << endl;
      return(false);
    }
  }

  return(true);
}


//------------------------------------------------------------------------
// Procedure: addSuffixToALogFile

string ALogClipHandler::addSuffixToALogFile(string filename)
{
  if(!strEnds(filename, ".alog"))
    return(filename);

  string outfile = filename;
  
  rbiteString(outfile, '.');

  outfile += m_suffix + ".alog";

  return(outfile);
}




