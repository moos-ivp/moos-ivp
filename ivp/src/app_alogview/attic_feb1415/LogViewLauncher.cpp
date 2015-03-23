/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogViewLauncher.cpp                                  */
/*    DATE: May 31st, 2005                                       */
/*    DATE: Feb 9th, 2015 Major overhaul mikerb                  */
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
#include "MBUtils.h"
#include "MBTimer.h"
#include "LogViewLauncher.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

LogViewLauncher::LogViewLauncher()
{
  m_tiff_file  = "";
  m_gui_width  = 1200;
  m_gui_height = 800;
  m_gui        = 0;
}

//-------------------------------------------------------------
// Procedure: launch

REPLAY_GUI *LogViewLauncher::launch(int argc, char **argv)
{
  MBTimer total_timer;
  total_timer.start();

  setBackground(argc, argv);
  setSizeOfGUI(argc, argv);
  setALogFiles(argc, argv);  

  cout << "Begin Checking alog file(s)------------------" << endl;
  m_dbroker.checkALogFiles();
  cout << "Begin Spliting alog file(s)------------------" << endl;
  m_dbroker.splitALogFiles();
  cout << "Begin TimeSetting alog file(s)---------------" << endl;
  m_dbroker.setTimingInfo();
  cout << "Done Spliting alog file(s)-------------------" << endl;
  m_dbroker.cacheMasterIndices();
  m_dbroker.cacheBehaviorIndices();

  buildGraphical();
  if(m_gui)
    checkForMinMaxTime(argc, argv);

  total_timer.stop();
  cout << termColor("blue") << "Done alogview launch time (cpu): ";
  cout << total_timer.get_float_cpu_time() << endl;
  cout << "Done alogview launch time (wall): ";
  cout << total_timer.get_float_wall_time() << termColor() << endl;

  return(m_gui);
}

//-------------------------------------------------------------
// Procedure: addPlotRequest()
//      Note: Expecting the request to be in the form:
//            VAR,FLD,FLD,FLD... with at least one FLD

bool LogViewLauncher::addPlotRequest(string request)
{
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  if(vsize < 2)
    return(false);
  
  for(i=0; i<vsize; i++) {
    if(isNumber(svector[i]) || strContainsWhite(svector[i]))
      return(false);
  }

  string moos_var = svector[0];
  for(i=1; i<vsize; i++) {
    m_plot_request_var.push_back(moos_var);
    m_plot_request_fld.push_back(svector[i]);
  }

  return(true);
}


//-------------------------------------------------------------
// Procedure: checkForMinMaxTime
//   Purpose: Handle the args --mintime, --maxtime, --nowtime

void LogViewLauncher::checkForMinMaxTime(int argc, char **argv)
{
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "--mintime=", 10)) {
      string sval = argi.substr(10);
      if(isNumber(sval))
	m_gui->setClipTimeMin(atof(sval.c_str()));
    }
    else if(strBegins(argi, "--maxtime=", 10)) {
      string sval = argi.substr(10);
      if(isNumber(sval))
	m_gui->setClipTimeMax(atof(sval.c_str()));
    }
    else if(strBegins(argi, "--nowtime=", 10)) {
      string sval = argi.substr(10);
      if(isNumber(sval))
	m_gui->setCurrTime(atof(sval.c_str()));
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
  m_tiff_file = "Default.tif";
  for(int i=1; i<argc; i++) {
    if(!strncmp(argv[i], "--background=", 13)) {
      string argi  = tolower(argv[i]);
      string front = biteString(argi, '=');
      string value = argi;
      
      if((value == "mit") || (value=="charles"))
	m_tiff_file = "AerialMIT.tif";
      else if((value == "fl") || (value == "forrest"))
	m_tiff_file = "forrest19.tif";
      else if(value == "none")
	m_tiff_file = "";
      else
	m_tiff_file = value;
    }
  }
  cout << "Tiff File: " << m_tiff_file << endl;
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
  m_gui_width  = 1200;
  m_gui_height = 800;
  
  for(int i=1; i<argc; i++) {
    string argi  = tolower(argv[i]);
    if(strBegins(argi, "--geometry=")) {
      biteString(argi, '=');
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
	string width  = biteStringX(value, 'x');
	string height = value;
	if(isNumber(width) && isNumber(height)) {
	  m_gui_width  = atof(width.c_str());
	  m_gui_height = atof(height.c_str());
	  m_gui_width  = vclip(m_gui_width, 750, 1920);
	  m_gui_height = vclip(m_gui_height, 600, 1200);
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
  for(int i=1; i<argc; i++) {
    if(strEnds(argv[i], ".alog")) 
      m_dbroker.addALogFile(argv[i]);
  }
}


//-------------------------------------------------------------
// Procedure: buildGraphical

void LogViewLauncher::buildGraphical()
{
  m_gui = new REPLAY_GUI(m_gui_width, m_gui_height, "alogview");
  if(!m_gui)
    return;

  m_gui->setDataBroker(m_dbroker);
  
  // Try to initialize the two LogPlots to be something reasonable
  unsigned int mix_size = m_dbroker.sizeMix();
  if(mix_size > 0) {
    string vname = m_dbroker.getVNameFromMix(0);
    m_gui->initLogPlotChoiceA(vname, "NAV_SPEED");
    m_gui->initLogPlotChoiceB(vname, "DESIRED_HEADING");
  }

  m_gui->updateXY();
  m_gui->np_viewer->setParam("tiff_file", m_tiff_file);

  m_gui->setCurrTime(-1); // GUI will seek a "start_time hint"
}

