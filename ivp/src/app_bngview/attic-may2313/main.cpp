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
#include "BNG_GUI.h"
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
  Fl::add_idle(idleProc);

  BNG_GUI* gui = new BNG_GUI(1400, 800, "MIT BNGViewer");

  //string tif_file = "Pianosa.tif";  // default
  string tif_file = "AerialMIT.tif";  // default
  
  int i;
  for(i=1; i<argc; i++) {
    string argi  = argv[i];

    string argi_copy  = argv[i];
    string argi_value = biteString(argi_copy, '=');
    double dval = atof(argi_value.c_str());
    
    if(strBegins(argi, "--file="))
      gui->m_vehi_pair.initFromFile(argi_value);
    
    else if(strBegins(argi, "--osx=")) 
      gui->m_vehi_pair.set_osx(dval);
    else if(strBegins(argi, "--osy=")) 
      gui->m_vehi_pair.set_osy(dval);
    else if(strBegins(argi, "--osh="))
      gui->m_vehi_pair.set_osh(dval);
    else if(strBegins(argi, "--osv="))
      gui->m_vehi_pair.set_osv(dval);
    
    else if(strBegins(argi, "--cnx=")) 
      gui->m_vehi_pair.set_cnx(dval);
    else if(strBegins(argi, "--cny=")) 
      gui->m_vehi_pair.set_cny(dval);
    else if(strBegins(argi, "--cnh="))
      gui->m_vehi_pair.set_cnh(dval);
    else if(strBegins(argi, "--cnv="))
      gui->m_vehi_pair.set_cnv(dval);

    else if(strContains(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
  }
  
  gui->cpa_viewer->setParam("tiff_file", tif_file);
  gui->updateXY();
  
  return Fl::run();
}


