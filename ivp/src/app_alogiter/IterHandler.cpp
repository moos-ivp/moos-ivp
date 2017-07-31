/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IterHandler.cpp                                      */
/*    DATE: Dec 22nd, 2013                                       */
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
#include <cmath>
#include "MBUtils.h"
#include "IterHandler.h"
#include "LogUtils.h"

using namespace std;


//--------------------------------------------------------
// Constructor

IterHandler::IterHandler()
{
  m_file_in  = 0;
}

//--------------------------------------------------------
// Procedure: handle
//     Notes: 

bool IterHandler::handle(const string& alogfile)
{
  m_file_in = fopen(alogfile.c_str(), "r");
  if(!m_file_in) {
    cout << "input not found or unable to open - exiting" << endl;
    return(false);
  }
  
  bool done = false;
  while(!done) {
    string line_raw = getNextRawLine(m_file_in);
    
    bool   line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_is_comment)
      continue;

    if(line_raw == "eof") 
      done = true;
    else {
      string varname = getVarName(line_raw);

      if(strEnds(varname, "_ITER_GAP")) {
	string app_name = findReplace(varname, "_ITER_GAP", "");
	string data_val = getDataEntry(line_raw);
	double iter_gap = atof(data_val.c_str());

	m_map_app_gap_total[app_name] += iter_gap;
	m_map_app_gap_count[app_name]++;

	if(m_map_app_gap_max.count(app_name) == 0) 
	  m_map_app_gap_max[app_name] = 0;
       	if(iter_gap > m_map_app_gap_max[app_name])
	  m_map_app_gap_max[app_name] = iter_gap;

	if(iter_gap > 1.25)
	  m_map_app_gap_count_1p25[app_name]++;
	if(iter_gap > 1.5)
	  m_map_app_gap_count_1p50[app_name]++;
	if(iter_gap > 2.0)
	  m_map_app_gap_count_2p00[app_name]++;


      }

      if(strEnds(varname, "_ITER_LEN")) {
	string app_name = findReplace(varname, "_ITER_LEN", "");
	string data_val = getDataEntry(line_raw);
	double iter_len = atof(data_val.c_str());
	m_map_app_len_total[app_name] += iter_len;
	m_map_app_len_count[app_name]++;

	if(m_map_app_len_max.count(app_name) == 0) 
	  m_map_app_len_max[app_name] = 0;
	if(iter_len > m_map_app_len_max[app_name])
	  m_map_app_len_max[app_name] = iter_len;

	if(iter_len > 0.25)
	  m_map_app_len_count_0p25[app_name]++;
	if(iter_len > 0.5)
	  m_map_app_len_count_0p50[app_name]++;
	if(iter_len > 0.75)
	  m_map_app_len_count_0p75[app_name]++;
	if(iter_len > 1.0)
	  m_map_app_len_count_1p00[app_name]++;
      }
    }
  }

  if(m_file_in)
    fclose(m_file_in);

  return(true);
}
  
//--------------------------------------------------------
// Procedure: printReport
  
void IterHandler::printReport()
{
  printReportGap();
  printReportLen();
  printReportSummary();
}

//--------------------------------------------------------
// Procedure: printReportGap
  
void IterHandler::printReportGap()
{
  unsigned int max_app_name_len = 10;
  map<string, unsigned int>::iterator p;
  for(p=m_map_app_gap_count.begin(); p!=m_map_app_gap_count.end(); p++) {
    string app_name = p->first;
    unsigned int app_name_len = app_name.length();
    if(app_name_len > max_app_name_len)
      max_app_name_len = app_name_len;
  }

  string empty(max_app_name_len, ' ');
  string appname("AppName");
  string dashes("-------------");

  empty   = padString(empty, max_app_name_len + 2, false);
  appname = padString(appname, max_app_name_len + 2, false);
  dashes  = padString(dashes, max_app_name_len + 2, false);

  cout << empty   << " GAP     GAP       PCT     PCT     PCT    " << endl;
  cout << appname << " MAX     AVG       >1.25   >1.50   >2.0   " << endl;
  cout << dashes  << "-----   -----      -----   -----   -----  " << endl;
    
  for(p=m_map_app_gap_count.begin(); p!=m_map_app_gap_count.end(); p++) {
    string app_name = p->first;
    cout << padString(app_name, max_app_name_len, false);
    
    double gap_cnt      = (double)(m_map_app_gap_count[app_name]);
    double gap_pct_1p25 = (double)(m_map_app_gap_count_1p25[app_name]) / gap_cnt;
    double gap_pct_1p50 = (double)(m_map_app_gap_count_1p50[app_name]) / gap_cnt;
    double gap_pct_2p00 = (double)(m_map_app_gap_count_2p00[app_name]) / gap_cnt;

    double gap_max = m_map_app_gap_max[app_name];
    double gap_avg = m_map_app_gap_total[app_name] / gap_cnt;
    
    string s_gap_max = doubleToString(gap_max, 2);
    string s_gap_avg = doubleToString(gap_avg, 2);
    string s_gap_pct_1p25 = doubleToString(gap_pct_1p25, 3);
    string s_gap_pct_1p50 = doubleToString(gap_pct_1p50, 3);
    string s_gap_pct_2p00 = doubleToString(gap_pct_2p00, 3);
    
    s_gap_max = padString(s_gap_max, 5, true);
    s_gap_avg = padString(s_gap_avg, 5, true);
    s_gap_pct_1p25 = padString(s_gap_pct_1p25, 5, true);
    s_gap_pct_1p50 = padString(s_gap_pct_1p50, 5, true);
    s_gap_pct_2p00 = padString(s_gap_pct_2p00, 5, true);
    
    cout << "  " << s_gap_max << "   " << s_gap_avg;
    cout << "      " << s_gap_pct_1p25;
    cout << "   " << s_gap_pct_1p50;
    cout << "   " << s_gap_pct_2p00 << endl;
  }
  cout << endl;
}



