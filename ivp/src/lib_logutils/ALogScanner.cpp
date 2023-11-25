/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ALogScanner.cpp                                      */
/*    DATE: June 3rd, 2008                                       */
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

#include <cstdio>
#include <iostream>
#include "MBUtils.h"
#include "ALogScanner.h"
#include "LogUtils.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------
// Procedure: scan

ScanReport ALogScanner::scan()
{
  ScanReport report;

  char carriage_return = 13;
  unsigned int lines_read = 0;

  bool done = false;

  if(m_verbose)
    cout << endl;
  
  while(!done) {

    if(m_verbose) {
      lines_read++;
      if((lines_read % 5000) == 0) {
	cout << "  Lines Read: " << uintToCommaString(lines_read);
	cout << carriage_return << flush;
      }
    }
    
    ALogEntry entry = getNextRawALogEntry(m_file, true);
    string status = entry.getStatus();
    // Check for the end of the file
    if(status == "eof")
      done = true;
    // If otherwise a "normal" line, process
    else if(status != "invalid") {
      string src = entry.getSource();
      // If the source is the IvP Helm, then see if the behavior
      // information is present and append to the source
      if((src == "pHelmIvP") && m_use_full_source) {
	string src_aux = entry.getSrcAux();
	if(src_aux != "") {
	  if(strContains(src_aux, ':')) {
	    string iter = biteString(src_aux, ':');
	    string bhv  = src_aux;
	    if(bhv != "")
	      src = src + ":" + bhv;
	  }
	  else
	    src += ":" + src_aux;
	}	      
      }
      
      report.addLine(entry.getTimeStamp(),
		     entry.getVarName(),
		     src,
		     entry.getStringVal());
    }
  }

  if(m_verbose) {
    cout << termColor("blue");
    cout << "  Lines Read: " << uintToCommaString(lines_read) << endl;
    cout << termColor();
  }
  
  return(report);
}

//--------------------------------------------------------
// Procedure: scanRateOnly

ScanReport ALogScanner::scanRateOnly()
{
  ScanReport report;
  bool done = false;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(m_file, true);
    string status = entry.getStatus();
    if(status == "eof")
      done = true;
    else if(status != "invalid")
      report.addLineRateOnly(entry);
  }
  return(report);
}

//--------------------------------------------------------
// Procedure: openALogFile

bool ALogScanner::openALogFile(string alogfile)
{
  m_file = fopen(alogfile.c_str(), "r");
  if(!m_file)
    return(false);
  else
    return(true);
}









