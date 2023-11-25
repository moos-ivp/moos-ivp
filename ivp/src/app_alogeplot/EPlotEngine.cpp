/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: EPlotEngine.cpp                                      */
/*    DATE: Jnauary 16th, 2016                                   */
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

#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "MBUtils.h"
#include "EPlotEngine.h"
#include "LogUtils.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------
// Constructor

EPlotEngine::EPlotEngine()
{
  m_verbose     = false;
  m_scene_file  = "./.tmp.scn";
  m_point_size  = "0.1";
  m_point_color = "darkblue";

  setPlotWidCM("15");
  setPlotHgtCM("10");
  setGridWidCM("15");
  setGridHgtCM("10");
  //setPlotWidCM("30");
  //setPlotHgtCM("20");
  //setGridWidCM("30");
  //setGridHgtCM("20");
}

//--------------------------------------------------------
// Procedure: addALogFile()

bool EPlotEngine::addALogFile(string alog_file)
{
  if(!okFileToRead(alog_file)) {
    cout << "Unable to read from: " << alog_file << endl;
    return(false);
  }
  
  m_alog_files.push_back(alog_file);
  return(true);
}

//--------------------------------------------------------
// Procedure: setSceneFile()

bool EPlotEngine::setSceneFile(string scene_file)
{
  if(!okFileToWrite(scene_file)) {
    cout << "Unable to use scene file: " << scene_file << endl;
    return(false);
  }
  
  m_scene_file = scene_file;
  return(true);
}

//--------------------------------------------------------
// Procedure: setPointColor()

bool EPlotEngine::setPointColor(string str)
{
  if(!isColor(str))
    return(false);  
  m_point_color = str;
  return(true);
}

//--------------------------------------------------------
// Procedure: setPointSize()

bool EPlotEngine::setPointSize(string str)
{
  if(!isNumber(str))
    return(false);
  m_point_size = str;
  return(true);
}

//--------------------------------------------------------
// Procedure: setPlotWidCM()

bool EPlotEngine::setPlotWidCM(string str)
{
  if(!isNumber(str))
    return(false);

  m_plot_wid_cm = atof(str.c_str());
  if(m_plot_wid_cm < 4)
    m_plot_wid_cm = 4;

  m_plot_wid_cm_str = doubleToStringX(m_plot_wid_cm,2);
  return(true);
}

//--------------------------------------------------------
// Procedure: setPlotHgtCM()

bool EPlotEngine::setPlotHgtCM(string str)
{
  if(!isNumber(str))
    return(false);

  m_plot_hgt_cm = atof(str.c_str());
  if(m_plot_hgt_cm < 4)
    m_plot_hgt_cm = 4;

  m_plot_hgt_cm_str = doubleToStringX(m_plot_hgt_cm,2);
  return(true);
}

//--------------------------------------------------------
// Procedure: setGridWidCM()

bool EPlotEngine::setGridWidCM(string str)
{
  if(!isNumber(str))
    return(false);

  m_grid_wid_cm = atof(str.c_str());
  if(m_grid_wid_cm < 4)
    m_grid_wid_cm = 4;

  m_grid_wid_cm_str = doubleToStringX(m_grid_wid_cm,2);
  return(true);
}

//--------------------------------------------------------
// Procedure: setGridHgtCM()

bool EPlotEngine::setGridHgtCM(string str)
{
  if(!isNumber(str))
    return(false);

  m_grid_hgt_cm = atof(str.c_str());
  if(m_plot_hgt_cm < 4)
    m_plot_hgt_cm = 4;

  m_grid_hgt_cm_str = doubleToStringX(m_grid_hgt_cm,2);
  return(true);
}

//--------------------------------------------------------
// Procedure: generate()

void EPlotEngine::generate()
{
  handleALogFiles();
  
  FILE *file_ptr = fopen(m_scene_file.c_str(), "w");
  if(!file_ptr) {
    cout << "Problem with file: " << m_scene_file << endl;
    return;
  }

  // Begin creation of the scene (.scn) file
  writeBaseGridFrame(file_ptr);
  writeBaseZones(file_ptr);
  writeBaseLabels(file_ptr);
  writeEncounters(file_ptr);

  fclose(file_ptr);

  // Begin creation of the Latex file
  string cmd = "scn2jpg.sh -v -s -o " + m_scene_file;
  int result = system(cmd.c_str());
  if(result != 0)
    cout << "Possible err in EPlotEngine Syscmd: " << cmd << endl;

}

