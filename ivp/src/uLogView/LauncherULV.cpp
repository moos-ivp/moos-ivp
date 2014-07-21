/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LauncherULV.cpp                                      */
/*    DATE: Sep 25th, 2011                                       */
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
#include "LauncherULV.h"
#include "FileBuffer.h"
#include "NodeRecordUtils.h"
#include "ColorParse.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

LauncherULV::LauncherULV() : GeneralLauncher()
{
  m_tif_file   = "";
  m_gui_width  = 1250;
  m_gui_height = 900;
  m_gui        = 0;
}

//-------------------------------------------------------------
// Procedure: launch

ULV_GUI *LauncherULV::launch(int argc, char **argv)
{
  MBTimer total_timer;
  total_timer.start();

  checkForMinMaxTime(argc, argv);
  setBackground(argc, argv);
  setSizeOfGUI(argc, argv);
  setWindowLayout(argc, argv);
  setALogFiles(argc, argv);  

  bool ok = setALogFileLogStarts();
  if(!ok)
    return(0);

  parseALogFiles();
  
  buildTimePlots();

  ok = ok && buildGraphical();

  total_timer.stop();
  cout << termColor("blue") << "Done logview launch time (cpu): ";
  cout << total_timer.get_float_cpu_time() << endl;
  cout << "Done logview launch time (wall): ";
  cout << total_timer.get_float_wall_time() << termColor() << endl;

  if(ok)
    return(m_gui);
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: setBackground
//            Find the tif (texture background) file if provided
//    Switch: --background=value 
//            --background=none

void LauncherULV::setBackground(int argc, char **argv)
{
  m_tif_file = "Default.tif";
  for(int i=1; i<argc; i++) {
    if(!strncmp(argv[i], "--background=", 13)) {
      string argi  = tolower(argv[i]);
      string front = biteString(argi, '=');
      string value = argi;
      
      if((value == "mit") || (value=="charles"))
	m_tif_file = "AerialMIT.tif";
      else if((value == "wmit") || (value=="wireframe") || (value=="wf"))
	m_tif_file = "WireFrameMIT-1024.tif";
      else if((value == "mb") || (value=="monterey"))
	m_tif_file = "Monterey-2048.tif";
      else if((value == "mbd"))
	m_tif_file = "Monterey-2048-30-30-100.tif";
      else if((value == "fl") || (value == "forrest"))
	m_tif_file = "forrest19.tif";
      else if((value == "glint") || (value == "glint"))
	m_tif_file = "glintA.tif";
      else if(value == "none")
	m_tif_file = "";
      else
	m_tif_file = value;
    }
  }
  cout << "Tiff File: " << m_tif_file << endl;
}


//-------------------------------------------------------------
// Procedure: setSizeOfGUI
//            Determine the GUI size
//  Switches: --geometry=large
//            --geometry=medium
//            --geometry=small
//            --geometry=xsmall
//            --geometry=950x600

void LauncherULV::setSizeOfGUI(int argc, char **argv)
{
  // Set the default values (~large) if no geometry switch provided.
  m_gui_width  = 1250;
  m_gui_height = 950;
  
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
// Procedure: setWindowLayout
//            Determine the layouts of windows within the gui
//  Switches: --layout=normal
//            --layout=noipfs
//            --layout=fullview


void LauncherULV::setWindowLayout(int argc, char **argv)
{
  m_window_layout = "normal";
  
  return; // For now just allow normal mode until further debugging on the other modes

  for(int i=1; i<argc; i++) {
    if(!strncmp(argv[i], "--layout=", 9)) {
      string argi  = tolower(argv[i]);
      string front = biteString(argi, '=');
      string value = argi;
      
      if(value == "fullview") 
	m_window_layout = "fullview";
      else
	cout << "Unrecognized layout (" << value << ") ignored." << endl;
    }
  }
}

//-------------------------------------------------------------
// Procedure: parseALogFiles
//            Parse all .alog files on the commmand line

void LauncherULV::parseALogFiles()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Parsing alog files..." << endl;

  unsigned int i, vsize = m_alog_files.size();
  for(i=0; i<vsize; i++) {
    cout << "  Handling " << m_alog_files[i] << "...";
    cout << flush;
    bool non_empty = parseALogFile(i);
    if(non_empty)
      cout << "DONE" << endl;
    else {
      cout << termColor("red");
      cout << "EMPTY!!! (THIS MAY BE WORTH CHECKING)" << endl;
      cout << termColor();
    } 
  }

  parse_timer.stop();
  cout << termColor("green");
  cout << "Done parsing .alog files - total parse time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();
}
  
//-------------------------------------------------------------
// Procedure: parseALogFile
//            Parse the .alog file given by the index

bool LauncherULV::parseALogFile(unsigned int index)
{
  if(index >= m_alog_files.size())
    return(false);

  //-----------------------------------------------------------
  // We want to make sure that our set of vectors grows by one
  // no matter what. So grow them here with default values;

  vector<ALogEntry> entries_log_plot;
  vector<ALogEntry> entries_vplug_plot;
  string            vname;
  string            vtype;
  double            vlength  = 0;

  m_vehicle_name.push_back(vname);
  m_vehicle_type.push_back(vtype);
  m_vehicle_length.push_back(vlength);
  m_entries_log_plot.push_back(entries_log_plot);
  m_entries_vplug_plot.push_back(entries_vplug_plot);
  //-----------------------------------------------------------

  string filestr = m_alog_files[index];
  FILE *f = fopen(filestr.c_str(), "r");
  if(!f)
    return(false);

  bool   time_read = false;
  double min_time_local = 0;
  double max_time_local = 0;

  bool   node_info_found = false;
  bool   empty_file = true;
  bool   done = false;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(f);
    string status = entry.getStatus();
    if(status == "eof")
      done = true;
    else if(status != "invalid") {
      empty_file = false;
      string var    = entry.getVarName();
      bool   isnum  = entry.isNumerical();
      double tstamp = entry.getTimeStamp();

      if(!time_read || (tstamp < min_time_local))
	min_time_local = tstamp;
      if(!time_read || (tstamp > max_time_local))
	max_time_local = tstamp;
      time_read = true;
      
      // Fallback method for determining the vehicle name
      if((vname == "") && (var == "DB_CLIENTS")) {
	string src = entry.getSource();
	if(strBegins(src, "MOOSDB_") && (src.length() > 7))
	  vname = src.substr(7);
      }
      
      if((!m_min_clip_time_set || (tstamp >= m_min_clip_time)) &&
	 (!m_max_clip_time_set || (tstamp <= m_max_clip_time))) {
	if(isnum)
	  entries_log_plot.push_back(entry);
	else {
	  if(!node_info_found && (var=="NODE_REPORT_LOCAL")) {
	    NodeRecord record = string2NodeRecord(entry.getStringVal());
	    vname = record.getName();
	    vtype = record.getType();
	    vlength = record.getLength();
	    node_info_found = true;
	  }
	  else if((var == "VIEW_POINT")   || (var == "VIEW_POLYGON") ||
		  (var == "VIEW_SEGLIST") || (var == "VIEW_CIRCLE")  ||
		  (var == "GRID_INIT")    || (var == "VIEW_MARKER")  ||
		  (var == "GRID_DELTA")   || (var == "VIEW_RANGE_PULSE")) {
	    entries_vplug_plot.push_back(entry);
	  }
	}
      }
    }
  }
    
  fclose(f);
  if(empty_file)
    return(false);

  // Worst case fall back vehicle name.
  if(vname == "") 
    vname = "V" + uintToString(index);

  m_vehicle_name[index] = vname;
  m_vehicle_type[index] = vtype;
  m_vehicle_length[index] = vlength;
  m_entries_log_plot[index] = entries_log_plot; 
  m_entries_vplug_plot[index] = entries_vplug_plot;

  double min_utc_time = min_time_local + m_log_starts[index];
  double max_utc_time = max_time_local + m_log_starts[index];
  if((index == 0) || (min_utc_time < m_global_mintime_utc))
    m_global_mintime_utc = min_utc_time;
  if((index == 0) || (max_utc_time > m_global_maxtime_utc))
    m_global_maxtime_utc = max_utc_time;

  return(true);
}
  

