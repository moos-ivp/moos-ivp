/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 12, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <cstdlib>
#include <vector>
#include <iostream>
#include "CPA_GUI.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  int i;
  Fl::add_idle(idleProc);

  CPA_GUI* gui = new CPA_GUI(1400, 800, "CPAViewer");

  //string tif_file = "Pianosa.tif";  // default
  string tif_file = "AerialMIT.tif";  // default
  
  for(i=1; i<argc; i++) {
    string argi  = argv[i];
    
    if(strBegins(argi, "--file=")) {
      biteString(argi, '=');
      gui->cpa_model->initFromFile(argi);
    }
    else if(strBegins(argi, "cnx=")) {
      biteString(argi, '=');
      gui->cpa_model->set_cnx(atof(argi.c_str()));
    }
    else if(strBegins(argi, "cny=")) {
      biteString(argi, '=');
      gui->cpa_model->set_cny(atof(argi.c_str()));
    }
    else if(strBegins(argi, "cn_rb=")) {
      biteString(argi, '=');
      gui->cpa_model->set_cn_range_bearing(argi);
    }
    else if(strBegins(argi, "cn_hdg=")) {
      biteString(argi, '=');
      gui->cpa_model->set_cncrs(atof(argi.c_str()));
    }
    else if(strBegins(argi, "cn_spd=")) {
      biteString(argi, '=');
      gui->cpa_model->set_cnspd(atof(argi.c_str()));
    }
    else if(strContains(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
  }
  
  gui->cpa_viewer->setParam("tiff_file", tif_file);
  gui->updateXY();
  
  return Fl::run();
}