//--------------------------------------------------------
// Procedure: handleALogFiles()

void EPlotEngine::handleALogFiles()
{
  m_community_name = "";
  m_cpa_events.clear();
  m_collision_range = 0;
  m_near_miss_range = 0;
  m_encounter_range = 0;

  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    bool ok = handleALogFile(m_alog_files[i]);
    if(!ok) {
      cout << "Could not process file: " << m_alog_files[i] << endl;
      exit(1);
    }
  }

  cout << "Total encounters: " << m_cpa_events.size() << endl;
  cout << "Collision Range:  " << m_collision_range << endl;
  cout << "Near Miss Range:  " << m_near_miss_range << endl;
  cout << "Encounter Range:  " << m_encounter_range << endl;
  cout << "Community Name:   " << m_community_name  << endl;

}


//--------------------------------------------------------
// Procedure: handleALogFile()

bool EPlotEngine::handleALogFile(string alog_file)
{
  FILE *file_ptr = fopen(alog_file.c_str(), "r");
  if(!file_ptr)
    return(false);
  
  if(m_verbose)
    cout << "Processing: " << alog_file << endl;

  unsigned int line_count  = 0;
  while(1) {
    line_count++;
    string line_raw = getNextRawLine(file_ptr);

    if(m_verbose) {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 100000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }
    
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    if(line_raw == "eof") 
      break;
    
    string var = getVarName(line_raw);
    if((m_community_name == "") && (var == "DB_TIME")) {
      string src = getSourceName(line_raw);
      m_community_name = rbiteString(src, '_');
    }
    else if(var == "ENCOUNTER_SUMMARY") {
      string spec = getDataEntry(line_raw);
      CPAEvent event(spec);
      m_cpa_events.push_back(event);
    }
    else if(var == "COLLISION_DETECT_PARAMS") {
      string param_list = getDataEntry(line_raw);
      vector<string> svector = parseString(param_list, ',');
      for(unsigned int i=0; i<svector.size(); i++) {
	string param = biteStringX(svector[i], '=');
	string value = svector[i];
	if(param == "collision_range")
	  m_collision_range = atof(value.c_str());
	else if(param == "near_miss_range")
	  m_near_miss_range = atof(value.c_str());
	else if(param == "encounter_range")
	  m_encounter_range = atof(value.c_str());
      }
    }
  }

  if(m_verbose) 
    cout << endl << uintToCommaString(line_count) << " lines total." << endl;
  
  if(file_ptr)
    fclose(file_ptr);

  return(true);
}


//--------------------------------------------------------
// Procedure: writeBaseGridFrame

void EPlotEngine::writeBaseGridFrame(FILE *f) const
{
  // Create the pts for the plot box (overall enclosing box)
  string pxl = "0";
  string pxh = m_plot_wid_cm_str;
  string pyl = "0";
  string pyh = m_plot_hgt_cm_str;
  stringstream framepts;
  framepts << "{";
  framepts << pxl << "," << pyl << ":";
  framepts << pxl << "," << pyh << ":";
  framepts << pxh << "," << pyh << ":";
  framepts << pxh << "," << pyl << ":";
  framepts << pxl << "," << pyl << "}";

  stringstream ss;
  ss << "grid = key=master,group=grid,depth=1,gcell=5,      \\ " << endl;
  ss << "       rounded=true,border_thickness=thin,         \\ " << endl;
  ss << "       border_color=menublue,gback_color=white,    \\ " << endl; 
  ss << "       gline_thickness=thin,gline_color=white,     \\ " << endl;
  ss << "       widthcm="  << m_grid_wid_cm_str << ",       \\ " << endl;
  ss << "       heightcm=" << m_grid_hgt_cm_str << "           " << endl;
  ss << endl; 
  ss << "polygon = key=tpoly,group=mission,depth=99,        \\ " << endl;
  ss << "          point_size=0,point_color=brown,          \\ " << endl;
  ss << "          line_color=black,line_style=solid,       \\ " << endl;
  ss << "          line_thickness=thin,                     \\ " << endl;
  ss << "          pts=" << framepts.str() << "                " << endl;
  ss << endl;
  ss << "scene_boundary = 1.5                                    " << endl;
  ss << endl;

  fprintf(f, "%s\n", ss.str().c_str());
}

