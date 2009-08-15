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
}

//-------------------------------------------------------------
// Procedure: setLaunch

REPLAY_GUI *LogViewLauncher::launch(int argc, char **argv)
{
  MBTimer total_timer;
  total_timer.start();

  setBackground(argc, argv);
  setSizeOfGUI(argc, argv);
  setALogFiles(argc, argv);

  bool ok = setALogFileSkews();
  parseALogFiles();
  
  ok = ok && buildLogPlots();
  ok = ok && buildHelmPlots();
  ok = ok && buildVPlugPlots();
  ok = ok && buildGraphical();

  total_timer.stop();
  cout << "Done logview launch time (cpu): ";
  cout << total_timer.get_float_cpu_time() << endl;
  cout << "Done logview launch time (wall): ";
  cout << total_timer.get_float_wall_time() << endl;

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

void LogViewLauncher::setBackground(int argc, char **argv)
{
  m_tif_file = "Default.tif";
  for(int i=1; i<argc; i++) {
    if(!strncmp(argv[i], "--background=", 13)) {
      string argi  = tolower(argv[i]);
      string front = biteString(argi, '=');
      string value = tolower(argi);
      
      if((value == "mit") || (value=="charles"))
	m_tif_file = "AerialMIT-1024.tif";
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
    }
  }
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
      string value = tolower(argi);
      
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
      cout << "Unable to open: " << filestr << endl;
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
      cout << "Unable to detect LOGSTART in file: " << filestr << endl;
      return(false);
    }
    if((i==0) || (logstarts[i] < min_logstart))
      min_logstart = logstarts[i];
  }

  // Apply min_logstart to all so the earliest has skew of zero
  for(i=0; i<m_alog_files.size(); i++)
    m_alog_files_skew.push_back(logstarts[i] - min_logstart);

  parse_timer.stop();
  cout << "Done detecting .alog file SKEWS - total detect time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
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
  cout << "Done parsing .alog files - total parse time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
}
  
//-------------------------------------------------------------
// Procedure: parseALogFile
//            Parse the .alog file given by the index

void LogViewLauncher::parseALogFile(unsigned int index)
{
  unsigned int i, vsize = m_alog_files.size();
  if(index >= vsize)
    return;

  string filestr = m_alog_files[index];
  FILE *f = fopen(filestr.c_str(), "r");
  if(!f)
    return;

  vector<ALogEntry> entries_log_plot;
  vector<ALogEntry> entries_bhv_ipf;
  vector<ALogEntry> entries_vplug_plot;
  vector<ALogEntry> entries_helm_plot;

  double skew = m_alog_files_skew[index];

  bool done = false;
  int count = 0;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(f);
    if(entry.getStatus() == "eof")
      done = true;
    else {
      string var = entry.getVarName();
      bool   isnum = entry.isNumerical();

      entry.skewBackward(skew);
      if(isnum)
	entries_log_plot.push_back(entry);
      else {
	if((var == "AIS_REPORT_LOCAL") || (var == "NODE_REPORT_LOCAL")) {
	  entries_log_plot.push_back(entry);
	  entries_helm_plot.push_back(entry);
	}
	else if((var == "VIEW_POINT")   || (var == "VIEW_POLYGON") ||
		(var == "VIEW_SEGLIST") || (var == "VIEW_CIRCLE")  ||
		(var == "GRID_INIT")    || (var == "VIEW_MARKER")  ||
		(var == "GRID_DELTA")) {
	  entries_vplug_plot.push_back(entry);
	}
	else if(var == "IVPHELM_SUMMARY")
	  entries_helm_plot.push_back(entry);
	else if(var == "BHV_IPF")
	  entries_bhv_ipf.push_back(entry);
      }
    }
  }
  fclose(f);
  
  m_entries_log_plot.push_back(entries_log_plot);
  m_entries_bhv_ipf.push_back(entries_bhv_ipf);
  m_entries_vplug_plot.push_back(entries_vplug_plot);
  m_entries_helm_plot.push_back(entries_helm_plot);
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
    pop_lp.setDefaultVName("V_" + intToString(i)); 
    bool ok = pop_lp.populateFromEntries(m_entries_log_plot[i]);
    if(!ok) {
      cout << "Problem with file " << m_alog_files[i] << ". Exiting" << endl;
      return(false);
    }
    
    vector<LogPlot> lp_vector;
    unsigned int k, lp_size = pop_lp.size();
    for(k=0; k<lp_size; k++)
      lp_vector.push_back(pop_lp.getLogPlot(k));

    m_log_plots.push_back(lp_vector);
  }

  parse_timer.stop();
  cout << "Done build LogPlots - total build time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
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
      cout << "Problem with file " << m_alog_files[i] << ". Exiting" << endl;
      return(false);
    }
    m_helm_plots.push_back(pop_hp.getHelmPlot());
  }

  parse_timer.stop();
  cout << "  Total HelmPlots: " << m_helm_plots.size() << endl;

  cout << "Done building HelmPlots - total build time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
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
  cout << "Done: VPlugPlot parse time: ";
  cout << parse_timer.get_float_cpu_time() << endl << endl;
  return(true);
}


//-------------------------------------------------------------
// Procedure: buildGraphical

bool LogViewLauncher::buildGraphical()
{
  m_gui = new REPLAY_GUI(m_gui_width, m_gui_height, "logview");
  if(!m_gui)
    return(false);

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
  
  // Populate the GUI with the HelmPlots built above
  for(k=0; k<m_helm_plots.size(); k++) 
    m_gui->addHelmPlot(m_helm_plots[k]);

  // Populate the GUI with the VPlugPlots built above
  for(k=0; k<m_vplug_plots.size(); k++) 
    m_gui->np_viewer->addVPlugPlot(m_vplug_plots[k]);

#if 0
  // Populate the GUI with the polygons built above
  for(j=0; j<polygons.size(); j++)
    m_gui->np_viewer->setParam("poly", polygons[j]);
#endif

  m_gui->updateXY();
  m_gui->np_viewer->setParam("tiff_file", m_tif_file);

  return(true);
}