//--------------------------------------------------------
// Procedure: printReportLen
  
void IterHandler::printReportLen()
{
  unsigned int max_app_name_len = 10;
  map<string, unsigned int>::iterator p;
  for(p=m_map_app_gap_count.begin(); p!=m_map_app_gap_count.end(); p++) {
    string app_name = p->first;
    unsigned int app_name_len = app_name.length();
    if(app_name_len > max_app_name_len)
      max_app_name_len = app_name_len;
  }

  string empty(max_app_name_len, ' ');
  string appname("AppName");
  string dashes("-------------");

  empty   = padString(empty, max_app_name_len + 2, false);
  appname = padString(appname, max_app_name_len + 2, false);
  dashes  = padString(dashes, max_app_name_len + 2, false);

  cout << empty   << " LEN    LEN      PCT     PCT     PCT     PCT  " << endl;
  cout << appname << " MAX    AVG      >0.25   >0.50   >0.75   >1.0 " << endl;
  cout << dashes  << "-----  -----     -----   -----   -----   -----" << endl;
    
  for(p=m_map_app_gap_count.begin(); p!=m_map_app_gap_count.end(); p++) {
    string app_name = p->first;
    cout << padString(app_name, max_app_name_len, false);
    
    double len_cnt      = (double)(m_map_app_len_count[app_name]);
    double len_pct_0p25 = (double)(m_map_app_len_count_0p25[app_name]) / len_cnt;
    double len_pct_0p50 = (double)(m_map_app_len_count_0p50[app_name]) / len_cnt;
    double len_pct_0p75 = (double)(m_map_app_len_count_0p75[app_name]) / len_cnt;
    double len_pct_1p00 = (double)(m_map_app_len_count_1p00[app_name]) / len_cnt;

    double len_max = m_map_app_len_max[app_name];
    double len_avg = m_map_app_len_total[app_name] / len_cnt;
    
    string s_len_max = doubleToString(len_max, 2);
    string s_len_avg = doubleToString(len_avg, 2);
    string s_len_pct_0p25 = doubleToString(len_pct_0p25, 3);
    string s_len_pct_0p50 = doubleToString(len_pct_0p50, 3);
    string s_len_pct_0p75 = doubleToString(len_pct_0p75, 3);
    string s_len_pct_1p00 = doubleToString(len_pct_1p00, 3);
    
    s_len_max = padString(s_len_max, 5, true);
    s_len_avg = padString(s_len_avg, 5, true);
    s_len_pct_0p25 = padString(s_len_pct_0p25, 5, true);
    s_len_pct_0p50 = padString(s_len_pct_0p50, 5, true);
    s_len_pct_0p75 = padString(s_len_pct_0p75, 5, true);
    s_len_pct_1p00 = padString(s_len_pct_1p00, 5, true);
    
    cout << "  " << s_len_max << "  " << s_len_avg;
    cout << "     " << s_len_pct_0p25;
    cout << "   " << s_len_pct_0p50;
    cout << "   " << s_len_pct_0p75;
    cout << "   " << s_len_pct_1p00 << endl;
  }
  cout << endl;
}



//--------------------------------------------------------
// Procedure: printReportSummary

void IterHandler::printReportSummary()
{
  double app_count = 0;
  double app_total_gap = 0;
  double app_total_len = 0;

  map<string, unsigned int>::iterator p;
  for(p=m_map_app_gap_count.begin(); p!=m_map_app_gap_count.end(); p++) {
    string app_name = p->first;
    app_count++;

    double gap_cnt = (double)(m_map_app_gap_count[app_name]);
    double gap_avg = m_map_app_gap_total[app_name] / gap_cnt;

    double len_cnt = (double)(m_map_app_len_count[app_name]);
    double len_avg = m_map_app_len_total[app_name] / len_cnt;

    app_total_gap += gap_avg;
    app_total_len += len_avg;
  }

  double app_total_gap_avg = app_total_gap / app_count;
  double app_total_len_avg = app_total_len / app_count;

  string s_app_total_gap_avg = doubleToString(app_total_gap_avg,2);
  string s_app_total_len_avg = doubleToString(app_total_len_avg,2);

  cout << "Mission Summmary " << endl;
  cout << "---------------------------" << endl;
  cout << " Collective APP_GAP: " << s_app_total_gap_avg << endl;
  cout << " Collective APP_LEN: " << s_app_total_len_avg << endl;

  cout << endl;
}










