/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LauncherULH.cpp                                      */
/*    DATE: Oct0411                                              */
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "LogUtils.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "MBTimer.h"
#include "LauncherULH.h"
#include "FileBuffer.h"
#include "NodeRecord.h"
#include "NodeRecordUtils.h"
#include "PopulatorHSM.h"
#include "ColorParse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

LauncherULH::LauncherULH() : GeneralLauncher()
{
  m_gui_width  = 475;
  m_gui_height = 600;
  m_gui        = 0;
}

//-------------------------------------------------------------
// Procedure: launch

ULH_GUI *LauncherULH::launch(int argc, char **argv)
{
  MBTimer total_timer;
  total_timer.start();

  checkForMinMaxTime(argc, argv); // Done in GeneralLauncher
  setALogFiles(argc, argv);       // Done in GeneralLauncher
  setALogFileLogStarts();         // Done in GeneralLauncher

  
  bool ok = parseALogFile();
  if(!ok) {
    cout << "Problem parsing alog file(s)." << endl;
    return(0);
  }

  setSizeOfGUI(argc, argv);

  buildPlotsAndGUI();

  total_timer.stop();
  cout << termColor("blue") << "Done logview launch time (cpu): ";
  cout << total_timer.get_float_cpu_time() << endl;
  cout << "Done logview launch time (wall): ";
  cout << total_timer.get_float_wall_time() << termColor() << endl;
  
  return(m_gui);
}


//-------------------------------------------------------------
// Procedure: setSizeOfGUI
//            Determine the GUI size
//  Switches: --geometry=large
//            --geometry=medium
//            --geometry=small
//            --geometry=xsmall
//            --geometry=950x600

void LauncherULH::setSizeOfGUI(int argc, char **argv)
{
  // Set the default values (~large) if no geometry switch provided.
  for(int i=1; i<argc; i++) {
    if(!strncmp(argv[i], "--geometry=", 10)) {
      string argi  = tolower(argv[i]);
      string front = biteString(argi, '=');
      string value = argi;
      
      if(value == "large")  {
	m_gui_width  = 1400;
	m_gui_height = 1100;
      }
      else if(value == "medium") {
	m_gui_width  = 1190;
	m_gui_height = 935;
      }
      else if(value == "small")  {
	m_gui_width  = 980;
	m_gui_height = 770;
      }
      else if(value == "xsmall") {
	m_gui_width  = 770;
	m_gui_height = 605;
      }
      else {
	vector<string> svector = parseString(value, 'x');
	if(svector.size() == 2) {
	  string width  = svector[0];
	  string height = svector[1];
	  if(isNumber(width) && isNumber(height)) {
	    m_gui_width  = atof(width.c_str());
	    m_gui_height = atof(width.c_str());
	    m_gui_width  = vclip(m_gui_width, 750, 1920);
	    m_gui_height = vclip(m_gui_height, 600, 1200);
	  }
	}
      }
    }
  }
}


//-------------------------------------------------------------
// Procedure: parseALogFile
//            Parse the .alog file given by the index