//-------------------------------------------------------------
// Procedure: buildTimePlots

bool LauncherULV::buildTimePlots()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Refining alog data to build TimePlots..." << endl;

  unsigned int i, vsize = m_alog_files.size();
  for(i=0; i<vsize; i++) {
    // Handle Log TimePlots
    Populator_LogTPlots pop_lp;
    pop_lp.setLogStartUTC(m_log_starts[i]);
    pop_lp.setVName(m_vehicle_name[i]); 
    pop_lp.populateFromEntries(m_entries_log_plot[i]);
    
    vector<LogTPlot> lp_vector;
    unsigned int k, lp_size = pop_lp.size();
    for(k=0; k<lp_size; k++)
      lp_vector.push_back(pop_lp.getLogPlot(k));

    if(lp_size == 0) {
      cout << termColor("red");
      cout << "Warning: No Numerical Entries (LogPlots) in ";
      cout << m_alog_files[i] << "!!!";
      cout << termColor() << endl;
    }
    m_log_plots.push_back(lp_vector);

    // Handle VPlug TimePlots
    Populator_VPlugTPlots pop_vp;
    pop_vp.setLogStartUTC(m_log_starts[i]);
    bool ok = pop_vp.populateFromEntries(m_entries_vplug_plot[i]);
    if(!ok) {
      cout << "Problem with file" << m_alog_files[i] << endl;
      return(false);
    }
    m_vplug_plots.push_back(pop_vp.getVPlugPlot());
  }

  parse_timer.stop();
  cout << termColor("green");
  cout << "Done build TimePlots - total build time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();
  return(true);
}

