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
// Procedure: setALogFileIn()

bool EPlotEngine::setALogFileIn(string alog_file)
{
  if(!okFileToRead(alog_file)) {
    cout << "Unable to read from: " << alog_file << endl;
    return(false);
  }
  
  m_alog_file_in = alog_file;
  return(true);
}

//--------------------------------------------------------
// Procedure: generate()

void EPlotEngine::generate()
{
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
  