//--------------------------------------------------------
// Procedure: writeBaseZones

void EPlotEngine::writeBaseZones(FILE *f) const
{
  double collision_pct = (m_collision_range / m_encounter_range);
  double near_miss_pct = (m_near_miss_range / m_encounter_range);
  double collision_range_cm = collision_pct * m_plot_wid_cm;
  double near_miss_range_cm = near_miss_pct * m_plot_wid_cm;
  
  string exl = doubleToStringX(near_miss_range_cm);
  string exh = m_plot_wid_cm_str;
  string eyl = "0";
  string eyh = doubleToStringX((m_plot_hgt_cm/2),2);
  stringstream ezonepts;
  ezonepts << "{";
  ezonepts << exl << "," << eyl << ":";
  ezonepts << exl << "," << eyh << ":";
  ezonepts << exh << "," << eyh << ":";
  ezonepts << exh << "," << eyl << ":";
  ezonepts << exl << "," << eyl << "}";
  
  string kxl = "0";
  string kxh = doubleToStringX(collision_range_cm);
  string kyl = "0";
  string kyh = m_plot_hgt_cm_str;
  stringstream kzonepts;
  kzonepts << "{";
  kzonepts << kxl << "," << kyl << ":";
  kzonepts << kxl << "," << kyh << ":";
  kzonepts << kxh << "," << kyh << ":";
  kzonepts << kxh << "," << kyl << ":";
  kzonepts << kxl << "," << kyl << "}";
  
  string nxl = doubleToString(collision_range_cm,2);
  string nxh = doubleToString(near_miss_range_cm,2);
  string nyl = "0";
  string nyh = m_plot_hgt_cm_str;
  stringstream nzonepts;
  nzonepts << "{";
  nzonepts << nxl << "," << nyl << ":";
  nzonepts << nxl << "," << nyh << ":";
  nzonepts << nxh << "," << nyh << ":";
  nzonepts << nxh << "," << nyl << ":";
  nzonepts << nxl << "," << nyl << "}";
  
  stringstream ss;
  ss << "polygon = key=kzone,group=mission,depth=98,           \\ " << endl;
  ss << "          point_size=0,point_color=brown,             \\ " << endl;
  ss << "          line_color=gray80,line_style=solid,         \\ " << endl;
  ss << "          line_thickness=thin,fill_color=lightpink,   \\ " << endl;
  ss << "          pts=" << kzonepts.str() << "                   " << endl;
  ss << endl;
  ss << "polygon = key=nzone,group=mission,depth=98,           \\ " << endl;
  ss << "          point_size=0,point_color=brown,             \\ " << endl;
  ss << "          line_color=gray80,line_style=solid,         \\ " << endl;
  ss << "          line_thickness=thin,fill_color=lightyellow, \\ " << endl;
  ss << "          pts=" << nzonepts.str() << "                \\ " << endl;
  ss << endl; 
  ss << "polygon = key=ezone,group=mission,depth=98,        \\ " << endl;
  ss << "          point_size=0,point_color=brown,          \\ " << endl;
  ss << "          line_color=gray80,line_style=solid,      \\ " << endl;
  ss << "          line_thickness=thin,fill_color=gray95,   \\ " << endl;
  ss << "          pts=" << ezonepts.str() << "                " << endl;
  ss << endl; 

  fprintf(f, "%s\n", ss.str().c_str());
}

//--------------------------------------------------------
// Procedure: writeBaseLabels

