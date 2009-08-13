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

#include <stdlib.h>
#include "MBUtils.h"
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
}

//-------------------------------------------------------------
// Procedure: setLaunch

int LogViewLauncher::launch(int argc, char **argv)
{
  setBackground(argc, argv);
  setSizeOfGUI(argc, argv);
  setALogFiles(argc, argv);

  parseALogFiles();

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
  for(int i=1; i<argc; i++) {
    if(strContains(argv[i], ".alog")) {
      m_alog_files.push_back(argv[i]);
      m_alog_files_skew.push_back(0);
    }
  }
}


//-------------------------------------------------------------
// Procedure: parseALogFiles
//            Parse all .alog files on the commmand line

void LogViewLauncher::parseALogFiles()
{
  unsigned int i, vsize = m_alog_files.size();
  for(i=0; i<vsize; i++)
    parseALogFile(i);
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
  fclose(f);

  vector<string> lines = fileBuffer(filestr);
  unsigned int lsize = lines.size();

  vector<ALogEntry> entries_log_plot;
  vector<ALogEntry> entries_bhv_ipf;
  vector<ALogEntry> entries_vplug_plot;
  vector<ALogEntry> entries_helm_plot;

  for(i=0; i<lsize; i++) {
    lines[i] = findReplace(lines[i], '\t', ' ');
    lines[i] = compactConsecutive(lines[i], ' ');
    lines[i] = stripBlankEnds(lines[i]);
    
    string time = stripBlankEnds(biteString(lines[i],' '));
    string var  = stripBlankEnds(biteString(lines[i],' '));
    string source = stripBlankEnds(biteString(lines[i],' '));
    string value  = stripBlankEnds(lines[i]);

    double dtime = atof(time.c_str());

    ALogEntry entry;
    if(isNumber(value)) {
      entry.set(dtime, var, source, atof(value.c_str()));
      entries_log_plot.push_back(entry);
    }
    else
      entry.set(dtime, var, source, value);
    
    if((var == "VIEW_POINT")   || (var == "VIEW_POLYGON") ||
       (var == "VIEW_SEGLIST") || (var == "VIEW_CIRCLE")  ||
       (var == "GRID_INIT")    || (var == "GRID_DELTA") ||
       (var == "VIEW_MARKER")  || (var == "NODE_REPORT_LOCAL")) 
      entries_vplug_plot.push_back(entry);
    if((var == "IVPHELM_SUMMARY") || (var == "NODE_REPORT_LOCAL"))
      entries_helm_plot.push_back(entry);
    if((var == "BHV_IPF") || (var == "NODE_REPORT_LOCAL"))
      entries_bhv_ipf.push_back(entry);
  }

  m_entries_log_plot.push_back(entries_log_plot);
  m_entries_bhv_ipf.push_back(entries_bhv_ipf);
  m_entries_vplug_plot.push_back(entries_vplug_plot);
  m_entries_helm_plot.push_back(entries_helm_plot);
}
  
