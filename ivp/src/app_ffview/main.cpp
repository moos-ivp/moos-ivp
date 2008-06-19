/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Apr 15th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <time.h>
#include "GUI.h"
#include "AOF_Rings.h"
#include "MBUtils.h"
#include "IO_Utilities.h"
#include "PopulatorAOF.h"
#include "FileBuffer.h"
#include <cstdlib>

using namespace std;

void pause(int nMS)
{
  timespec TimeSpec;
  TimeSpec.tv_sec 	= nMS / 1000;
  TimeSpec.tv_nsec 	= (nMS%1000) *1000000;
  
  nanosleep(&TimeSpec,NULL);
}


//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  Fl::flush();
  pause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  int i;
  GUI *gui  = new GUI(1000, 900, "aof-ipf-function-viewer");

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
    if(aof)
      gui->setAOF(aof);
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
	    gui->viewer->setParam(left, atof(right.c_str()));
	  else
	    gui->viewer->setParam(left, right);
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
      gui->viewer->setParam("frame_color", frame_color);
    }
    else if(strContains(argv[i], "clear_color=")) {
      string clear_color = tokStringParse(argi, "clear_color", '#', '=');
      gui->viewer->setParam("clear_color", clear_color);
    }
    else if(strContains(argv[i], "back_color=")) {
      string clear_color = tokStringParse(argi, "back_color", '#', '=');
      gui->viewer->setParam("clear_color", clear_color);
    }
    else if(strContains(argv[i], "draw_frame=")) {
      string draw_frame = tokStringParse(argi, "draw_frame", '#', '=');
      gui->viewer->setParam("draw_frame", draw_frame);
    }
    else if(strContains(argv[i], "draw_base=")) {
      string draw_base = tokStringParse(argi, "draw_base", '#', '=');
      gui->viewer->setParam("draw_base", draw_base);
    }
    else if(strContains(argv[i], "frame_height=")) {
      double frame_height = tokDoubleParse(argi, "frame_height", '#', '=');
      gui->viewer->setParam("set_frame_height", frame_height);
    }
    else if(strContains(argv[i], "set_x_rotation=")) {
      double xrot = tokDoubleParse(argi, "set_x_rotation", '#', '=');
      gui->viewer->setParam("set_x_rotation", xrot);
    }
    else if(strContains(argv[i], "set_z_rotation=")) {
      double zrot = tokDoubleParse(argi, "set_z_rotation", '#', '=');
      gui->viewer->setParam("set_z_rotation", zrot);
    }
  }

  return Fl::run();
}
