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
#include "MOOSGenLibGlobalHelper.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

PMV_GUI::PMV_GUI(int g_w, int g_h, const char *g_l)
  : MarineVehiGUI(g_w, g_h, g_l) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  int info_size=12;
  
  m_trail_color_ix = 0;
  m_curr_time      = 0;

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

  warp = new MY_Output(720, h()-30, 70, 20, "Warp:"); 
  warp->textsize(info_size); 
  warp->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
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
  string time_str = doubleToString(m_curr_time, 1);
  time->value(time_str.c_str());

  string vname = mviewer->getStringInfo("active_vehicle_name");

  if(vname == "") {
    v_nam->value(" n/a");
    v_typ->value(" n/a");
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

  double dwarp = GetMOOSTimeWarp();
  string swarp = dstringCompact(doubleToString(dwarp,2));
  warp->value(swarp.c_str());

  v_nam->value(vname.c_str());

  string vtype = mviewer->getStringInfo("body");
  v_typ->value(vtype.c_str());

  string xpos = mviewer->getStringInfo("xpos", 1);
  x_mtr->value(xpos.c_str());

  string ypos = mviewer->getStringInfo("ypos", 1);
  y_mtr->value(ypos.c_str());
  
  string lat = mviewer->getStringInfo("lat", 6);
  v_lat->value(lat.c_str());

  string lon = mviewer->getStringInfo("lon", 6);
  v_lon->value(lon.c_str());

  string spd = mviewer->getStringInfo("speed", 1);
  v_spd->value(spd.c_str());

  string crs = mviewer->getStringInfo("course", 1);
  v_crs->value(crs.c_str());

  string dep = mviewer->getStringInfo("depth", 1);
  v_dep->value(dep.c_str());
  
  string age_ais = mviewer->getStringInfo("age_ais", 2);
  if(age_ais == "-1")
    age_ais = "n/a";
  v_ais->value(age_ais.c_str());
}

