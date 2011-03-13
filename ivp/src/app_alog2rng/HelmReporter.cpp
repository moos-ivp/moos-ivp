/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmReporter.cpp                                     */
/*    DATE: March 10th, 2010                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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

