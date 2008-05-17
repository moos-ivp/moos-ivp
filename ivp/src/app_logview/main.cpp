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
#include "Populator_LogPlots.h"
#include "Populator_GridPlot.h"

using namespace std;

void help_message();

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
  int i, j, res = 0;

  string gui_size = "large";

  // Look for a request for help
  res = getArg(argc, argv, 0, "-h", "--help");
  if(res) {
    help_message();
    return(0);
  }

  // Find the tif (texture background) file if provided
  //---------------------------------------------------------------------
  string tif_file = "Default.tif";
  for(i=1; i<argc; i++) {
    string argi  = tolower(argv[i]);
    if((argi == "mit") || (argi=="charles"))
      tif_file = "AerialMIT-1024.tif";
    if((argi == "wmit") || (argi=="wireframe") || (argi=="wf"))
      tif_file = "WireFrameMIT-1024.tif";
    if((argi == "mb") || (argi=="monterey"))
      tif_file = "Monterey-2048.tif";
    if((argi == "mbd"))
      tif_file = "Monterey-2048-30-30-100.tif";
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
    if(arg=="large")  gui_size = "large";
    if(arg=="medium") gui_size = "medium";
    if(arg=="small")  gui_size = "small";
    if(arg=="xsmall") gui_size = "xsmall";
  }

  // Find all the slog files provided on the command line
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
  
  // Read in the skews from each slog file
  //---------------------------------------------------------------------
  double min_skew = 0;
  for(i=0; i<alog_files.size(); i++) {
    vector<string> svector = fileBuffer(alog_files[i], 5);
    int vsize = svector.size();
    for(int j=0; j<vsize; j++) {
      string line = compactConsecutive(svector[j], ' ');
      vector<string> evector = parseString(line, ' ');
      if(evector.size() >= 3)
	if(evector[1] == "LOGSTART") {
	  double skew = atof(evector[2].c_str());
	  alog_files_skew[i] = skew;
	  if((i==0) || (skew<min_skew))
	    min_skew = skew;
	}
    }
  }
  
  // Apply min_skew to all so the earliest has skew of zero
  //---------------------------------------------------------------------
  for(i=0; i<alog_files_skew.size(); i++)
    alog_files_skew[i] -= min_skew;
  
  // Build all the gridplots from the vector of alog files.
  //---------------------------------------------------------------------
  vector<GridPlot> gridplots;

  MBTimer parse_timer;
  parse_timer.start();
  cout << "Parsing alog files to build GridPlots..." << endl;
  
  for(i=0; i<alog_files.size(); i++) {
    Populator_GridPlot pop_gp;
    pop_gp.populate(alog_files[i]);
    int psize = pop_gp.size();
    cout << "Built " << psize << " GridPlots from " << alog_files[i] << endl;
    for(int j=0; j<psize; j++) 
      gridplots.push_back(pop_gp.getGridPlot(j));
  }

  parse_timer.stop();
  cout << "Done: GridPlot parse time: " << parse_timer.get_float_cpu_time();
  cout << endl << endl;


  // Build all the logplots from the vector of slog files.
  //---------------------------------------------------------------------
  vector<vector<LogPlot> > logplots;

  parse_timer.reset();
  parse_timer.start();
  cout << "Parsing slog files to build LogPlots..." << endl;

  for(i=0; i<alog_files.size(); i++) {
    Populator_LogPlots pop_lp;
    //pop_lp.setSkew(alog_files_skew[i]);
    pop_lp.setVName("V_" + intToString(i)); 
    bool ok = pop_lp.setFileALog(alog_files[i]);
    if(!ok) {
      cout << "Problem with file " << alog_files[i] << ". Exiting" << endl;
      exit(0);
    }
    
    pop_lp.populateFromALog();
    
    vector<LogPlot> lvector;
    int lsize = pop_lp.size();
    for(int i=0; i<lsize; i++)
      lvector.push_back(pop_lp.getLogPlot(i));

    logplots.push_back(lvector);
  }

  parse_timer.stop();
  cout << "Done: LogPlot parse time: " << parse_timer.get_float_cpu_time();
  cout << endl << endl;


  // Build all the Polygons and Grids from the vector of non-slog files.
  //---------------------------------------------------------------------
  vector<XYPolygon> polygons;
  vector<XYGrid>    searchgrids;

  for(i=0; i<non_log_files.size(); i++) {
    vector<XYPolygon> pvector = readPolysFromFile(non_log_files[i]);
    vector<XYGrid>    qvector = readGridsFromFile(non_log_files[i]);
    for(j=0; j<pvector.size(); j++)
      polygons.push_back(pvector[j]);
    for(j=0; j<qvector.size(); j++)
      searchgrids.push_back(qvector[j]);
  }
  
  // If we've gotten this far without errors, go ahead and create the GUI
  // to prepare for populating with data structures.
  //---------------------------------------------------------------------

  Fl::add_idle(idleProc);
  if(gui_size=="large")
    gui = new REPLAY_GUI(1400, 1100, "OpRegion-Viewer");
  if(gui_size=="medium")
    gui = new REPLAY_GUI(1190, 935, "OpRegion-Viewer");
  if(gui_size=="small")
    gui = new REPLAY_GUI(980, 770, "OpRegion-Viewer");
  if(gui_size=="xsmall")
    gui = new REPLAY_GUI(770, 605, "OpRegion-Viewer");

  // Populate the GUI with the GridPlots built above
  for(i=0; i<gridplots.size(); i++)
    gui->addGridPlot(gridplots[i]);
  
  // Populate the GUI with the LogPlots built above
  for(i=0; i<logplots.size(); i++) {
    for(int j=0; j<logplots[i].size(); j++) {
      gui->addLogPlot(logplots[i][j]);
      if(logplots[i][j].get_varname() == "NAV_X")
	gui->np_viewer->addLogPlotNAVX(logplots[i][j]);
      else if(logplots[i][j].get_varname() == "NAV_Y")
	gui->np_viewer->addLogPlotNAVY(logplots[i][j]);
      else if(logplots[i][j].get_varname() == "NAV_HEADING")
	gui->np_viewer->addLogPlotHDG(logplots[i][j]);
    }
  }

  // Populate the GUI with the polygons built above
  for(i=0; i<polygons.size(); i++)
    gui->addPoly(polygons[i]);

  // Populate the GUI with the search grids build above
  for(i=0; i<searchgrids.size(); i++)
    gui->addGrid(searchgrids[i]);

  gui->updateXY();
  gui->readTiff(tif_file);

  // Enter the GUI event loop.
  return Fl::run();
}


//--------------------------------------------------------
// Procedure: help_message()

void help_message()
{
  cout << endl;
  cout << "Usage: logview file1.slog [file2.slog] [file3] [-h] [--help] " << endl;
  cout << "   At least one .slog file must be provided " << endl;
  cout << "   Non slog files will be scanned for polygons " << endl;
  cout << endl;
}








