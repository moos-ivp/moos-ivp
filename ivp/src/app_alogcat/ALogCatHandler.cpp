/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogCatHandler.cpp                                   */
/*    DATE: Aug 13th, 2018                                       */
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
#include "ALogCatHandler.h"

using namespace std;

//--------------------------------------------------------
// Procedure: Constructor

ALogCatHandler::ALogCatHandler()
{
  m_force_overwrite = false;
  m_verbose = false;
}

//--------------------------------------------------------
// Procedure: addALogFile()

bool ALogCatHandler::addALogFile(string alog_file)
{
  if(!strEnds(alog_file, ".alog"))
    return(false);
  
  if(vectorContains(m_alog_files, alog_file))
    return(false);

  if(strContainsWhite(alog_file))
    return(false);
  
  m_alog_files.push_back(alog_file);
  return(true);
}

//-------------------------------------------------------------------------
// Procedure: process()

bool ALogCatHandler::process()
{
  return(true);
}

//------------------------------------------------------------------------
// Procedure: processFile()

bool ALogCatHandler::processFile(string infile)
{
  return(true);
}


//------------------------------------------------------------------------
// Procedure: preCheck()

bool ALogCatHandler::preCheck()
{
  // Part 1: Check the validity of alog files
  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    if(!okFileToRead(m_alog_files[i])) {
      cout << "Cannot read " << m_alog_files[i] << endl;
      return(false);
    }
  }
  
  if(!okFileToWrite(m_outfile)) {
    cout << "Will not be able to write to: " << m_outfile << endl;
    return(false);
  }
  
  return(true);
}