bool LauncherULH::parseALogFile()
{
  if(m_alog_files.size() == 0) {
    cout << termColor("red") << "No Log File!" << termColor() << endl;
    return(false);
  }  
  if(m_log_starts.size() == 0) {
    cout << termColor("red") << "No LogStart!" << termColor() << endl;
    return(false);
  }  
  
  string alog_file = m_alog_files[0];
  double log_start = m_log_starts[0];

  cout << "Parsing alog file: " << alog_file << endl;

  FILE *f = fopen(alog_file.c_str(), "r");
  if(!f) {
    cout << termColor("red");
    cout << "Unable to open alogfile: " << alog_file << endl;
    cout << termColor();
    return(false);
  }

  MBTimer parse_timer;
  parse_timer.start();

  vector<string>    node_reports;
  IvPDomain         ivp_domain;

  bool   time_read = false;
  double min_time_local = 0;
  double max_time_local = 0;
  
  bool empty_file = true;
  bool done = false;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(f);
    string status = entry.getStatus();
    if(status == "eof")
      done = true;
    else if(status != "invalid") {
      empty_file = false;
      string var    = entry.getVarName();
      double tstamp = entry.getTimeStamp();

      if(!time_read || (tstamp < min_time_local))
	min_time_local = tstamp;
      if(!time_read || (tstamp > max_time_local))
	max_time_local = tstamp;
      time_read = true;
      
      if((m_vname_bynoderep.empty()) && (var=="NODE_REPORT_LOCAL")) {
	NodeRecord record = string2NodeRecord(entry.getStringVal());
	m_vname_bynoderep = record.getName();
      }
      else if((m_vname_bymoosdb.empty()) && (var=="DB_CLIENTS")) {
	string src = entry.getSource();
	if(strBegins(src, "MOOSDB_") && (src.length() > 7))
	  m_vname_bymoosdb = src.substr(7);
      }
      else if(var=="IVPHELM_DOMAIN") {
	string domain_str = entry.getStringVal();
	m_ivp_domain = stringToDomain(domain_str);
      }
      
      if((!m_min_clip_time_set || (tstamp >= m_min_clip_time)) &&
	 (!m_max_clip_time_set || (tstamp <= m_max_clip_time))) {
	if((var == "IVPHELM_SUMMARY") ||
	   (var == "IVPHELM_ENGAGED") ||
	   (var == "BHV_WARNING") ||
	   (var == "BHV_ERROR"))
	  m_entries.push_back(entry);
      }
    }
  }

  fclose(f);

  if((m_vname_bymoosdb != "") && (m_vname_bynoderep != "") &&
     (m_vname_bymoosdb != m_vname_bynoderep)) {
    cout << termColor("red");
    cout << "Warning: Discrepancy in determining vehicle name:" << endl;
    cout << " Name according to the MOOSDB: " << m_vname_bymoosdb << endl;
    cout << " Name according to NodeReports: " << m_vname_bynoderep << endl;
    cout << " Using the name according to the node report. " << endl;
    cout << termColor();    
  }

  //------------------------------------------------------------
  // Calculate the min/max utc time based on min/max local times
  // noted and the declared logstart utc time.
  m_global_mintime_utc = min_time_local + log_start;
  m_global_maxtime_utc = max_time_local + log_start;
  //-------------------------------------------------------------

  m_vname = m_vname_bynoderep;
  if(m_vname == "")
    m_vname = m_vname_bymoosdb;

  if(m_vname == "") {
    cout << "Warning: The vehicle name could not be determined." << endl;
    cout << " Using the name \"ghost\".  " << endl;
    cout << termColor();    
    return(false);
  }
  
  parse_timer.stop();
  cout << termColor("green");
  cout << "Done parsing " << alog_file << " - total parse time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();

  if(empty_file) {
    cout << termColor("red");
    cout << "EMPTY!!! (THIS MAY BE WORTH CHECKING)" << endl;
    cout << termColor();    
    return(false);
  }

  return(true);
}
  

//-------------------------------------------------------------
// Procedure: buildPlotsAndGUI

bool LauncherULH::buildPlotsAndGUI()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Refining alog data to build TimePlots..." << endl;

  PopulatorHSM pop_hsm;
  pop_hsm.setVName(m_vname);
  pop_hsm.setLogStartUTC(m_log_starts[0]);

  bool ok = pop_hsm.populateFromEntries(m_entries);
  parse_timer.stop();
  if(!ok) {
    cout << termColor("red") << "Unable to build Time Plots!!!" << endl;
    cout << "(Likely due to lack of relevant entries in .alog file)";
    cout << endl << termColor() << endl;
    return(false);
  }

  cout << termColor("green");
  cout << "Done: TimePlot parse time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();

  //-------------------------------------------------------------
  // Start Building the GUI
  //-------------------------------------------------------------
  string title = "uLogViewHelm-" + m_vname;
  m_gui = new ULH_GUI(m_gui_width, m_gui_height, title.c_str());
  if(!m_gui)
    return(false);

  m_gui->setWindowLayout(m_window_layout);

  const HelmTPlot& helm_plot = pop_hsm.getHelmPlot();
  m_gui->setHelmPlot(helm_plot);

  m_gui->hsm.addVarPlot(pop_hsm.getErrorsVPlot());
  m_gui->hsm.addVarPlot(pop_hsm.getWarningsVPlot());
  m_gui->hsm.addVarPlot(pop_hsm.getEngagedVPlot());
  
  m_gui->updateXY();

  if(m_now_time_set)
    m_gui->setCurrTime(m_now_time);
  else
    m_gui->setCurrTime(-1); // GUI will seek a "start_time hint"
    
  return(true);
}





