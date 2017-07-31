/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogViewLauncher.cpp                                  */
/*    DATE: May 31st, 2005                                       */
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
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

LogViewLauncher::LogViewLauncher()
{
  m_gui_width  = 1200;
  m_gui_height = 800;
  m_gui        = 0;

  m_tiff_file      = "Default.tif";
  m_start_var_lft  = "NAV_SPEED";
  m_start_var_rgt  = "DESIRED_HEADING";
  m_alt_nav_prefix = "NAV_GT_";

  m_start_panx = 0;
  m_start_pany = 0;
  m_start_zoom = 1;
  m_start_time = 0;

  m_min_time = 0;
  m_max_time = 0;
  m_min_time_set = 0;
  m_max_time_set = 0;

  m_quick_start = false;
  m_config_file_read = false;
}

//-------------------------------------------------------------
// Procedure: launch

REPLAY_GUI *LogViewLauncher::launch(int argc, char **argv)
{
  MBTimer total_timer;
  total_timer.start();

  bool ok = true;
  ok = ok && parseCommandArgs(argc, argv);
  ok = ok && sanityCheck();
  ok = ok && configDataBroker();
  ok = ok && configGraphical();
  
  total_timer.stop();
  cout << termColor("blue") << "Done alogview launch time (cpu): ";
  cout << total_timer.get_float_cpu_time() << endl;
  cout << "Done alogview launch time (wall): ";
  cout << total_timer.get_float_wall_time() << termColor() << endl;

  if(!ok)
    return(0);
  return(m_gui);
}

//-------------------------------------------------------------
// Procedure: parseCommandArgs

bool LogViewLauncher::parseCommandArgs(int argc, char **argv)
{
  // Part 1: Handle any alogview config file explicitly given
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strBegins(argi, "--alc=")) {
      string filename = argi.substr(6);
      bool handled = handleALogViewConfig(filename);
      if(!handled) {
	cout << "Unhandled alogview config file: " << filename << endl;
	return(false);
      }	
    }
  }

  // Part 2: If no alogview config file explicitly given, check for
  // the default but optional alogview config file, .alogview, located
  // in the current working directory.
  if(!m_config_file_read)
    handleALogViewConfig(".alogview");
  
  // Part 3: Handle all other arguments. Command line arguments will
  // thus override anything provided in one of the alogview config files.
  for(int i=1; i<argc; i++) {
    bool handled = true;
    string argi = argv[i];
    if(strEnds(argi, ".alog")) 
      m_dbroker.addALogFile(argi);
    else
      handled = handleConfigParam(argi);

    if(!handled) {
      cout << "Unhandled argument: " << argi << endl;
      return(false);
    }
  }

  return(true);
}

//-------------------------------------------------------------
// Procedure: handleConfigParam()
//   Purpose: A handler that can parse config parameters given on
//            the command line, and config params in a config file.

bool LogViewLauncher::handleConfigParam(string argi)
{
  bool handled = true;
  if(strEnds(argi, ".alog")) 
    m_dbroker.addALogFile(argi);
  else if(strBegins(argi, "--mintime=")) 
    handled = handleMinTime(argi.substr(10));
  else if(strBegins(argi, "--maxtime=")) 
    handled = handleMaxTime(argi.substr(10));
  else if(strBegins(argi, "--bg="))
    handled = handleBackground(argi.substr(5));
  else if(strBegins(argi, "--geometry=")) 
    handled = handleGeometry(argi.substr(11));
  else if(strBegins(argi, "--lp="))
    handled = handleInitialLogPlotL(argi.substr(5));
  else if(strBegins(argi, "--rp=")) 
    handled = handleInitialLogPlotR(argi.substr(5));
  else if(strBegins(argi, "--panx=")) 
    handled = handlePanX(argi.substr(7));
  else if(strBegins(argi, "--pany=")) 
    handled = handlePanY(argi.substr(7));
  else if(strBegins(argi, "--zoom=")) 
    handled = handleZoom(argi.substr(7));
  else if(strBegins(argi, "--nowtime=")) 
    handled = handleNowTime(argi.substr(10));
  else if(strBegins(argi, "--bv=")) 
    handled = handleBehaviorVarMapping(argi.substr(5));
  else if((argi == "--quick") || (argi == "-q")) 
    m_quick_start = true;
  else if(strBegins(argi, "--altnav=")) 
    m_alt_nav_prefix = argi.substr(9);
  else
    handled = handleParamsGUI(argi);

  return(handled);
}

//-------------------------------------------------------------
// Procedure: handleParamsGUI

