/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Apr 15th 2005                                        */
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
#include <time.h>
#include "FFV_GUI.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "IO_Utilities.h"
#include "PopulatorAOF.h"
#include "FileBuffer.h"
#include <cstdlib>

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("ffview", "gpl");
    return(0);
  }

  int i;
  FFV_GUI *gui  = new FFV_GUI(950, 700, "aof-ipf-function-viewer");

  //-------------------------------------------- Handle AOF Files
  string aof_file;
  string info_file;
  for(i=1; i<argc; i++) {
    string argi = argv[i];
    if(strContains(argv[i], ".aof"))
      aof_file = argv[i];
    else if(strContains(argv[i], ".info"))
      info_file = argv[i];
  }

  if(aof_file != "") {
    PopulatorAOF populator;
    AOF *aof = populator.populate(aof_file);
    if(aof) {
      cout << "AOF built." << endl;
      gui->setAOF(aof);
    }
    else
      cout << "AOF not built." << endl;
  }
  
  if(info_file != "") {
    FILE *f = fopen(info_file.c_str(), "r");
    if(!f) {
      cout << "Could not find File: " << info_file << endl;
      return(0);
    }
    cout << "Successfully found file: " << info_file << endl;
    fclose(f);
    vector<string> file_vector = fileBuffer(info_file);
    int lineCount = file_vector.size();
    
    for(int j=0; j<lineCount; j++) {
      string line = stripBlankEnds(file_vector[j]);
      if((line.length()!=0) && ((line)[0]!='#')) {
	vector<string> svector = parseString(line, '=');
	if(svector.size() == 2) {
	  string left  = stripBlankEnds(svector[0]);
	  string right = stripBlankEnds(svector[1]);
	  cout << "left:  " << left  << endl;
	  cout << "right: " << right << endl;
	  if(isNumber(right)) 
	    gui->setViewerParam(left, atof(right.c_str()));
	  else
	    gui->setViewerParam(left, right);
	}
      }
    }
  }
  
  
  // Now Handle command-line over-rides of parameter default values or
  // parameter values set in the info file if any.
  for(i=1; i<argc; i++) {
    string argi = argv[i];
    if(strContains(argv[i], "frame_color=")) {
      string frame_color = tokStringParse(argi, "frame_color", '#', '=');
      gui->setViewerParam("frame_color", frame_color);
    }
    else if(strContains(argv[i], "clear_color=")) {
      string clear_color = tokStringParse(argi, "clear_color", '#', '=');
      gui->setViewerParam("clear_color", clear_color);
    }
    else if(strContains(argv[i], "back_color=")) {
      string clear_color = tokStringParse(argi, "back_color", '#', '=');
      gui->setViewerParam("clear_color", clear_color);
    }
    else if(strContains(argv[i], "draw_frame=")) {
      string draw_frame = tokStringParse(argi, "draw_frame", '#', '=');
      gui->setViewerParam("draw_frame", draw_frame);
    }
    else if(strContains(argv[i], "draw_base=")) {
      string draw_base = tokStringParse(argi, "draw_base", '#', '=');
      gui->setViewerParam("draw_base", draw_base);
    }
    else if(strContains(argv[i], "frame_height=")) {
      double frame_height = tokDoubleParse(argi, "frame_height", '#', '=');
      gui->setViewerParam("set_frame_height", frame_height);
    }
    else if(strContains(argv[i], "set_x_rotation=")) {
      double xrot = tokDoubleParse(argi, "set_x_rotation", '#', '=');
      gui->setViewerParam("set_x_rotation", xrot);
    }
    else if(strContains(argv[i], "set_z_rotation=")) {
      double zrot = tokDoubleParse(argi, "set_z_rotation", '#', '=');
      gui->setViewerParam("set_z_rotation", zrot);
    }
  }

  return Fl::run();
}

