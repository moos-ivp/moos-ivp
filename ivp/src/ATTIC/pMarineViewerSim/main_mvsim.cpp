/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main_mvsim.cpp                                       */
/*    DATE: March 25th 2005                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "MVS_GUI.h"
#include "Populator_SimMod.h"
#include "Populator_SSR.h"
#include "MBUtils.h"
#include "IO_GeomUtils.h"

using namespace std;

MVS_GUI *gui = 0;

vector<XYGrid> getGridsFromSSR(SSR *ssr);

//--------------------------------------------------------
// Procedure: usage

void exit_with_usage()
{
  cout << "Usage: mvsim file.ssr [file.tif] [-noimg]" << endl << flush;
  exit(-1);
}


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
  int i;

  SIM*         sim;
  IB_HelmIvP*  infobuff;

  string ssr_file;
  string tif_file = "AerialMIT.tif";

  for(i=1; i<argc; i++) {
    string argi  = argv[i];
    
    if(strContains(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
    else if(strContains(argi, ".ssr"))
      ssr_file = argi;
    else 
      exit_with_usage();
  }
  
  if(ssr_file == "")
    exit_with_usage();
  

  Populator_SSR  pop_ssr; 

  bool ok = pop_ssr.populate(ssr_file.c_str());
  if(!ok) {
    cout << "Problem with SSR file " << ssr_file << endl;
    exit(-1);
  }
  SSR *ssr = pop_ssr.getSSR();

  Populator_SimMod pop_sm(ssr);
  pop_sm.populate();
  
  Fl::add_idle(idleProc);

  gui  = new MVS_GUI(900, 800, "MarineViewerSim");
  sim  = pop_sm.getSIM();
  infobuff = pop_sm.getIB();

  gui->set_sim(sim);
  gui->set_ibuff(infobuff);
  gui->readTiff(tif_file.c_str());

  vector<XYGrid> grid_vector = getGridsFromSSR(ssr);
  for(i=0; i<grid_vector.size(); i++) 
    gui->addGrid(grid_vector[i]);

  cout << "Total Grids found: " << grid_vector.size() << endl;

  return Fl::run();
}



//--------------------------------------------------------
// Procedure: getGridsFromSSR

vector<XYGrid> getGridsFromSSR(SSR *ssr)
{
  vector<XYGrid> ret_grid_vector;
  vector<XYGrid> one_grid_vector;

  int ssr_size = ssr->getCount();
  for(int i=0; i<ssr_size; i++) {
    string bhv_file = ssr->get_h_bhv(i);
    one_grid_vector = readGridsFromFile(bhv_file);
    for(int j=0; j<one_grid_vector.size(); j++)
      ret_grid_vector.push_back(one_grid_vector[j]);
  }
  return(ret_grid_vector);
}




