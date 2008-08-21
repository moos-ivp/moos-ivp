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

  m_report = scanner.scan();
  if(m_report.size() == 0) {
    cout << "Empty log file - exiting." << endl;
    return;
  }

  m_report.sort(m_sort_style);

  int line_digits  = (int)(log10(m_report.getMaxLines())) + 2;
  int char_digits  = (int)(log10(m_report.getMaxChars())) + 2;
  int start_digits = (int)(log10(m_report.getMaxStartTime())) + 4;
  int stop_digits  = (int)(log10(m_report.getMaxStopTime())) + 4;

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
  string svname_digits = intToString(m_report.getVarNameMaxLength());
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

  unsigned int i, vsize = m_report.size();
  for(i=0; i<vsize; i++) {
    string varname     = m_report.getVarName(i);
    string varsources  = m_report.getVarSources(i);
    double first       = m_report.getVarFirstTime(i);
    double last        = m_report.getVarLastTime(i);
    unsigned int lcnt  = m_report.getVarCount(i);
    unsigned int chars = m_report.getVarChars(i);

    string sfirst = doubleToString(first, 2);
    string slast  = doubleToString(last,  2);
    
    printf(bformat_string.c_str(),  varname.c_str(), lcnt, 
	   chars, sfirst.c_str(), slast.c_str(), varsources.c_str());
  }

  string start_time = doubleToString(m_report.getMinStartTime(),2);
  string stop_time  = doubleToString(m_report.getMaxStopTime(),2);

  unsigned int digits = m_report.getVarNameMaxLength() + line_digits + 
    char_digits + start_digits + stop_digits;
  string separator = "------------------";
  for(i=0; i<digits; i++)
    separator += "-";
  printf("%s\n", separator.c_str());
  printf("Total variables: %d\n", m_report.size());
  printf("Start/Stop Time: %s / %s\n", start_time.c_str(),
	 stop_time.c_str());
}



//--------------------------------------------------------
// Procedure: appStatReport
//     Notes: 

void ScanHandler::appStatReport()
{
  if(m_report.size() == 0)
    return;

  unsigned int i, vsize;

  m_report.fillAppStats();
  vector<string> all_sources = m_report.getAllSources();
  vsize = all_sources.size();

  if(vsize == 0)
    return;

  int max_source_name_length = 0;
  for(i=0; i<vsize; i++) {
    string source = all_sources[i];
    if(source.length() > max_source_name_length)
      max_source_name_length = source.length();
  }

  printf("\n\n\n");

  string app_header = "MOOS Application";
  app_header = padString(app_header, max_source_name_length+6, false);

  string app_dash = "---------------";
  app_dash = padString(app_dash, max_source_name_length+6, false);

  printf("%s  %s  %s  %s  %s\n", app_header.c_str(), "Total Lines", 
	 "Total Chars", "Lines/Total", "Chars/Total");
  printf("%s  -----------  -----------  -----------  -----------\n", 
	 app_dash.c_str());


  for(i=0; i<vsize; i++) {
    string source = all_sources[i];
    double lines  = m_report.getLinesBySource(source);
    double chars  = m_report.getCharsBySource(source);
    double lines_pct = m_report.getLinesPctBySource(source);
    double chars_pct = m_report.getCharsPctBySource(source);

    string s_lines = doubleToString(lines,0);
    string s_chars = doubleToString(chars,0);
    string s_lines_pct = doubleToString((lines_pct*100.0),2);
    string s_chars_pct = doubleToString((chars_pct*100.0),2);

    s_lines = padString(dstringCompact(s_lines), 8, true);
    s_chars = padString(dstringCompact(s_chars), 10, true);
    s_lines_pct = padString(s_lines_pct, 6, true);
    s_chars_pct = padString(s_chars_pct, 6, true);
    source = padString(source, max_source_name_length, false);

    printf("%s           %s   %s       %s       %s\n", 
	   source.c_str(), s_lines.c_str(), 
	   s_chars.c_str(), s_lines_pct.c_str(), s_chars_pct.c_str());
  }
  
}

