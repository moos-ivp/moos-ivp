/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PMV_GUI.cpp                                          */
/*    DATE: November, 2004                                       */
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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "PMV_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

PMV_GUI::PMV_GUI(int g_w, int g_h, const char *g_l)
  : CommonMarineSimGUI(g_w, g_h, g_l) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  augmentMenu();

  int info_size=12;
 
  curr_time = 0;

  mviewer  = new MarineViewer(0, 30, w(), h()-100);
  cmviewer = mviewer;

  v_nam = new MY_Output(60, h()-60, 70, 20, "VName:"); 
  v_nam->textsize(info_size); 
  v_nam->labelsize(info_size);

  time = new MY_Output(60, h()-30, 70, 20, "Time:"); 
  time->textsize(info_size); 
  time->labelsize(info_size);

  x_mtr = new MY_Output(200, h()-60, 70, 20, "meters x:"); 
  x_mtr->textsize(info_size); 
  x_mtr->labelsize(info_size);

  y_mtr = new MY_Output(200, h()-30, 70, 20, "meters y:"); 
  y_mtr->textsize(info_size); 
  y_mtr->labelsize(info_size);

  x_lon = new MY_Output(350, h()-60, 70, 20, "longitude:"); 
  x_lon->textsize(info_size); 
  x_lon->labelsize(info_size);

  y_lat = new MY_Output(350, h()-30, 70, 20, "latitude:"); 
  y_lat->textsize(info_size); 
  y_lat->labelsize(info_size);

  v_spd = new MY_Output(500, h()-60, 70, 20, "Speed:"); 
  v_spd->textsize(info_size); 
  v_spd->labelsize(info_size);

  v_crs = new MY_Output(500, h()-30, 70, 20, "Heading:"); 
  v_crs->textsize(info_size); 
  v_crs->labelsize(info_size);

  v_dep = new MY_Output(650, h()-60, 70, 20, "Depth:"); 
  v_dep->textsize(info_size); 
  v_dep->labelsize(info_size);

  //b_items = new MY_Hold_Browser(620, h()-80, 265, 70); 
  //b_items->textsize(info_size); 
  //b_items->labelsize(info_size);
  //b_items->callback((Fl_Callback*)PMV_GUI::cb_ItemSelect);

  mbar->add("ForeView/Grids Layer", FL_CTRL+'g', 
	    (Fl_Callback*)PMV_GUI::cb_ToggleGridsTop, 
	    (void*)0, FL_MENU_RADIO|FL_MENU_VALUE);

  //updateInfoBHV();

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Procedure: augmentMenu()

void PMV_GUI::augmentMenu()
{
  //  mbar->add("Replay/Collecting Toggle",  'w', (Fl_Callback*)REPLAY_GUI::cb_CollectToggle,(void*)0, FL_MENU_DIVIDER);
};

//----------------------------------------------------------
// Procedure: handle
//      Note: As it stands, this method could be eliminated entirely, and the 
//            default behavior of the parent class should work fine. But if
//            we want to tinker with event handling, this method is the place.

int PMV_GUI::handle(int event) 
{
  return(Fl_Window::handle(event));
}

//----------------------------------------- ToggleGridsTop
inline void PMV_GUI::cb_ToggleGridsTop_i() {
  mviewer->toggleGridsTop();
}
void PMV_GUI::cb_ToggleGridsTop(Fl_Widget* o) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_ToggleGridsTop_i();
}

//----------------------------------------- UpdateXY
void PMV_GUI::updateXY() {
  int  index = mviewer->getDataIndex();
  char buff[64];

  //sprintf(buff, "%f", mviewer->getImgPctX(index));
  //x_img->value(buff);
  //sprintf(buff, "%f", mviewer->getImgPctY(index));
  //y_img->value(buff);

  string mtrx_str = doubleToString(mviewer->getMetersX(index),2);
  x_mtr->value(mtrx_str.c_str());
  string mtry_str = doubleToString(mviewer->getMetersY(index),2);
  y_mtr->value(mtry_str.c_str());

  string lon_str = doubleToString(mviewer->getLonX(index),4);
  x_lon->value(lon_str.c_str());
  string lat_str = doubleToString(mviewer->getLatY(index),4);
  y_lat->value(lat_str.c_str());

  string spd_str = doubleToString(mviewer->getSpd(index),2);
  v_spd->value(spd_str.c_str());
  string crs_str = doubleToString(mviewer->getCrs(index),2);
  v_crs->value(crs_str.c_str());
  string dep_str = doubleToString(mviewer->getDep(index),3);
  v_dep->value(dep_str.c_str());

  string time_str = doubleToString(curr_time, 2);
  time->value(time_str.c_str());
  std::string str = mviewer->getKName(index);
  v_nam->value(str.c_str());
}