bool LogViewLauncher::handleParamsGUI(string argi)
{
  if(strBegins(argi, "--seglist_viewable_all=")     ||
     strBegins(argi, "--seglist_viewable_labels=")  ||
     strBegins(argi, "--seglr_viewable_all=")       ||
     strBegins(argi, "--seglr_viewable_labels=")    ||
     strBegins(argi, "--point_viewable_all=")       ||
     strBegins(argi, "--point_viewable_labels=")    ||
     strBegins(argi, "--vector_viewable_all=")      ||
     strBegins(argi, "--vector_viewable_labels=")   ||
     strBegins(argi, "--circle_viewable_all=")      ||
     strBegins(argi, "--circle_viewable_labels=")   ||
     strBegins(argi, "--grid_viewable_all=")        ||
     strBegins(argi, "--grid_viewable_labels=")     ||
     strBegins(argi, "--range_pulse_viewable_all=") ||
     strBegins(argi, "--trails_color=")             ||
     strBegins(argi, "--hash_vieweble=")            ||
     strBegins(argi, "--tiff_viewable=")            ||
     strBegins(argi, "--hash_delta=")               ||
     strBegins(argi, "--vehicles_shape_scale=")     ||
     strBegins(argi, "--vehicles_viewable=")        ||
     strBegins(argi, "--trails_point_size=")        ||
     strBegins(argi, "--trails_viewable=")          ||
     strBegins(argi, "--trails_color=")             ||
     strBegins(argi, "--marker_viewable_all=")      ||
     strBegins(argi, "--marker_viewable_labels=")   ||
     strBegins(argi, "--polygon_viewable_all=")     ||
     strBegins(argi, "--polygon_viewable_labels=")) {
    string value = rbiteString(argi, '=');
    string param = rbiteString(argi, '-');
    m_gui_params.push_back(param);
    m_gui_values.push_back(value);
  }
  else
    return(false);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: sanityCheck()

bool LogViewLauncher::sanityCheck()
{
  if(m_dbroker.sizeALogs() == 0)
    return(false);

  if(m_min_time_set && m_max_time_set && (m_min_time >= m_max_time)) {
    cout << "WARNING: --mintime > --maxtime. Ignoring request" << endl;
    return(false);
  }

  if(m_min_time_set && (m_min_time < 0))
    cout << "WARNING: --mintime is less than zero. " << endl;
    
  if(m_max_time_set && (m_max_time < 0))
    cout << "WARNING: --maxtime is less than zero. " << endl;

  return(true);
}


//-------------------------------------------------------------
// Procedure: configDataBroker

bool LogViewLauncher::configDataBroker()
{
  bool ok = true;
  cout << "Begin Checking alog file(s)------------------" << endl;
  ok = ok && m_dbroker.checkALogFiles();
  cout << "Begin Splitting alog file(s)------------------" << endl;
  ok = ok && m_dbroker.splitALogFiles();
  cout << "Begin TimeSetting alog file(s)---------------" << endl;
  ok = ok && m_dbroker.setTimingInfo();

  if(!ok)
    return(false);

  if(m_min_time_set)
    m_dbroker.setPrunedMinTime(m_min_time);
  if(m_max_time_set) 
    m_dbroker.setPrunedMaxTime(m_max_time);

  m_dbroker.cacheMasterIndices();
  m_dbroker.cacheBehaviorIndices();
  
  return(true);
}



//-------------------------------------------------------------
// Procedure: configGraphical

bool LogViewLauncher::configGraphical()
{
  m_gui = new REPLAY_GUI(m_gui_width, m_gui_height, "alogview");
  if(!m_gui)
    return(false);

  if(m_quick_start)
    m_gui->np_viewer->setMinimalMem();
  if(m_alt_nav_prefix != "")
    m_gui->np_viewer->setAltNavPrefix(m_alt_nav_prefix);

  m_gui->setDataBroker(m_dbroker);
  
  // Try to initialize the two LogPlots to be something reasonable
  if(!m_quick_start) {
    unsigned int mix_size = m_dbroker.sizeMix();
    if(mix_size > 0) {
      m_gui->initLogPlotChoiceA(m_start_veh_lft, m_start_var_lft);
      m_gui->initLogPlotChoiceB(m_start_veh_rgt, m_start_var_rgt);
    }
  }

  m_gui->np_viewer->setParam("set_pan_x", m_start_panx);
  m_gui->np_viewer->setParam("set_pan_y", m_start_pany);
  m_gui->np_viewer->setParam("set_zoom", m_start_zoom);
  m_gui->np_viewer->setParam("tiff_file", m_tiff_file);

  for(unsigned int i=0; i<m_gui_params.size(); i++) {
    string param = m_gui_params[i];
    string value = m_gui_values[i];
    m_gui->np_viewer->setParam(param, value);
  }
  
  if(m_start_time > 0)
    m_gui->setCurrTime(m_start_time);
  else
    m_gui->setCurrTime(-1); // GUI will seek a "start_time hint"
  m_gui->updateXY();

  // Map associating scope variables with given behaviors. For convencience
  // in using the GUI_IPF viewer.
  m_gui->setBehaviorVarMap(m_map_bhv_vars);
  
  return(true);
}


//-------------------------------------------------------------
// Procedure: handleMinTime()  --mintime=NUM

bool LogViewLauncher::handleMinTime(string val)
{
  if(!isNumber(val))
    return(false);

  m_min_time = atof(val.c_str());
  m_min_time_set = true;
  return(true);
}

//-------------------------------------------------------------
// Procedure: handleMaxTime()  --maxtime=NUM

bool LogViewLauncher::handleMaxTime(string val)
{
  if(!isNumber(val))
    return(false);

  m_max_time = atof(val.c_str());
  m_max_time_set = true;
  return(true);
}


//-------------------------------------------------------------
// Procedure: handleBackground  --bg=FILE.tif

bool LogViewLauncher::handleBackground(string val)
{
  if(val == "none") {
    m_tiff_file = "";
    return(true);
  }

  if((val == "mit") || (val=="charles"))
    m_tiff_file = "AerialMIT.tif";
  else if((val == "fl") || (val == "forrest"))
    m_tiff_file = "forrest19.tif";
  else if(strEnds(val, ".tif") || strEnds(val, ".tiff"))
    m_tiff_file = val;
  else
    return(false);

  return(true);
}


//-------------------------------------------------------------
// Procedure: handleGeometry  --geometry=SPEC

bool LogViewLauncher::handleGeometry(string val)
{
  if(val == "large")  {
    m_gui_width  = 1400;
    m_gui_height = 1100;
  }
  else if(val == "medium") {
    m_gui_width  = 1190;
    m_gui_height = 935;
  }
  else if(val == "small")  {
    m_gui_width  = 980;
    m_gui_height = 770;
  }
  else if(val == "xsmall") {
    m_gui_width  = 770;
    m_gui_height = 605;
  }
  else {
    string width  = biteStringX(val, 'x');
    string height = val;
    if(isNumber(width) && isNumber(height)) {
      m_gui_width  = atof(width.c_str());
      m_gui_height = atof(height.c_str());
      m_gui_width  = vclip(m_gui_width, 750, 1920);
      m_gui_height = vclip(m_gui_height, 600, 1200);
    }
    else
      return(false);
  }
  return(true);
}


//-------------------------------------------------------------
// Procedure: handleInitialLogPlotL  --lp=henry:NAV_X 
//                                   --lp=NAV_HEADING

bool LogViewLauncher::handleInitialLogPlotL(string val)
{
  if(strContains(val, ":")) {
    m_start_veh_lft = biteString(val, ':');
    m_start_var_lft = val;
  }
  else
    m_start_var_lft = val;
  return(true);
}


//-------------------------------------------------------------
// Procedure: handleInitialLogPlotR  --rp=henry:NAV_X 
//                                   --rp=NAV_HEADING

bool LogViewLauncher::handleInitialLogPlotR(string val)
{
  if(strContains(val, ":")) {
    m_start_veh_rgt = biteString(val, ':');
    m_start_var_rgt = val;
  }
  else
    m_start_var_rgt = val;
  return(true);
}

//-------------------------------------------------------------
// Procedure: handlePanX()    --panx=30

bool LogViewLauncher::handlePanX(string val)
{
  if(!isNumber(val))
    return(false);
  m_start_panx = atof(val.c_str());
  return(true);
}

//-------------------------------------------------------------
// Procedure: handlePanY()    --pany=30

bool LogViewLauncher::handlePanY(string val)
{
  if(!isNumber(val))
    return(false);
  m_start_pany = atof(val.c_str());
  return(true);
}

//-------------------------------------------------------------
// Procedure: handleZoom()    --zoom=1.4

bool LogViewLauncher::handleZoom(string val)
{
  if(!isNumber(val))
    return(false);
  m_start_zoom = atof(val.c_str());
  return(true);
}
  
//-------------------------------------------------------------
// Procedure: handleNowTime()    --nowtime=111.124

bool LogViewLauncher::handleNowTime(string val)
{
  if(!isNumber(val))
    return(false);
  m_start_time = atof(val.c_str());
  return(true);
}
 
//-------------------------------------------------------------
// Procedure: handleBehaviorVarMapping
//  Examples:
//      --bv=avdcol_ben:COLREGS_A_MODE_BEN
//      --bv=avdcol_ben:COLREGS_A_MODE_BEN:AVDCOL_RANGE_BEN

bool LogViewLauncher::handleBehaviorVarMapping(string val)
{
  if(!strContains(val, ':'))
    return(false);

  string bhv = biteStringX(val, ':');
  string var = val;

  m_map_bhv_vars[bhv] = val; 
  return(true);
}


//-------------------------------------------------------------
// Procedure: handleALogViewConfig

bool LogViewLauncher::handleALogViewConfig(string filename)
{
  if(m_config_file_read)
    return(false);
  
  vector<string> lines = fileBuffer(filename);
  if(lines.size() == 0)
    return(false);

  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);

    if((line == "") || strBegins(line, "//") || strBegins(line, "#"))
      continue;

    bool handled = handleConfigParam(line);
    if(!handled) {
      cout << "Unhandled argument: " << line << endl;
      return(false);
    }
  }
  m_config_file_read = true;
  return(true);
}