//-------------------------------------------------------------
// Procedure: buildGraphical

bool LauncherULV::buildGraphical()
{
  m_gui = new ULV_GUI(m_gui_width, m_gui_height, "uLogView");
  if(!m_gui)
    return(false);

  m_gui->setWindowLayout(m_window_layout);

  // Populate the GUI with the previously built LogPlots

  unsigned int i, vsize = m_log_plots.size(); // vehicle (alog) index
  for(i=0; i<vsize; i++) {
    NavPlotElement element; 

    // All of the log plots will be sent to the LogPlotViewer. 
    // Only some (X,Y,HDG) log plots will be held for NavPlotViewer

    unsigned int j, jsize = m_log_plots[i].size();
    for(j=0; j<jsize; j++) {
      // Send ALL the LogPlots to the GUI LogPlotViewer component
      m_gui->addLogPlot(m_log_plots[i][j]);
      // Send CERTAIN LogPlots to the GUI NavPlotViewer component
      if(m_log_plots[i][j].getVarName() == "NAV_X")
	element.setLogPlotNAVX(m_log_plots[i][j]);
      else if(m_log_plots[i][j].getVarName() == "NAV_Y")
	element.setLogPlotNAVY(m_log_plots[i][j]);
      else if(m_log_plots[i][j].getVarName() == "NAV_HEADING")
	element.setLogPlotHDG(m_log_plots[i][j]);
    }

    // Populate teh GUI NavPlotViewer with prior-built VPlugPlots
    element.setVPlugPlot(m_vplug_plots[i]);
    element.setVName(m_vehicle_name[i]);
    element.setVType(m_vehicle_type[i]);
    element.setVLength(m_vehicle_length[i]);
    element.setLogStartUTC(m_log_starts[i]);

    element.initialize(); // Crucial! Sets the bounds etc.
    m_gui->np_viewer->addElement(element);
  }
  
  m_gui->updateXY();
  m_gui->np_viewer->setParam("tiff_file", m_tif_file);

  m_gui->np_viewer->setMinMaxTimeUTC(m_global_mintime_utc, 
				     m_global_maxtime_utc);
  m_gui->lp_viewer->setMinMaxTimeUTC(m_global_mintime_utc, 
				     m_global_maxtime_utc);

  if(m_now_time_set)
    m_gui->setCurrTime(m_now_time);
  else {
    double window = m_global_maxtime_utc - m_global_mintime_utc;
    double start  = m_global_mintime_utc + (0.25 * window);
    m_gui->setCurrTime(start);
  }
    
  return(true);
}





