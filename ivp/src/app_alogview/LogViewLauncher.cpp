/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogViewLauncher.cpp                                  */
/*    DATE: May 31st, 2005                                       */
/*          August 2009 - Took over most of main.cpp's role      */
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LogUtils.h"
#include "MBUtils.h"
#include "MBTimer.h"
#include "LogViewLauncher.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

LogViewLauncher::LogViewLauncher()
{
  m_tif_file   = "";
  m_gui_width  = 1400;
  m_gui_height = 1100;
  m_gui        = 0;
  m_min_time   = 0; 
  m_max_time   = 0;
  m_now_time   = 0;
  m_min_time_set = false;
  m_max_time_set = false;
  m_now_time_set = false;
}

//-------------------------------------------------------------
// Procedure: launch

REPLAY_GUI *LogViewLauncher::launch(int argc, char **argv)
{
  MBTimer total_timer;
  total_timer.start();

  checkForMinMaxTime(argc, argv);
  setBackground(argc, argv);
  setSizeOfGUI(argc, argv);
  setWindowLayout(argc, argv);
  setALogFiles(argc, argv);  

  bool ok = setALogFileSkews();
  if(!ok)
    return(0);
  parseALogFiles();
  determineVehicleNames();
  
  buildLogPlots();
  buildHelmPlots();
  buildVPlugPlots();
  buildIPFPlots();

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
// Procedure: checkForMinMaxTime
//   Purpose: Handle the command line args --mintime and --maxtime
//            If one has been set prior to the other, then the 2nd
//            provided argument must respect the first or else it
//            will simply be ignored. That is, if maxtime has been
//            set, --mintime must provide a time < maxtime. 

void LogViewLauncher::checkForMinMaxTime(int argc, char **argv)
{
  int i;
  for(i=1; i<argc; i++) {
    string argi = argv[i];
    unsigned int len = argi.length();
    if((len>10) && !strncmp(argi.c_str(), "--mintime=", 10)) {
      string value = argv[i]+10;
      double dval  = atof(value.c_str());
      if(isNumber(value) && (!m_max_time_set || (dval < m_max_time))) {
	m_min_time = dval; 
	m_min_time_set = true;
      }
    }
    else if((len>10) && !strncmp(argi.c_str(), "--maxtime=", 10)) {
      string value = argv[i]+10;
      double dval  = atof(value.c_str());
      if(isNumber(value) && (!m_min_time_set || (dval > m_min_time))) {
	m_max_time = dval; 
	m_max_time_set = true;
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
// Procedure: setBackground
//            Find the tif (texture background) file if provided
//    Switch: --background=value 
//            --background=none

void LogViewLauncher::setBackground(int argc, char **argv)
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

void LogViewLauncher::setSizeOfGUI(int argc, char **argv)
{
  // Set the default values (~large) if no geometry switch provided.
  m_gui_width  = 1400;
  m_gui_height = 1100;
  
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


void LogViewLauncher::setWindowLayout(int argc, char **argv)
{
  m_window_layout = "normal";
  
  for(int i=1; i<argc; i++) {
    if(!strncmp(argv[i], "--layout=", 9)) {
      string argi  = tolower(argv[i]);
      string front = biteString(argi, '=');
      string value = argi;
      
      if(value == "noipfs") 
	m_window_layout = "noipfs";
      else if(value == "fullview") 
	m_window_layout = "fullview";
      else
	cout << "Unrecognized layout (" << value << ") ignored." << endl;
    }
  }
}

//-------------------------------------------------------------
// Procedure: setALogFiles
//            Find all .alog files on the commmand line

void LogViewLauncher::setALogFiles(int argc, char **argv)
{
  for(int i=1; i<argc; i++)
    if(strContains(argv[i], ".alog"))
      m_alog_files.push_back(argv[i]);
}


//-------------------------------------------------------------
// Procedure: setALogFileSkews

bool LogViewLauncher::setALogFileSkews()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Detecting alog file skews..." << endl;

  unsigned int i, j, vsize = m_alog_files.size();
  vector<double> logstarts(vsize,0);

  double min_logstart = 0;
  for(i=0; i<vsize; i++) {
    string filestr = m_alog_files[i];
    FILE *f = fopen(filestr.c_str(), "r");
    if(!f) {
      cout << termColor("red");
      cout << "Unable to open: " << filestr << termColor() << endl;
      return(false);
    }
    for(j=0; j<5; j++) {
      string line = getNextRawLine(f);
      if(strContains(line, "LOGSTART")) {
	logstarts[i] = getLogStart(line);
      }
    }
    fclose(f);
    if(logstarts[i] == 0) {
      cout << termColor("red");
      cout << "Unable to detect LOGSTART in file: " << filestr << endl;
      cout << termColor();
      return(false);
    }
    if((i==0) || (logstarts[i] < min_logstart))
      min_logstart = logstarts[i];
  }

  // Apply min_logstart to all so the earliest has skew of zero
  for(i=0; i<m_alog_files.size(); i++)
    m_alog_files_skew.push_back(logstarts[i] - min_logstart);

  parse_timer.stop();
  cout << termColor("green");
  cout << "Done detecting .alog file SKEWS - total detect time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();
  return(true);
}
  
//-------------------------------------------------------------
// Procedure: parseALogFiles
//            Parse all .alog files on the commmand line

void LogViewLauncher::parseALogFiles()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Parsing alog files..." << endl;

  unsigned int i, vsize = m_alog_files.size();
  for(i=0; i<vsize; i++) {
    cout << "  Handling " << m_alog_files[i] << "...";
    cout << flush;
    parseALogFile(i);
    cout << "DONE" << endl;
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

void LogViewLauncher::parseALogFile(unsigned int index)
{
  unsigned int vsize = m_alog_files.size();
  if(index >= vsize)
    return;

  string filestr = m_alog_files[index];
  FILE *f = fopen(filestr.c_str(), "r");
  if(!f)
    return;

  vector<string>    node_reports;
  vector<ALogEntry> entries_log_plot;
  vector<ALogEntry> entries_ipf_plot;
  vector<ALogEntry> entries_vplug_plot;
  vector<ALogEntry> entries_helm_plot;

  double skew = m_alog_files_skew[index];

  bool done = false;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(f);
    if(entry.getStatus() == "eof")
      done = true;
    else {
      string var = entry.getVarName();
      string src = entry.getSource();
      bool   isnum = entry.isNumerical();

      entry.skewBackward(skew);
      double tstamp = entry.getTimeStamp();
      if((!m_min_time_set || (tstamp >= m_min_time)) &&
	 (!m_max_time_set || (tstamp <= m_max_time))) {
	if(isnum)
	  entries_log_plot.push_back(entry);
	else {
	  if((var=="AIS_REPORT_LOCAL") && (src=="pTransponderAIS"))
	    node_reports.push_back(entry.getStringVal());
	  else if((var=="NODE_REPORT_LOCAL") && (src=="pNodeReporter"))
	    node_reports.push_back(entry.getStringVal());
	  else if((var == "VIEW_POINT")   || (var == "VIEW_POLYGON") ||
		  (var == "VIEW_SEGLIST") || (var == "VIEW_CIRCLE")  ||
		  (var == "GRID_INIT")    || (var == "VIEW_MARKER")  ||
		  (var == "GRID_DELTA")) {
	    entries_vplug_plot.push_back(entry);
	  }
	  else if(var == "IVPHELM_SUMMARY")
	    entries_helm_plot.push_back(entry);
	  else if(var == "BHV_IPF")
	    entries_ipf_plot.push_back(entry);
	}
      }
    }
  }
  fclose(f);

  m_node_reports.push_back(node_reports);
  m_entries_log_plot.push_back(entries_log_plot);
  m_entries_ipf_plot.push_back(entries_ipf_plot);
  m_entries_vplug_plot.push_back(entries_vplug_plot);
  m_entries_helm_plot.push_back(entries_helm_plot);
}
  
//-------------------------------------------------------------
// Procedure: determineVehicleNames
//   Purpose: To determine, for each alog file, i.e., vehicle, the
//            vehicle name, type and length from contents of either
//            the "NODE_REPORT_LOCAL" or older "AIS_REPORT_LOCAL"
//            variable entries. 
//            The member variables m_vehicle_name, _type, _length
//            will be filled in. 

void LogViewLauncher::determineVehicleNames()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Determining vehicle names from ALog Files..." << endl;

  unsigned int j, jsize = m_alog_files.size();
  for(j=0; j<jsize; j++) {
    string vname = "V" + intToString(j);
    string vtype = "auv";
    string vlength = "10";
    bool   vname_set = false;
    bool   vtype_set = false;
    bool   vlength_set = false;
    bool   vname_inconsistent = false;
    bool   vtype_inconsistent = false;
    bool   vlength_inconsistent = false;

    if(j < (m_node_reports.size())) {
      unsigned int k, ksize = m_node_reports[j].size();
      
      if(ksize == 0) {
	cout << termColor("red");
	cout << "Warning: No NODE_REPORTS in " << m_alog_files[j] << "!!!";
	cout << termColor() << endl;
      }

      for(k=0; k<ksize; k++) {
	vector<string> svector = parseString(m_node_reports[j][k],',');
	unsigned int i, vsize = svector.size();
	for(i=0; i<vsize; i++) {
	  string left  = stripBlankEnds(tolower(biteString(svector[i],'=')));
	  string right = stripBlankEnds(svector[i]);

	  if(left == "name") {
	    if(!vname_set || (vname == right)) {
	      vname = right;
	      vname_set = true;
	    }
	    else
	      vname_inconsistent = true;
	  }
	  else if(left == "type") {
	    if(!vtype_set || (vtype == right)) {
	      vtype = right;
	      vtype_set = true;
	    }
	    else
	      vtype_inconsistent = true;
	  }
	  else if(left == "length") {
	    if(!vlength_set || (vlength == right)) {
	      vlength = right;
	      vlength_set = true;
	    }
	    else
	      vlength_inconsistent = true;
	  }
	}
      }
    }
    m_vehicle_name.push_back(vname);
    m_vehicle_type.push_back(vtype);
    m_vehicle_length.push_back(atof(vlength.c_str()));
    
    cout << termColor("red");
    if(vname_inconsistent) {
      cout << "Warning: Vehicle name for file " << m_alog_files[j];
      cout << " was inconsistent across node reports." << endl;
    }
    if(vtype_inconsistent) {
      cout << "Warning: Vehicle type for file " << m_alog_files[j];
      cout << " was inconsistent across node reports." << endl;
    }
    if(vlength_inconsistent) {
      cout << "Warning: Vehicle length for file " << m_alog_files[j];
      cout << " was inconsistent across node reports." << endl;
    }
    cout << termColor();

    cout << "  File " << m_alog_files[j] << " name=" << vname << endl;
    cout << "  File " << m_alog_files[j] << " type=" << vtype << endl;
    cout << "  File " << m_alog_files[j] << " len=" << vlength << endl;
  }

  parse_timer.stop();
  cout << termColor("green");
  cout << "Done setting vehicle names - total parse time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();

}

//-------------------------------------------------------------
// Procedure: buildLogPlots
//            

bool LogViewLauncher::buildLogPlots()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Refining alog data to build LogPlots..." << endl;

  unsigned int i, vsize = m_alog_files.size();

  for(i=0; i<vsize; i++) {
    Populator_LogPlots pop_lp;
    pop_lp.setVName(m_vehicle_name[i]); 
    pop_lp.populateFromEntries(m_entries_log_plot[i]);
    
    vector<LogPlot> lp_vector;
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
  }

  parse_timer.stop();
  cout << termColor("green");
  cout << "Done build LogPlots - total build time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();
  return(true);
}

//-------------------------------------------------------------
// Procedure: buildHelmPlots
//            

bool LogViewLauncher::buildHelmPlots()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Refining alog data to build HelmPlots..." << endl;

  unsigned int i, vsize = m_alog_files.size();
  for(i=0; i<vsize; i++) {
    Populator_HelmPlots pop_hp;
    bool ok = pop_hp.populateFromEntries(m_entries_helm_plot[i]);
    if(!ok) {
      cout << termColor("red") << "Unable to build Helm Plots!!!" << endl;
      cout << "(Likely due to lack of IVPHELM_* entries in .alog files)";
      cout << termColor() << endl << endl;
      return(false);
    }
    m_helm_plots.push_back(pop_hp.getHelmPlot());
  }

  for(i=0; i<vsize; i++) {
    m_helm_plots[i].set_vehi_name(m_vehicle_name[i]);
    m_helm_plots[i].set_vehi_type(m_vehicle_type[i]);
    m_helm_plots[i].set_vehi_length(m_vehicle_length[i]);
  }

  parse_timer.stop();
  cout << "  Total HelmPlots: " << m_helm_plots.size() << endl;

  cout << termColor("green");
  cout << "Done building HelmPlots - total build time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();
  return(true);
}

//-------------------------------------------------------------
// Procedure: buildVPlugPlots

bool LogViewLauncher::buildVPlugPlots()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Refining alog data to build VPlugPlots..." << endl;

  unsigned int i, vsize = m_alog_files.size();
  for(i=0; i<vsize; i++) {
    Populator_VPlugPlots pop_vp;
    bool ok = pop_vp.populateFromEntries(m_entries_vplug_plot[i]);
    if(!ok) {
      cout << "Problem with file " << m_alog_files[i] << ". Exiting" << endl;
      return(false);
    }
    m_vplug_plots.push_back(pop_vp.getVPlugPlot());
  }

  parse_timer.stop();
  cout << termColor("green");
  cout << "Done: VPlugPlot parse time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();
  return(true);
}


//-------------------------------------------------------------
// Procedure: buildIPFPlots

bool LogViewLauncher::buildIPFPlots()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Refining alog data to build IPF_Plots..." << endl;

  unsigned int i, vsize = m_alog_files.size();
  for(i=0; i<vsize; i++) {
    Populator_IPF_Plot pop_ipf;
    pop_ipf.setVName(m_vehicle_name[i]);
    bool ok = pop_ipf.populateFromEntries(m_entries_ipf_plot[i]);
    if(!ok) {
      cout << "  Problem with file " << m_alog_files[i] << endl;
      return(false);
    }
    unsigned int k, ksize = pop_ipf.size();
    for(k=0; k<ksize; k++)
      m_ipf_plots.push_back(pop_ipf.getPlotIPF(k));
  }
  parse_timer.stop();

  
  if(m_ipf_plots.size() == 0) {
    cout << termColor("red") << "Unable to build IPF Plots!!!" << endl;
    cout << "(Likely due to lack of BHV_IPF entries in .alog files)";
    cout << endl << termColor() << endl;
    return(false);
  }

  cout << termColor("green");
  cout << "Done: IPF_Plot parse time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  cout << termColor();
  return(true);
}


