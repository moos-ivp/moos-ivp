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
#include "REPLAY_GUI.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "MBTimer.h"
#include "IO_GeomUtils.h"
#include "XYPolygon.h"
#include "LogPlot.h"
#include "NavPlot.h"
#include "Populator_LogPlots.h"
#include "Populator_NavPlot.h"

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
  vector<string> slog_files;
  vector<double> slog_files_skew;
  vector<string> non_slog_files;

  for(i=1; i<argc; i++) {
    if(strContains(argv[i], ".slog")) {
      slog_files.push_back(argv[i]);
      slog_files_skew.push_back(0);
    }
    else
      non_slog_files.push_back(argv[i]);
  }
  if((argc <= 1) || (slog_files.size() == 0)) {
    cout << "Must provide an .slog file" << endl;
    return(0);
  }
  
  // Read in the skews from each slog file
  //---------------------------------------------------------------------
  double min_skew = 0;
  for(i=0; i<slog_files.size(); i++) {
    vector<string> svector = fileBuffer(slog_files[i], 5);
    int vsize = svector.size();
    for(int j=0; j<vsize; j++) {
      string line = compactConsecutive(svector[j], ' ');
      vector<string> evector = parseString(line, ' ');
      if(evector.size() >= 3)
	if(evector[1] == "LOGSTART") {
	  double skew = atof(evector[2].c_str());
	  slog_files_skew[i] = skew;
	  if((i==0) || (skew<min_skew))
	    min_skew = skew;
	}
    }
  }
  
  // Apply min_skew to all so the earliest has skew of zero
  //---------------------------------------------------------------------
  for(i=0; i<slog_files_skew.size(); i++)
    slog_files_skew[i] -= min_skew;
  

  // Build all the navplots from the vector of slog files.
  //---------------------------------------------------------------------
  vector<NavPlot> navplots;

  MBTimer parse_timer;
  parse_timer.start();
  cout << "Parsing slog files to build NavPlots..." << endl;

  for(i=0; i<slog_files.size(); i++) {
    Populator_NavPlot pop_np;
    pop_np.set_skew(slog_files_skew[i]);
    bool ok = pop_np.set_file(slog_files[i]);
    if(!ok) {
      cout << "Problem with file " << slog_files[0] << ". Exiting" << endl;
      exit(0);
    }
    ok = pop_np.populate();
    if(!ok) {
      cout << "Could not determine X,Y trajectory in file " << slog_files[i];
      cout << ". Exiting now." << endl;
      exit(0);
    }
    NavPlot navplot = pop_np.get_navplot();
    navplots.push_back(navplot);
  }

  parse_timer.stop();
  cout << "Done: NavPlot parse time: " << parse_timer.get_float_cpu_time();
  cout << endl << endl;


  // Build all the logplots from the vector of slog files.
  //---------------------------------------------------------------------
  vector<vector<LogPlot> > logplots;

  parse_timer.reset();
  parse_timer.start();
  cout << "Parsing slog files to build LogPlots..." << endl;

  for(i=0; i<slog_files.size(); i++) {
    Populator_LogPlots pop_lp;
    pop_lp.set_skew(slog_files_skew[i]);
    pop_lp.set_vname("V_" + intToString(i)); 
    bool ok = pop_lp.set_file(slog_files[i]);
    if(!ok) {
      cout << "Problem with file " << slog_files[i] << ". Exiting" << endl;
      exit(0);
    }
    
    pop_lp.populate();
    
    vector<LogPlot> lvector;
    int lsize = pop_lp.get_size();
    for(int i=0; i<lsize; i++)
      lvector.push_back(pop_lp.get_logplot(i));

    logplots.push_back(lvector);
  }

  parse_timer.stop();
  cout << "Done: LogPlot parse time: " << parse_timer.get_float_cpu_time();
  cout << endl << endl;


  // Build all the Polygons and Grids from the vector of non-slog files.
  //---------------------------------------------------------------------
  vector<XYPolygon> polygons;
  vector<XYGrid>    searchgrids;

  for(i=0; i<non_slog_files.size(); i++) {
    vector<XYPolygon> pvector = readPolysFromFile(non_slog_files[i]);
    vector<XYGrid>    qvector = readGridsFromFile(non_slog_files[i]);
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

  gui->setLogFile(slog_files[0]);

  // Populate the GUI with the NavPlots built above
  for(i=0; i<navplots.size(); i++)
    gui->addNavPlot(navplots[i]);

  // Populate the GUI with the LogPlots built above
  for(i=0; i<logplots.size(); i++)
    for(int j=0; j<logplots[i].size(); j++)
      gui->addLogPlot(logplots[i][j]);
  
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








