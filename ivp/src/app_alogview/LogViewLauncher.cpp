/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "TermUtils.h"
#include "MBTimer.h"
#include "LogViewLauncher.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Constructor()

LogViewLauncher::LogViewLauncher()
{
  m_gui_width  = 1200;
  m_gui_height = 800;
  m_gui        = 0;
  m_verbose    = false;
  
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

  // Configure to show updates on num lines read during reading
  m_dbroker.setProgress(true);
}

//-------------------------------------------------------------
// Procedure: launch()

REPLAY_GUI *LogViewLauncher::launch(int argc, char **argv)
{
  MBTimer total_timer;
  total_timer.start();

  bool ok = true;

  cout << termColor("blue");
  cout << "*********************************************************" << endl;
  cout << "* alogview                    Mike Benjamin, MIT MechE  *" << endl; 
  cout << "*********************************************************" << endl;
  cout << termColor();
  
  ok = ok && parseCommandArgs(argc, argv);
  ok = ok && sanityCheck();
  ok = ok && configDataBroker();
  ok = ok && configRegionInfo();
  ok = ok && configGraphical();
  
  total_timer.stop();

  double dbl_wall = total_timer.get_float_cpu_time();
  double dbl_cpu  = total_timer.get_float_wall_time();

  string str = "CPU Time: " + doubleToString(dbl_cpu,2);
  str += " Wall time: " + doubleToString(dbl_wall,2);
  str = padString(str, 54, false);

  cout << termColor("blue");
  cout << "*********************************************************" << endl;
  cout << "* Launch Summary:                                       *" << endl; 
  cout << "* " << str << "*" << endl;
  cout << "*********************************************************" << endl;
  cout << termColor();

  if(!ok)
    return(0);
  return(m_gui);
}

//-------------------------------------------------------------
// Procedure: parseCommandArgs()

bool LogViewLauncher::parseCommandArgs(int argc, char **argv)
{
  if(m_verbose)
    cout << "Parsing Command Args..." << endl;
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
    string argi = argv[i];
    bool handled = handleConfigParam(argi);

    if(!handled) {
      cout << "Unhandled argument: " << argi << endl;
      return(false);
    }
  }

  // Part 4: Handle the alog files last
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strEnds(argi, ".alog")) 
      m_dbroker.addALogFile(argi);
  }

  return(true);
}

//-------------------------------------------------------------
// Procedure: handleConfigParam()
//   Purpose: A handler that can parse config parameters given on
//            the command line, and config params in a config file.

