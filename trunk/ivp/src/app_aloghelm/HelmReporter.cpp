/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmReporter.cpp                                     */
/*    DATE: March 10th, 2010                                     */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "MBUtils.h"
#include "HelmReporter.h"
#include "LogUtils.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------
// Procedure: handle
//     Notes: 

bool HelmReporter::handle(const string& alogfile)
{
  if(alogfile == "") {
    cout << termColor("red");
    cout << "Alog file was specified. Exiting now." << endl;
    cout << termColor();
    return(false);
  }

  FILE *file_ptr = fopen(alogfile.c_str(), "r");
  if(!file_ptr) {
    cout << termColor("red");
    cout << "Alog file not found or unable to open - exiting" << endl;
    cout << termColor();
    return(false);
  }
  
  unsigned int line_count  = 0;
  unsigned int life_events = 0;
  bool done = false;
  while(!done) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr);

    if((line_count % 10000) == 0)
      cout << "+" << flush;
    if((line_count % 100000) == 0)
      cout << " (" << uintToCommaString(line_count) << ") lines" << endl;


    bool line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_raw == "eof") 
      done = true;
    
    if(!done && !line_is_comment) {
      string varname = getVarName(line_raw);
      string data = getDataEntry(line_raw);
      if(varname == "IVPHELM_LIFE_EVENT") { 
	m_life_events.addLifeEvent(data);
	life_events++;
      }
    }
  }
  cout << endl << uintToCommaString(line_count) << " lines total." << endl;
  cout << uintToString(life_events) << " life events." << endl;

  if(file_ptr)
    fclose(file_ptr);

  return(true);
}

//--------------------------------------------------------
// Procedure: setReportType
//     Notes: 

bool HelmReporter::setReportType(string rtype)
{
  if(rtype == "life")
    m_report_life = true;
  else
    return(false);

  return(true);
}

//--------------------------------------------------------
// Procedure: printReport
//     Notes: 

void HelmReporter::printReport()
{
  cout << endl << endl << endl << endl << endl << endl;
  if(m_report_life) {
    vector<string> report_lines = m_life_events.getReport();
    unsigned int i, vsize = report_lines.size();
    for(i=0; i<vsize; i++)
      cout << report_lines[i] << endl;
  }
}