void EPlotEngine::writeBaseLabels(FILE *f) const
{
  double encounter_range_cm = m_plot_wid_cm;

  double x_cpa_label_cm = 0.67 * m_plot_wid_cm;
  string x_cpa_label_cm_str = doubleToString(x_cpa_label_cm,2);

  stringstream ss;
  ss << "// Axis Titles                                        " << endl;
  ss << "label = key=x-axis,group=mission,depth=205,        \\ " << endl;
  ss << "        label=Closest Point of Approach (meters),  \\ " << endl;
  ss << "        label_color=black,label_size=footnotesize, \\ " << endl;
  ss << "        y=-1.5,                                    \\ " << endl;
  ss << "        x=" << x_cpa_label_cm_str << "                " << endl;
  ss << endl; 
  ss << "label = key=y-axis,group=mission,depth=205,        \\ " << endl;
  ss << "        label=Efficiency,label_color=black,        \\ " << endl;
  ss << "        label_size=footnotesize,x=-2,              \\ " << endl;
  ss << "        y=" << doubleToStringX(m_grid_hgt_cm/2,2)       << endl;
  ss << endl;
  ss << "label = key=y-axis_copy,group=mission,depth=205,   \\ " << endl;
  ss << "        label=[0-100]\\%,label_color=black,        \\ " << endl;
  ss << "        label_size=footnotesize,x=-2,              \\ " << endl;
  ss << "        y=" << doubleToStringX((m_grid_hgt_cm/2)-2,2)   << endl;
  ss << endl;


  string krange_str = doubleToStringX(m_collision_range,2);
  string nrange_str = doubleToStringX(m_near_miss_range,2);
  string erange_str = doubleToStringX(m_encounter_range,2);

  double collision_pct = (m_collision_range / m_encounter_range);
  double near_miss_pct = (m_near_miss_range / m_encounter_range);
  double collision_range_cm = collision_pct * encounter_range_cm;
  double near_miss_range_cm = near_miss_pct * encounter_range_cm;

  string collision_range_cm_str = doubleToString(collision_range_cm,2);
  string near_miss_range_cm_str = doubleToString(near_miss_range_cm,2);
  string encounter_range_cm_str = doubleToString(encounter_range_cm-1,2);

  ss << "// X Axis Numerical labels                            " << endl;
  ss << "label = key=x-axis-0,group=mission,depth=205,      \\ " << endl;
  ss << "        label_color=black,                         \\ " << endl;
  ss << "        label=0,                                   \\ " << endl;
  ss << "        label_size=scriptsize,x=0.25,y=-0.5           " << endl;
  ss << endl;
  ss << "label = key=x-axis-k,group=mission,depth=205,      \\ " << endl;
  ss << "        label_color=black,                         \\ " << endl;
  ss << "        label=" << krange_str << ",                \\ " << endl;
  ss << "        label_size=scriptsize,y=-0.5,              \\ " << endl;
  ss << "        x=" << collision_range_cm_str << "            " << endl;
  ss << endl;
  ss << "label = key=x-axis-n,group=mission,depth=205,      \\ " << endl;
  ss << "        label_color=black,                         \\ " << endl;
  ss << "        label=" << nrange_str << ",                \\ " << endl;
  ss << "        label_size=scriptsize,y=-0.5,              \\ " << endl;
  ss << "        x=" << near_miss_range_cm_str << "            " << endl;
  ss << endl;
  ss << "label = key=x-axis-e,group=mission,depth=205,      \\ " << endl;
  ss << "        label=30,label_color=black,                \\ " << endl;
  ss << "        label=" << erange_str << ",                \\ " << endl;
  ss << "        label_size=scriptsize,y=-0.5,              \\ " << endl;
  ss << "        x=" << encounter_range_cm << "                " << endl;

  fprintf(f, "%s\n", ss.str().c_str());
}

//--------------------------------------------------------
// Procedure: writeEncounters()

void EPlotEngine::writeEncounters(FILE* f) const
{
  stringstream ss;

  string pcolor = m_point_color;
  string psize  = m_point_size;
  string depth  = "180";
  string group  = "eplot";
  for(unsigned int i=0; i<m_cpa_events.size(); i++) {
    double cpa = m_cpa_events[i].getCPA();
    double eff = m_cpa_events[i].getEFF();

    double y   = (eff/100) * m_plot_hgt_cm;
    double x   = (cpa / m_encounter_range) * m_plot_wid_cm;
    string xstr = doubleToStringX(x,2);
    string ystr = doubleToStringX(y,2);

    ss << "point = key=p" << i << ",group=" << group;
    ss << ",depth=" << depth << ",x=" << xstr << ",y=" << ystr << ", \\ " << endl;
    ss << "        point_size=" << psize << ",point_color=" << pcolor << endl;
    //    ss << endl;
  }
  fprintf(f, "%s\n", ss.str().c_str());
  
}
  





