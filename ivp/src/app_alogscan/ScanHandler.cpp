/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ScanHandle.cpp                                       */
/*    DATE: June 5rd, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include "MBUtils.h"
#include "ALogScanner.h"
#include "ScanHandler.h"

using namespace std;


//--------------------------------------------------------
// Procedure: setParam
//     Notes: 

void ScanHandler::setParam(const string& param, const string& value)
{
  if(param == "sort_style")
    m_sort_style = value;
}

//--------------------------------------------------------
// Procedure: handle
//     Notes: 

void ScanHandler::handle(const string& alogfile)
{
  ALogScanner scanner;
  bool ok = scanner.openALogFile(alogfile);
  if(!ok) {
    cout << "Unable to find or open " << alogfile << " - Exiting." << endl;
    return;
  }

  ScanReport report = scanner.scan();
  if(report.size() == 0) {
    cout << "Empty log file - exiting." << endl;
    return;
  }

  report.sort(m_sort_style);

  int line_digits  = (int)(log10(report.getMaxLines())) + 2;
  int char_digits  = (int)(log10(report.getMaxChars())) + 2;
  int start_digits = (int)(log10(report.getMaxStartTime())) + 4;
  int stop_digits  = (int)(log10(report.getMaxStopTime())) + 4;

  cout << "line_digits:  " << line_digits  << endl;
  cout << "char_digits:  " << char_digits  << endl;
  cout << "start_digits: " << start_digits << endl;
  cout << "stop_digits:  " << stop_digits  << endl;

  if(line_digits  < 5)  line_digits  = 5;
  if(char_digits  < 5)  char_digits  = 5;
  if(start_digits < 6)  start_digits = 6;
  if(stop_digits  < 6)  stop_digits  = 6;

  string sline_digits  = intToString(line_digits);
  string schar_digits  = intToString(char_digits);
  string svname_digits = intToString(report.getVarNameMaxLength());
  string sstart_digits = intToString(start_digits);
  string sstop_digits  = intToString(stop_digits);

  string hformat_string = ("%-" + svname_digits + "s ");
  hformat_string += ("%" + sline_digits + "s ");
  hformat_string += ("%" + schar_digits + "s  ");
  hformat_string += ("%" + sstart_digits + "s  ");
  hformat_string += ("%" + sstop_digits  + "s  %s\n");

  string bformat_string = ("%-" + svname_digits + "s ");
  bformat_string += ("%" + sline_digits + "d ");
  bformat_string += ("%" + schar_digits + "d  ");
  bformat_string += ("%" + sstart_digits + "s  ");
  bformat_string += ("%" + sstop_digits  + "s  %s\n");

  printf("\n");
  printf(hformat_string.c_str(),
	 "Variable Name", "Lines", "Chars", " Start", "  Stop", "Sources");
  printf(hformat_string.c_str(),
	 "-------------", "-----", "-----", "------", "------", "-------");

  unsigned int i, vsize = report.size();
  for(i=0; i<vsize; i++) {
    string varname     = report.getVarName(i);
    string varsources  = report.getVarSources(i);
    double first       = report.getVarFirstTime(i);
    double last        = report.getVarLastTime(i);
    unsigned int lcnt  = report.getVarCount(i);
    unsigned int chars = report.getVarChars(i);

    string sfirst = doubleToString(first, 2);
    string slast  = doubleToString(last,  2);
    
    printf(bformat_string.c_str(),  varname.c_str(), lcnt, 
	   chars, sfirst.c_str(), slast.c_str(), varsources.c_str());
  }

  string start_time = doubleToString(report.getMinStartTime(),2);
  string stop_time  = doubleToString(report.getMaxStopTime(),2);

  unsigned int digits = report.getVarNameMaxLength() + line_digits + 
    char_digits + start_digits + stop_digits;
  string separator = "------------------";
  for(i=0; i<digits; i++)
    separator += "-";
  printf("%s\n", separator.c_str());
  printf("Total variables: %d\n", report.size());
  printf("Start/Stop Time: %s / %s\n", start_time.c_str(),
	 stop_time.c_str());
  
}