bool LogViewLauncher::handleConfigParam(string argi)
{
  if(m_verbose)
    cout << "Handling Config Params: [" << argi << "]" << endl;

  bool handled = true;
  if(strEnds(argi, ".alog")) 
    handled = true; // handled separately
  else if(strBegins(argi, "--max_fptrs=")) 
    handled = handleMaxFilePtrs(argi.substr(12));
  else if(strBegins(argi, "--vqual=")) 
    handled = handleVQual(argi.substr(8));
  else if(strBegins(argi, "--bg="))
    handled = handleBackground(argi.substr(5));
  else if(strEnds(argi, ".tif")) 
    handled = handleBackground(argi);
  else if(strBegins(argi, "--mintime=")) 
    handled = handleMinTime(argi.substr(10));
  else if(strBegins(argi, "--maxtime=")) 
    handled = handleMaxTime(argi.substr(10));
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
  else if(strBegins(argi, "--grep=")) 
    handled = handleGrep(argi.substr(7));
  else if(strBegins(argi, "--bv=")) 
    handled = handleBehaviorVarMapping(argi.substr(5));
  else if((argi == "-vb") || (argi == "--verbose")) 
    m_verbose = true;
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
  if(m_verbose)
    cout << "Handling GUI Params..." << endl;
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
     strBegins(argi, "--hash_viewable=")            ||
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
  if(m_verbose)
    cout << "Handling Sanity Checks..." << endl;
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
// Procedure: configDataBroker()

bool LogViewLauncher::configDataBroker()
{
  cout << "*********************************************************" << endl;
  cout << "* STARTUP PART 1: Build/Confirm data cache files        *" << endl; 
  cout << "* The first time alogview launches on new alog file(s)  *" << endl;
  cout << "* this may take more time as a data cache is created by *" << endl;
  cout << "* creating a dedicated file for each logged variable.   *" << endl;
  cout << "* Subsequent re-launches on the same data will be fast. *" << endl;
  cout << "*********************************************************" << endl;

  bool ok = true;
  ok = ok && m_dbroker.checkALogFiles();
  ok = ok && m_dbroker.splitALogFiles();
  ok = ok && m_dbroker.setTimingInfo();

  if(!ok)
    return(false);

  if(m_min_time_set)
    m_dbroker.setPrunedMinTime(m_min_time);
  if(m_max_time_set) 
    m_dbroker.setPrunedMaxTime(m_max_time);

  m_dbroker.cacheMasterIndices();
  m_dbroker.cacheBehaviorIndices();
  m_dbroker.cacheAppLogIndices();

  return(true);
}


//-------------------------------------------------------------
// Procedure: configRegionInfo()

bool LogViewLauncher::configRegionInfo()
{
  string region_info = m_dbroker.getRegionInfo();
  string img_str  = tokStringParse(region_info, "img_file");
  string zoom_str = tokStringParse(region_info, "zoom");
  string panx_str = tokStringParse(region_info, "pan_x");
  string pany_str = tokStringParse(region_info, "pan_y");
  string lat_str  = tokStringParse(region_info, "lat_datum");
  string lon_str  = tokStringParse(region_info, "lon_datum");

  if(region_info == "") {
    lat_str="43.8253";
    lon_str="-70.3304";
    img_str="forrest19.tif";
    zoom_str="0.95";
    panx_str="-90";
    pany_str="-280";
    cout << termColor("red");
    cout << "WARNING: No REGION_INFO found in the log file. Defaulting " << endl;
    cout << "to use Forest Lake image and context. The REGION_INFO is  " << endl;
    cout << "normally published by pMarineViewer. If pMarineViwer was  " << endl;
    cout << "running, ensure that REGION_INFO is being logged. If      " << endl;
    cout << "pMarineViewer was not running, the content of REGION_INFO " << endl;
    cout << "can be passed on the command-line to alogview, or alogview" << endl;
    cout << "can be launch with Forest Lake, and just toggle off the   " << endl;
    cout << "background image if it is distracting.                    " << endl;
    cout << "                                                          " << endl;
    cout << "Hit Any Key to Continue.";
    cout << termColor();
    getCharNoWait();
  }
  
  cout << "*********************************************************" << endl;
  cout << "* STARTUP PART 2: Determine Region Information          *" << endl; 
  cout << "*    Image: " << padString(img_str,  44, false) <<     "*" << endl;
  cout << "*     Zoom: " << padString(zoom_str, 44, false) <<     "*" << endl;
  cout << "*     PanX: " << padString(panx_str, 44, false) <<     "*" << endl;
  cout << "*     PanY: " << padString(pany_str, 44, false) <<     "*" << endl;
  cout << "* DatumLat: " << padString(lat_str,  44, false) <<     "*" << endl;
  cout << "* DatumLon: " << padString(lon_str,  44, false) <<     "*" << endl;
  cout << "*********************************************************" << endl;

  //  if(region_info == "")
  //  return(true);

  // As a habit we prefer to start alogview more zoomed out than orig
  if(isNumber(zoom_str)) {
    double dval = atof(zoom_str.c_str());
    dval = dval / 2;
    string sval = doubleToString(dval);
    handleZoom(zoom_str);
  }

  handleBackground(img_str);
  handlePanX(panx_str);
  handlePanY(pany_str);

  return(true);
}



//-------------------------------------------------------------
// Procedure: configGraphical()

bool LogViewLauncher::configGraphical()
{
  cout << "*********************************************************" << endl;
  cout << "* STARTUP PART 3: Configure Graphical Information       *" << endl; 
  cout << "* (a) Init GUI Window                                   *" << endl; 
  cout << "* (b) Init DataBroker from Cache, connect to GUI        *" << endl; 
  cout << "* (c) Init Left/Right Log Plots                         *" << endl; 
  cout << "*********************************************************" << endl;

  cout << "(a) Initializing GUI... " << endl;
  m_gui = new REPLAY_GUI(m_gui_width, m_gui_height, "alogview");
  if(!m_gui)
    return(false);
  m_gui->np_viewer->setParam("set_pan_x", m_start_panx);
  m_gui->np_viewer->setParam("set_pan_y", m_start_pany);
  m_gui->np_viewer->setParam("set_zoom", m_start_zoom);
  m_gui->np_viewer->setVerbose(m_verbose);
  for(unsigned int i=0; i<m_gui_params.size(); i++) {
    string param = m_gui_params[i];
    string value = m_gui_values[i];
    m_gui->np_viewer->setParam(param, value);
  }

  // Map associating scope variables with given behaviors. For convencience
  // in using the GUI_IPF viewer.
  m_gui->setBehaviorVarMap(m_map_bhv_vars);

  for(unsigned int i=0; i<m_tiff_files.size(); i++)
    m_gui->np_viewer->setParam("tiff_file", m_tiff_files[i]);
  
  if(m_quick_start)
    m_gui->np_viewer->setMinimalMem();
  if(m_alt_nav_prefix != "")
    m_gui->np_viewer->setAltNavPrefix(m_alt_nav_prefix);


  double img_wid_mtrs = m_gui->np_viewer->getImgWidthMtrs();
  if(img_wid_mtrs > 5000000)
    m_gui->np_viewer->setParam("hash_delta", "1000000");
  else if(img_wid_mtrs > 500000)
    m_gui->np_viewer->setParam("hash_delta", "100000");
  else if(img_wid_mtrs > 50000)
    m_gui->np_viewer->setParam("hash_delta", "10000");
  else if(img_wid_mtrs > 5000)
    m_gui->np_viewer->setParam("hash_delta", "1000");
  else if(img_wid_mtrs > 500)
    m_gui->np_viewer->setParam("hash_delta", "100");
  else 
    m_gui->np_viewer->setParam("hash_delta", "50");
  
  cout << "(b) Initializing DataBroker from Cache files..." << endl; 
  m_gui->setDataBroker(m_dbroker);
  m_gui->setGrepStr1(m_grep1);
  m_gui->setGrepStr2(m_grep2);
  
  // Try to initialize the two LogPlots to be something reasonable
  if(!m_quick_start) {
    cout << "(c) Initializing Left/Right Plots" << endl;
    unsigned int mix_size = m_dbroker.sizeMix();
    if(mix_size > 0) {
      m_gui->initLogPlotChoiceA(m_start_veh_lft, m_start_var_lft);
      m_gui->initLogPlotChoiceB(m_start_veh_rgt, m_start_var_rgt);
    }
  }

  if(m_start_time > 0)
    m_gui->setCurrTime(m_start_time);
  else
    m_gui->setCurrTime(-1); // GUI will seek a "start_time hint"
  m_gui->updateXY();


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

  // Handle legacy aliases
  if(val == "mit") 
    m_tiff_file = "MIT_SP.tif";
  else if(val=="charles")
    m_tiff_file = "MIT_SP.tif";
  else if((val == "fl") || (val == "forrest"))
    m_tiff_file = "forrest19.tif";
  else
    m_tiff_file = val;
  
  if(!strEnds(m_tiff_file, ".tif"))
    return(false);

  m_tiff_files.push_back(m_tiff_file);

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
// Procedure: handleGrep()    --grep=foobar

bool LogViewLauncher::handleGrep(string val)
{
  if(val == "")
    return(false);

  if(m_grep1 == "")
    m_grep1 = val;
  else if(m_grep2 == "")
    m_grep2 = val;
  else
    return(false);

  return(true);
}
 
//-------------------------------------------------------------
// Procedure: handleMaxFilePtrs()    --max_fptrs=200
// 
// Note: This sets the limit on the number of open file pointers
//       open at any one time when splitting the alog file into
//       separate files. This number is limited by the operating
//       system, usually no less than about 256, but can be set
//       much higher. The larger the cache, the faster the
//       splitting process. For variables not cached, each line
//       in the alog file, the split file must be opened before
//       writing the line and closed immediately afterward.

bool LogViewLauncher::handleMaxFilePtrs(string val)
{
  if(!isNumber(val))
    return(false);

  int max_fptrs = atoi(val.c_str());
  if(max_fptrs < 10)
    max_fptrs = 10;
  if(max_fptrs > 1000)
    max_fptrs = 1000;

  m_dbroker.setMaxFilePtrs((unsigned int)(max_fptrs));
  
  return(true);
}
 
//-------------------------------------------------------------
// Procedure: handleVQual()    --vqual=MED/low/high/max
// 
// Note: This modifies the binval used in creating the time
//       indexed clusters of visual artifacts (VPlugPlot).
//       Lower binval size means higher precision, more load
//       time and memory usage.
//       MED uses a policy of adjusting the binval size based
//       on the number of visual objects.
//       LOW will double the binval size from the result of the
//       policy. HIGH will halve the value. MAX will set the
//       binval size to zero which is highest precision but
//       potentially very slow. (MAX was the only option prior
//       to July 2023)

bool LogViewLauncher::handleVQual(string val)
{
  val = tolower(val);
  if((val != "med") && (val != "low") && (val != "high") &&
     (val != "max") && (val != "vlow") && (val != "vvlow"))
    return(false);
  
  m_dbroker.setVQual(val);
  
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





