/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ScanHandle.cpp                                       */
/*    DATE: June 5rd, 2008                                       */
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
#include <cmath>
#include <cstdio>
#include "MBUtils.h"
#include "ALogScanner.h"
#include "ScanHandler.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------
// Constructor

ScanHandler::ScanHandler()
{
  m_pcolor_map["pHelmIvP"]       = "blue";
  m_pcolor_map["pNodeReporter"]  = "green";
  
  m_pcolors.push_back("blue");
  m_pcolors.push_back("green");
  m_pcolors.push_back("red");
  m_pcolors.push_back("magenta");
  m_pcolors.push_back("cyan");
  m_pcolors.push_back("yellow");
  m_pcolors.push_back("nocolor");

  m_next_color_ix = 2;

  m_use_colors = true;
  m_use_full_source = true;
}

//--------------------------------------------------------
// Procedure: setParam

bool ScanHandler::setParam(const string& param, const string& value)
{
  bool handled = true;
  if(param == "sort_style")
    m_sort_style = value;
  else if(param == "proc_colors")
    handled = setBooleanOnString(m_use_colors, value);
  else if(param == "use_full_source")
    handled = setBooleanOnString(m_use_full_source, value);
  else
    handled = false;

  return(handled);
}

//--------------------------------------------------------
// Procedure: procColor

string ScanHandler::procColor(string proc_name)
{
  string pcolor = m_pcolor_map[proc_name];
  if(pcolor != "") {
    return(pcolor);
  }

  unsigned int total_colors = m_pcolors.size(); 
  if(total_colors == 0)
    return("nocolor");
  
  if(m_next_color_ix >= total_colors-1) {
    string last_color = m_pcolors[total_colors-1];
    m_pcolor_map[proc_name] = last_color;
    return(last_color);
  }
  else {
    string next_color = m_pcolors[m_next_color_ix];
    m_pcolor_map[proc_name] = next_color;
    m_next_color_ix++;
    return(next_color);
  }

}

//--------------------------------------------------------
// Procedure: handle

bool ScanHandler::handle(const string& alogfile, bool rate_only)
{
  ALogScanner scanner;

  scanner.setUseFullSource(m_use_full_source);
  
  bool ok = scanner.openALogFile(alogfile);
  if(!ok) {
    cout << "Unable to find or open " << alogfile << " - Exiting." << endl;
    return(false);
  }

  cout << "Scanning " << alogfile << "... " << flush;
  if(rate_only)
    m_report = scanner.scanRateOnly();
  else
    m_report = scanner.scan();

  if(!rate_only && (m_report.size() == 0)) {
    cout << "Empty log file - exiting." << endl;
    return(false);
  }
  return(true);
}


//--------------------------------------------------------
// Procedure: varStatReport

void ScanHandler::varStatReport()
{
  m_report.sort(m_sort_style);

#ifdef _WIN32
  int line_digits  = (int)(log10( (double)m_report.getMaxLines())) + 2;
  int char_digits  = (int)(log10( (double)m_report.getMaxChars())) + 4;
  int start_digits = (int)(log10( m_report.getMaxStartTime())) + 4;
  int stop_digits  = (int)(log10( m_report.getMaxStopTime())) + 4;
#else 
  int line_digits  = static_cast<int>(log10(static_cast<double>(m_report.getMaxLines()))) + 2;
  int char_digits  = static_cast<int>(log10(static_cast<double>(m_report.getMaxChars()))) + 4;
  int start_digits = (int)(log10(m_report.getMaxStartTime())) + 4;
  int stop_digits  = (int)(log10(m_report.getMaxStopTime())) + 4;
#endif 

  //cout << "line_digits:  " << line_digits  << endl;
  //cout << "char_digits:  " << char_digits  << endl;
  //cout << "start_digits: " << start_digits << endl;
  //cout << "stop_digits:  " << stop_digits  << endl;

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
  bformat_string += ("%" + schar_digits + "s  ");
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
    string schars = intToCommaString(chars);

    if(m_use_colors) {
      string varsources_copy = varsources;
      string first_source = biteString(varsources_copy, ',');

      string key = first_source;
      if(strContains(first_source, ':')) {
	string app_name = biteString(first_source, ':');
	if(app_name != "")
	  key = app_name;
	else
	  key = first_source;
      }
      string color = termColor(procColor(key));
      printf("%s", color.c_str());
    }

    printf(bformat_string.c_str(),  varname.c_str(), lcnt, 
	   schars.c_str(), sfirst.c_str(), slast.c_str(), varsources.c_str());

    if(m_use_colors)
      printf("%s", termColor().c_str());
  }

  string start_time = doubleToString(m_report.getMinStartTime(),2);
  string stop_time  = doubleToString(m_report.getMaxStopTime(),2);

  cout << "--------------------------------------------------" << endl;
  cout << "Total variables: " <<  m_report.size() << endl;
  cout << "Start/Stop Time: " << start_time << " / " << stop_time << endl;

}

//--------------------------------------------------------
// Procedure: dataRateReport

void ScanHandler::dataRateReport()
{
  string data_rate  = doubleToString(m_report.getDataRate(),2);
  string data_ratek = doubleToString(m_report.getDataRate()/1000,2);

  cout << "Data Rate (chars/sec): " << data_rate;
  cout << "  (" << data_ratek << "K/sec)" << endl;
}


//--------------------------------------------------------
// Procedure: appStatReport

void ScanHandler::appStatReport()
{
  if(m_report.size() == 0)
    return;

  m_report.fillAppStats();
  vector<string> all_sources = m_report.getAllSources();

  unsigned int vsize = all_sources.size();
  if(vsize == 0)
    return;
  
  unsigned int i, max_source_name_length = 0;
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
    unsigned int lines = m_report.getLinesBySource(source);
    unsigned int chars = m_report.getCharsBySource(source);
    double lines_pct = m_report.getLinesPctBySource(source);
    double chars_pct = m_report.getCharsPctBySource(source);

    string s_lines = uintToString(lines);
    string s_chars = intToCommaString(chars);
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

//--------------------------------------------------------
// Procedure: loglistReport
//     Notes: 

void ScanHandler::loglistReport()
{
  unsigned int total_vars = m_report.size();

  cout << endl << endl;
  cout << "List of Logged variables (" << total_vars << ")" << endl;
  cout << "---------------------------------" << endl;
  for(unsigned int i=0; i<total_vars; i++) {
    cout << "  Log = " << m_report.getVarName(i) << endl;
  }
}









