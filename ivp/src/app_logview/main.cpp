/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: May 1st, 2005                                        */
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

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "REPLAY_GUI.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "MBTimer.h"
#include "IO_GeomUtils.h"
#include "XYPolygon.h"
#include "LogPlot.h"
#include "HelmPlot.h"
#include "Populator_HelmPlots.h"
#include "Populator_LogPlots.h"
#include "LMV_Utils.h"

using namespace std;

void help_message();
void version_message();

REPLAY_GUI* gui = 0;

//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  if(gui) gui->conditional_step();
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  int i, res = 0;
  unsigned int j, k;

  string gui_size = "large";

  // Look for a request for help
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    help_message();
    return(0);
  }

  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    version_message();
    return(0);
  }

  // Find the tif (texture background) file if provided
  //---------------------------------------------------------------------
  string tif_file = "Default.tif";
  for(i=1; i<argc; i++) {
    string argi  = tolower(argv[i]);
    if((argi == "mit") || (argi=="charles"))
      tif_file = "AerialMIT-1024.tif";
    else if((argi == "wmit") || (argi=="wireframe") || (argi=="wf"))
      tif_file = "WireFrameMIT-1024.tif";
    else if((argi == "mb") || (argi=="monterey"))
      tif_file = "Monterey-2048.tif";
    else if((argi == "mbd"))
      tif_file = "Monterey-2048-30-30-100.tif";
    else if((argi == "-fl") || (argi == "-forrest"))
      tif_file = "forrest19.tif";
    else if((argi == "glint") || (argi == "-glint"))
      tif_file = "glintA.tif";
  }

  for(i=1; i<argc; i++)
    if(strContains(argv[i], ".tif"))
      tif_file = argv[i];
  for(i=1; i<argc; i++)
    if(!strcmp(argv[i], "-noimg"))
      tif_file = "";

  // Determine the GUI size
  //---------------------------------------------------------------------
  for(i=1; i<argc; i++) {
    string arg = argv[i];
    if(arg=="--large")  gui_size = "large";
    if(arg=="--medium") gui_size = "medium";
    if(arg=="--small")  gui_size = "small";
    if(arg=="--xsmall") gui_size = "xsmall";
  }

  // Find all the alog files provided on the command line
  //---------------------------------------------------------------------
  vector<string> alog_files;
  vector<double> alog_files_skew;
  vector<string> non_log_files;

  for(i=1; i<argc; i++) {
    if(strContains(argv[i], ".alog")) {
      alog_files.push_back(argv[i]);
      alog_files_skew.push_back(0);
    }
    else
      non_log_files.push_back(argv[i]);
  }
  if((argc <= 1) || (alog_files.size() == 0)) {
    cout << "Must provide an .alog file" << endl;
    return(0);
  }
  
  // Read in the skews from each alog file
  //---------------------------------------------------------------------
  double min_skew = 0;
  for(k=0; k<alog_files.size(); k++) {
    vector<string> svector = fileBuffer(alog_files[k], 5);
    unsigned int vsize = svector.size();
    for(j=0; j<vsize; j++) {
      string line = compactConsecutive(svector[j], ' ');
      vector<string> evector = parseString(line, ' ');
      if(evector.size() >= 3)
	if(evector[1] == "LOGSTART") {
	  double skew = atof(evector[2].c_str());
	  alog_files_skew[k] = skew;
	  if((k==0) || (skew<min_skew))
	    min_skew = skew;
	}
    }
  }
  
  // Apply min_skew to all so the earliest has skew of zero
  //---------------------------------------------------------------------
  for(j=0; j<alog_files_skew.size(); j++)
    alog_files_skew[j] -= min_skew;
  
  // Build all the logplots from the vector of alog files.
  //---------------------------------------------------------------------
  vector<vector<LogPlot> > logplots;

  MBTimer parse_timer;
  parse_timer.start();
  cout << "Parsing alog files to build LogPlots..." << endl;

  for(j=0; j<alog_files.size(); j++) {
    Populator_LogPlots pop_lp;
    //pop_lp.setSkew(alog_files_skew[j]);
    pop_lp.setDefaultVName("V_" + intToString(j)); 
    bool ok = pop_lp.setFileALog(alog_files[j]);
    if(!ok) {
      cout << "Problem with file " << alog_files[j] << ". Exiting" << endl;
      exit(0);
    }
    
    pop_lp.populateFromALog();
    
    vector<LogPlot> lvector;
    unsigned int lsize = pop_lp.size();
    for(k=0; k<lsize; k++)
      lvector.push_back(pop_lp.getLogPlot(k));

    logplots.push_back(lvector);
  }

  parse_timer.stop();
  cout << "Done: LogPlot parse time: " << parse_timer.get_float_cpu_time();
  cout << endl << endl;

  // Build all the HelmPlots from the vector of alog files.
  //---------------------------------------------------------------------
  vector<HelmPlot> helm_plots;

  MBTimer parse_timer_hp;
  parse_timer_hp.reset();
  parse_timer_hp.start();
  cout << "Parsing alog files to build HelmPlots..." << endl;

  for(j=0; j<alog_files.size(); j++) {
    Populator_HelmPlots pop_hp;
    bool ok = pop_hp.setFileALog(alog_files[j]);
    if(!ok) {
      cout << "Problem making HelmPlot from file " << alog_files[j] 
	   << ". Exiting" << endl;
      exit(0);
    }
    
    pop_hp.populateFromALog();

    HelmPlot hplot = pop_hp.getHelmPlot();
    //hplot.print();
    
    helm_plots.push_back(hplot);
  }

  parse_timer_hp.stop();
  cout << "Done: HelmPlot parse time: " << parse_timer_hp.get_float_cpu_time();
  cout << endl << endl;


  // Build all the Polygons and Grids from the vector of non-alog files.
  //---------------------------------------------------------------------
  vector<string>  polygons;

  for(j=0; j<non_log_files.size(); j++) {
    vector<string> svector;
    svector = readEntriesFromFile(non_log_files[j], "poly:polygon");
    for(k=0; k<svector.size(); k++)
      polygons.push_back(svector[k]);
  }
  
  // If we've gotten this far without errors, go ahead and create the GUI
  // to prepare for populating with data structures.
  //---------------------------------------------------------------------

  Fl::add_idle(idleProc);
  if(gui_size=="large")
    gui = new REPLAY_GUI(1400, 1100, "logview");
  if(gui_size=="medium")
    gui = new REPLAY_GUI(1190, 935, "logview");
  if(gui_size=="small")
    gui = new REPLAY_GUI(980, 770, "logview");
  if(gui_size=="xsmall")
    gui = new REPLAY_GUI(770, 605, "logview");

  // Populate the GUI with the LogPlots built above
  for(k=0; k<logplots.size(); k++) {
    for(j=0; j<logplots[k].size(); j++) {
      gui->addLogPlot(logplots[k][j]);
      if(logplots[k][j].get_varname() == "NAV_X")
	gui->np_viewer->addLogPlotNAVX(logplots[k][j]);
      else if(logplots[k][j].get_varname() == "NAV_Y")
	gui->np_viewer->addLogPlotNAVY(logplots[k][j]);
      else if(logplots[k][j].get_varname() == "NAV_HEADING")
	gui->np_viewer->addLogPlotHDG(logplots[k][j]);
    }
  }

  // Populate the GUI with the HelmPlots built above
  for(k=0; k<helm_plots.size(); k++) 
    gui->np_viewer->addHelmPlot(helm_plots[k]);

  // Populate the GUI with the polygons built above
  for(j=0; j<polygons.size(); j++)
    gui->np_viewer->setParam("poly", polygons[j]);

  gui->updateXY();
  gui->np_viewer->setParam("tiff_file", tif_file);

  // Enter the GUI event loop.
  return Fl::run();
}


//--------------------------------------------------------
// Procedure: help_message()

void help_message()
{
  cout << endl;
  cout << "Usage: logview file1.alog [file2.alog] [-h] [--help] " << endl;
  cout << "   At least one .alog file must be provided " << endl;
  cout << "   Non alog files will be scanned for polygons " << endl;
  cout << endl;
}

//--------------------------------------------------------
// Procedure: version_message()

void version_message()
{
  vector<string> svector = getReleaseInfo("logview");
  for(int j=0; j<svector.size(); j++)
    cout << svector[j] << endl;
}








