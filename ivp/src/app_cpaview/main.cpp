/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 12, 2007                                         */
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

