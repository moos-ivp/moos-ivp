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

#include "PMV_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

PMV_GUI::PMV_GUI(int g_w, int g_h, const char *g_l)
  : MarineVehiGUI(g_w, g_h, g_l) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  augmentMenu();
  
  int info_size=12;
 
  m_curr_time = 0;

  mviewer  = new PMV_Viewer(0, 30, w(), h()-100);
  cmviewer = mviewer;

  v_nam = new MY_Output(60, h()-60, 70, 20, "VName:"); 
  v_nam->textsize(info_size); 
  v_nam->labelsize(info_size);

  v_typ = new MY_Output(60, h()-30, 70, 20, "VType:"); 
  v_typ->textsize(info_size); 
  v_typ->labelsize(info_size);

  x_mtr = new MY_Output(190, h()-60, 70, 20, "X(m):"); 
  x_mtr->textsize(info_size); 
  x_mtr->labelsize(info_size);

  y_mtr = new MY_Output(190, h()-30, 70, 20, "Y(m):"); 
  y_mtr->textsize(info_size); 
  y_mtr->labelsize(info_size);

  v_lat = new MY_Output(305, h()-60, 90, 20, "Lat:"); 
  v_lat->textsize(info_size); 
  v_lat->labelsize(info_size);

  v_lon = new MY_Output(305, h()-30, 90, 20, "long:"); 
  v_lon->textsize(info_size); 
  v_lon->labelsize(info_size);

  v_spd = new MY_Output(470, h()-60, 55, 20, "Spd(m/s):"); 
  v_spd->textsize(info_size); 
  v_spd->labelsize(info_size);

  v_crs = new MY_Output(470, h()-30, 55, 20, "Heading:"); 
  v_crs->textsize(info_size); 
  v_crs->labelsize(info_size);

  v_dep = new MY_Output(610, h()-60, 55, 20, "Dep(m):"); 
  v_dep->textsize(info_size); 
  v_dep->labelsize(info_size);

  v_ais = new MY_Output(610, h()-30, 55, 20, "Age-AIS:"); 
  v_ais->textsize(info_size); 
  v_ais->labelsize(info_size);

  time = new MY_Output(720, h()-60, 70, 20, "Time:"); 
  time->textsize(info_size); 
  time->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}



//-------------------------------------------------------------------
// Procedure: augmentMenu()

void PMV_GUI::augmentMenu()
{
  mbar->add("ForeView/Cycle Focus", 'v', (Fl_Callback*)PMV_GUI::cb_CycleFocus,(void*)0, 0);
}


//----------------------------------------------------------
// Procedure: handle
//      Note: As it stands, this method could be eliminated entirely, and the 
//            default behavior of the parent class should work fine. But if
//            we want to tinker with event handling, this method is the place.

int PMV_GUI::handle(int event) 
{
  return(Fl_Window::handle(event));
}

//----------------------------------------- UpdateXY
void PMV_GUI::updateXY() {
  int  index = mviewer->getDataIndex();

  string time_str = doubleToString(m_curr_time, 1);
  time->value(time_str.c_str());

  string vname = mviewer->getVehiName(index);

  if(vname == "") {
    v_nam->value(" n/a");
    x_mtr->value(" n/a");
    y_mtr->value(" n/a");
    v_spd->value(" n/a");
    v_crs->value(" n/a");
    v_lat->value(" n/a");
    v_lon->value(" n/a");
    v_dep->value(" n/a");
    v_ais->value(" n/a");
    return;
  }

  v_nam->value(vname.c_str());

  string vtype = mviewer->getVehiType(index);
  v_typ->value(vtype.c_str());

  string lat_str = "??";
  string lon_str = "??";
  double dlat, dlon;
  bool ok = mviewer->getLatLon(index, dlat, dlon);
  if(ok) {
    lat_str = doubleToString(dlat,6);
    lon_str = doubleToString(dlon,6);
  }
  v_lat->value(lat_str.c_str());
  v_lon->value(lon_str.c_str());

  string mtrx_str = doubleToString(mviewer->getMetersX(index),1);
  x_mtr->value(mtrx_str.c_str());
  string mtry_str = doubleToString(mviewer->getMetersY(index),1);
  y_mtr->value(mtry_str.c_str());

  string spd_str = doubleToString(mviewer->getSpd(index),1);
  v_spd->value(spd_str.c_str());
  string crs_str = doubleToString(mviewer->getCrs(index),1);
  v_crs->value(crs_str.c_str());
  string dep_str = doubleToString(mviewer->getDep(index),1);
  v_dep->value(dep_str.c_str());

  double age_ais = mviewer->getAgeAIS(index);
  string ais_str = doubleToString(age_ais,3);
  if(age_ais == -1)
    ais_str = "n/a";
  v_ais->value(ais_str.c_str());
}


//----------------------------------------- CycleFocus
inline void PMV_GUI::cb_CycleFocus_i(int val) {
  mviewer->cycleIndex();
  mviewer->redraw();
  updateXY();
}

void PMV_GUI::cb_CycleFocus(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((PMV_GUI*)(o->parent()->user_data()))->cb_CycleFocus_i(val);
}