//-------------------------------------------------------------
// Procedure: buildGraphical

bool LogViewLauncher::buildGraphical()
{
  m_gui = new REPLAY_GUI(m_gui_width, m_gui_height, "logview");
  if(!m_gui)
    return(false);

  m_gui->setWindowLayout(m_window_layout);

  unsigned int j, k;

  // Populate the GUI with the LogPlots built above
  for(k=0; k<m_log_plots.size(); k++) {
    for(j=0; j<m_log_plots[k].size(); j++) {
      m_gui->addLogPlot(m_log_plots[k][j]);
      if(m_log_plots[k][j].get_varname() == "NAV_X")
	m_gui->np_viewer->addLogPlotNAVX(m_log_plots[k][j]);
      else if(m_log_plots[k][j].get_varname() == "NAV_Y")
	m_gui->np_viewer->addLogPlotNAVY(m_log_plots[k][j]);
      else if(m_log_plots[k][j].get_varname() == "NAV_HEADING")
	m_gui->np_viewer->addLogPlotHDG(m_log_plots[k][j]);
    }
  }
  
  // Populate the GUI with the priot-built HelmPlots
  for(k=0; k<m_helm_plots.size(); k++) 
    m_gui->addHelmPlot(m_helm_plots[k]);

  // Populate the GUI with the prior-built VPlugPlots
  for(k=0; k<m_vplug_plots.size(); k++) 
    m_gui->np_viewer->addVPlugPlot(m_vplug_plots[k]);

  // Populate the GUI with the prior-built IPF_Plots
  for(k=0; k<m_ipf_plots.size(); k++)
    m_gui->addIPF_Plot(m_ipf_plots[k]);

#if 0
  // Populate the GUI with the polygons built above
  for(j=0; j<polygons.size(); j++)
    m_gui->np_viewer->setParam("poly", polygons[j]);
#endif

  m_gui->updateXY();
  m_gui->np_viewer->setParam("tiff_file", m_tif_file);

  if(m_now_time_set)
    m_gui->setCurrTime(m_now_time);


  return(true);
}
