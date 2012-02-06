/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GeneralLauncher.cpp                                  */
/*    DATE: Oct 1st, 2011                                        */
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "LogUtils.h"
#include "MBUtils.h"
#include "MBTimer.h"
#include "GeneralLauncher.h"
#include "FileBuffer.h"
#include "ColorParse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

GeneralLauncher::GeneralLauncher()
{
  m_min_clip_time   = 0; 
  m_max_clip_time   = 0;
  m_min_clip_time_set = false;
  m_max_clip_time_set = false;

  m_now_time   = 0;
  m_now_time_set = false;

  m_global_mintime_utc = 0;
  m_global_maxtime_utc = 0;
}

//-------------------------------------------------------------
// Procedure: checkForMinMaxTime
//   Purpose: Handle the command line args --mintime and --maxtime
//            If one has been set prior to the other, then the 2nd
//            provided argument must respect the first or else it
//            will simply be ignored. That is, if maxtime has been
//            set, --mintime must provide a time < maxtime. 

void GeneralLauncher::checkForMinMaxTime(int argc, char **argv)
{
  int i;
  for(i=1; i<argc; i++) {
    string argi = argv[i];
    unsigned int len = argi.length();
    if((len>10) && !strncmp(argi.c_str(), "--mintime=", 10)) {
      string value = argv[i]+10;
      double dval  = atof(value.c_str());
      if(isNumber(value) && 
	 (!m_max_clip_time_set || (dval < m_max_clip_time))) {
	m_min_clip_time = dval; 
	m_min_clip_time_set = true;
      }
    }
    else if((len>10) && !strncmp(argi.c_str(), "--maxtime=", 10)) {
      string value = argv[i]+10;
      double dval  = atof(value.c_str());
      if(isNumber(value) && 
	 (!m_min_clip_time_set || (dval > m_min_clip_time))) {
	m_max_clip_time = dval; 
	m_max_clip_time_set = true;
      }
    }
    else if((len>10) && !strncmp(argi.c_str(), "--nowtime=", 10)) {
      string value = argv[i]+10;
      double dval  = atof(value.c_str());
      if(isNumber(value)) {
	m_now_time = dval; 
	m_now_time_set = true;
      }
    }
  }
}

//-------------------------------------------------------------
// Procedure: setALogFiles
//            Find all .alog files on the commmand line

void GeneralLauncher::setALogFiles(int argc, char **argv)
{
  for(int i=1; i<argc; i++)
    if(strContains(argv[i], ".alog"))
      m_alog_files.push_back(argv[i]);
}


//-------------------------------------------------------------
// Procedure: setALogFileLogStarts

bool GeneralLauncher::setALogFileLogStarts()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Setting alog file logstarts..." << endl;

  bool no_problems = true;
  unsigned int i, vsize = m_alog_files.size();
  for(i=0; i<vsize; i++) {
    double logstart = getLogStartFromFile(m_alog_files[i]);
    if(logstart == 0) {
      no_problems = false;
      cout << termColor("red");
      cout << "Undetected LOGSTART in file: " << m_alog_files[i] << endl;
      cout << termColor();
    }

    cout << termColor("green");
    cout << "Logstart=" << doubleToString(logstart) << " for " << 
      m_alog_files[i] << endl;
    cout << termColor();

    m_log_starts.push_back(logstart);
  }

  parse_timer.stop();
  cout << termColor("green");
  cout << "Done detecting .alog file logstarts. ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();
  return(no_problems);
}
  
