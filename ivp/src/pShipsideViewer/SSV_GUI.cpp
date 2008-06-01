/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_GUI.cpp                                          */
/*    DATE: September 11th, 2007                                 */
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

#include "SSV_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

SSV_GUI::SSV_GUI(int g_w, int g_h, const char *g_l)
  : MarineVehiGUI(g_w, g_h, g_l) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  augmentMenu();

  int info_size=12;
  //int bcol_width = 100;
  //int bcol_pos = w()-bcol_width;
  
  int wid_b = 150;
  int col_b = w() - wid_b;

  mviewer  = new SSV_Viewer(0, 30, w()-wid_b, h()-100);
  cmviewer = mviewer;

  ownship_b0 = new MY_Button(col_b+2, 30, wid_b-4, 28, "ownship");
  contact_b1 = new MY_Button(col_b+2, 60, wid_b-4, 28, "  ---  ");
  contact_b2 = new MY_Button(col_b+2, 90, wid_b-4, 28, "  ---  ");
  contact_b3 = new MY_Button(col_b+2,120, wid_b-4, 28, "  ---  ");
  contact_b4 = new MY_Button(col_b+2,150, wid_b-4, 28, "  ---  ");
  contact_b5 = new MY_Button(col_b+2,180, wid_b-4, 28, "  ---  ");
  contact_b6 = new MY_Button(col_b+2,210, wid_b-4, 28, "  ---  ");

  ownship_b0->callback((Fl_Callback*)SSV_GUI::cb_ButtonView,(void*)0);
  contact_b1->callback((Fl_Callback*)SSV_GUI::cb_ButtonView,(void*)1);
  contact_b2->callback((Fl_Callback*)SSV_GUI::cb_ButtonView,(void*)2);
  contact_b3->callback((Fl_Callback*)SSV_GUI::cb_ButtonView,(void*)3);
  contact_b4->callback((Fl_Callback*)SSV_GUI::cb_ButtonView,(void*)4);
  contact_b5->callback((Fl_Callback*)SSV_GUI::cb_ButtonView,(void*)5);
  contact_b6->callback((Fl_Callback*)SSV_GUI::cb_ButtonView,(void*)6);

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

  v_spd = new MY_Output(350, h()-60, 55, 20, "Speed:"); 
  v_spd->textsize(info_size); 
  v_spd->labelsize(info_size);

  v_crs = new MY_Output(350, h()-30, 55, 20, "Heading:"); 
  v_crs->textsize(info_size); 
  v_crs->labelsize(info_size);

  v_dep = new MY_Output(500, h()-60, 55, 20, "Depth:"); 
  v_dep->textsize(info_size); 
  v_dep->labelsize(info_size);

  v_ais = new MY_Output(500, h()-30, 55, 20, "Age-AIS:"); 
  v_ais->textsize(info_size); 
  v_ais->labelsize(info_size);

  v_lat = new MY_Output(600, h()-60, 80, 20, "Lat:"); 
  v_lat->textsize(info_size); 
  v_lat->labelsize(info_size);

  v_lon = new MY_Output(600, h()-30, 80, 20, "Lon:"); 
  v_lon->textsize(info_size); 
  v_lon->labelsize(info_size);

  d_hash = new MY_Output(750, h()-30, 55, 20, "Hash:"); 
  d_hash->textsize(info_size); 
  d_hash->labelsize(info_size);

  d_radial = new MY_Output(750, h()-60, 55, 20, "Radial:"); 
  d_radial->textsize(info_size); 
  d_radial->labelsize(info_size);

  int a_top = h()-323; // eyeballing it
  int a_txt = 25;
  //int a_hgt = 75;
  m_deploy_box_text = new Fl_Box(FL_BORDER_BOX, col_b+2, a_top, 
				 wid_b-4, a_txt, "All Vehicles");
  m_deploy_box_text->color(FL_DARK_GREEN);
  
  int d_top = a_top + a_txt + 1;
  int d_hgt = 75;
  m_deploy_box_body = new Fl_Box(FL_BORDER_BOX, col_b+2, d_top, 
				 wid_b-4, d_hgt, 0);
  m_deploy_box_body->color(FL_DARK_GREEN);
  m_deploy_all_on  = new MY_Button(col_b+10, d_top+10, 
				   wid_b-20, 25, "DEPLOY ON");
  m_deploy_all_off = new MY_Button(col_b+10, d_top+40, 
				   wid_b-20, 25, "DEPLOY OFF");
  
  m_deploy_all_on->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)0);
  m_deploy_all_off->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)1);
  
  int s_top = d_top + d_hgt + 1;
  int s_hgt = 75;
  m_station_box_body = new Fl_Box(FL_BORDER_BOX, col_b+2, s_top, 
				  wid_b-4, s_hgt, 0);
  m_station_box_body->color(FL_DARK_GREEN);
  m_station_all_on  = new MY_Button(col_b+10, s_top+10, 
				    wid_b-20, 25, "STATION ON");
  m_station_all_off = new MY_Button(col_b+10, s_top+40, 
				    wid_b-20, 25, "STATION OFF");
  m_station_all_on->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)2);
  m_station_all_off->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)3);
  
  int e_top = s_top + s_hgt + 1;
  int e_hgt = 75;
  m_engage_box_body = new Fl_Box(FL_BORDER_BOX, col_b+2, e_top, 
				 wid_b-4, e_hgt, 0);
  m_engage_box_body->color(FL_DARK_GREEN);
  m_engage_all_ok = new MY_Button(col_b+10, e_top+10, 
				   wid_b-20, 25, "ENGAGE OK");
  m_engage_all_off = new MY_Button(col_b+10, e_top+40, 
				   wid_b-20, 25, "ENGAGE OFF");
  m_engage_all_ok->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)4);
  m_engage_all_off->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)5);
  
  


  int ca_top = h()-600;
  int ca_txt = 25;
  //int ca_hgt = 75;
  m_cdeploy_box_text = new Fl_Box(FL_BORDER_BOX, col_b+2, ca_top, 
				  wid_b-4, ca_txt, "Current Vehicle");
  m_cdeploy_box_text->color(FL_DARK_BLUE);
  m_cdeploy_box_text->labelcolor(FL_WHITE);
  
  int cd_top = ca_top + ca_txt + 1;
  int cd_hgt = 75;
  m_cdeploy_box_body = new Fl_Box(FL_BORDER_BOX, col_b+2, cd_top, 
				  wid_b-4, cd_hgt, 0);
  m_cdeploy_box_body->color(FL_DARK_BLUE);
  m_deploy_cur_on  = new MY_Button(col_b+10, cd_top+10, 
				   wid_b-20, 25, "DEPLOY ON");
  m_deploy_cur_off = new MY_Button(col_b+10, cd_top+40, 
				   wid_b-20, 25, "DEPLOY OFF");
  m_deploy_cur_on->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)20);
  m_deploy_cur_off->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)21);
  


  int cs_top = cd_top + cd_hgt + 1;
  int cs_hgt = 75;
  m_cstation_box_body = new Fl_Box(FL_BORDER_BOX, col_b+2, cs_top, 
				  wid_b-4, cs_hgt, 0);
  m_cstation_box_body->color(FL_DARK_BLUE);
  m_station_cur_on  = new MY_Button(col_b+10, cs_top+10, 
				    wid_b-20, 25, "STATION ON");
  m_station_cur_off = new MY_Button(col_b+10, cs_top+40, 
				    wid_b-20, 25, "STATION OFF");
  m_station_cur_on->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)22);
  m_station_cur_off->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)23);
  

  int ce_top = cs_top + cs_hgt + 1;
  int ce_hgt = 75;
  m_cengage_box_body = new Fl_Box(FL_BORDER_BOX, col_b+2, ce_top, 
				  wid_b-4, ce_hgt, 0);
  m_cengage_box_body->color(FL_DARK_BLUE);
  m_engage_cur_ok  = new MY_Button(col_b+10, ce_top+10, 
				    wid_b-20, 25, "ENGAGE OK");
  m_engage_cur_off = new MY_Button(col_b+10, ce_top+40, 
				   wid_b-20, 25, "ENGAGE OFF");
  m_engage_cur_ok->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)24);
  m_engage_cur_off->callback((Fl_Callback*)SSV_GUI::cb_MOOS_Button,(void*)25);
  
  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Procedure: addContactButton

void SSV_GUI::addContactButton(int index, string vname)
{
  vname = toupper(stripBlankEnds(vname));

  //int bcol_width = 100;
  //int bcol_pos = w()-bcol_width;

  if(index == 0)
    contact_b1->copy_label(vname.c_str());
  else if(index == 1)
    contact_b2->copy_label(vname.c_str());
  else if(index == 2)
    contact_b3->copy_label(vname.c_str());
  else if(index == 3)
    contact_b4->copy_label(vname.c_str());
  else if(index == 4)
    contact_b5->copy_label(vname.c_str());
  else if(index == 5)
    contact_b6->copy_label(vname.c_str());
}

//-------------------------------------------------------------------
// Procedure: getPendingVar

string SSV_GUI::getPendingVar(unsigned int ix)
{
  if((ix >=0) && (ix < m_pending_vars.size()))
    return(m_pending_vars[ix]);
  else
    return("");
}

//-------------------------------------------------------------------
// Procedure: getPendingVal

string SSV_GUI::getPendingVal(unsigned int ix)
{
  if((ix >=0) && (ix < m_pending_vals.size()))
    return(m_pending_vals[ix]);
  else
    return("");
}

//-------------------------------------------------------------------
// Procedure: clearPending

void SSV_GUI::clearPending()
{
  m_ssv_mutex.Lock();

  m_pending_vars.clear();
  m_pending_vals.clear();

  m_ssv_mutex.UnLock();
}

//-------------------------------------------------------------------
// Procedure: pushPending

void SSV_GUI::pushPending(string var, string val)
{
  m_ssv_mutex.Lock();

  m_pending_vars.push_back(var);
  m_pending_vals.push_back(val);

  m_ssv_mutex.UnLock();
}

//-------------------------------------------------------------------
// Procedure: augmentMenu()

void SSV_GUI::augmentMenu()
{
  mbar->add("Shipside/ShipCentric Toggle",'s',(Fl_Callback*)SSV_GUI::cb_CentricToggle,(void*)0, 0);
  mbar->add("Shipside/ShipCentric Off",    0, (Fl_Callback*)SSV_GUI::cb_CentricToggle,(void*)1, 0);
  mbar->add("Shipside/ShipCentric On",     0, (Fl_Callback*)SSV_GUI::cb_CentricToggle,(void*)2, FL_MENU_DIVIDER);
  mbar->add("Shipside/Radial Off",   0, (Fl_Callback*)SSV_GUI::cb_Radial,(void*)0, 0);
  mbar->add("Shipside/Radial  100",  0, (Fl_Callback*)SSV_GUI::cb_Radial,(void*)100, 0);
  mbar->add("Shipside/Radial  200",  0, (Fl_Callback*)SSV_GUI::cb_Radial,(void*)200, 0);
  mbar->add("Shipside/Radial  500",  0, (Fl_Callback*)SSV_GUI::cb_Radial,(void*)500, 0);
  mbar->add("Shipside/Radial 1000",  0, (Fl_Callback*)SSV_GUI::cb_Radial,(void*)1000, 0);
  mbar->add("Shipside/Radial Cycle", 'r', (Fl_Callback*)SSV_GUI::cb_Radial,(void*)-1, 0);

  mbar->add("ForeView/Cycle Focus", 'v', (Fl_Callback*)SSV_GUI::cb_CycleFocus,(void*)0, 0);

};


//----------------------------------------------------------
// Procedure: handle
//    As it stands, this method could be eliminated entirely, and the 
//    default behavior of the parent class should work fine. But if
//    we want to tinker with event handling, this method is the place.

int SSV_GUI::handle(int event) 
{
  return(Fl_Window::handle(event));
}

//----------------------------------------- UpdateXY
void SSV_GUI::updateXY() {
  int    index = mviewer->getDataIndex();
  string vname = mviewer->getVehiName(index);
  v_nam->value(vname.c_str());
  if(vname == "")
    m_cdeploy_box_text->copy_label("- none -");
  else
    m_cdeploy_box_text->copy_label(vname.c_str());
  
  double curr_time = mviewer->getTime();
  string time_str = doubleToString(curr_time, 1);
  time->value(time_str.c_str());

  double hash = mviewer->getHashDelta();
  string hash_str = doubleToString(hash,1);
  d_hash->value(hash_str.c_str());
  
  if(mviewer->hasVehiName("ownship")) {
    double radial_size = mviewer->getRadialSize();
    string radial_str = doubleToString(radial_size,1);
    d_radial->value(radial_str.c_str());
    d_radial->value(radial_str.c_str());
  }
  else
    d_radial->value(" /na");
    

  if(vname == "") {
    x_mtr->value(" n/a");
    y_mtr->value(" n/a");
    v_spd->value(" n/a");
    v_crs->value(" n/a");
    v_dep->value(" n/a");
    v_ais->value(" n/a");
    v_nam->value(" n/a");
    v_lat->value(" n/a");
    v_lon->value(" n/a");
    return;
  }

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

  double age_ais = mviewer->getAgeAIS(index);
  string ais_str = doubleToString(age_ais,3);
  if(age_ais == -1)
    ais_str = "n/a";
  v_ais->value(ais_str.c_str());

  if(mviewer->hasVehiName(ownship_b0->label()))
    ownship_b0->labelcolor(FL_BLACK);
  else
    ownship_b0->labelcolor(FL_DARK_RED);

  updateButtonColor(ownship_b0);
  updateButtonColor(contact_b1);
  updateButtonColor(contact_b2);
  updateButtonColor(contact_b3);
  updateButtonColor(contact_b4);
  updateButtonColor(contact_b5);
  updateButtonColor(contact_b6);
}

//----------------------------------------- UpdateButtonColor
void SSV_GUI::updateButtonColor(MY_Button* b) {

  if(mviewer->hasVehiName(b->label()))
    b->labelcolor(FL_BLACK);
  else
    b->labelcolor(FL_WHITE);
  b->redraw();
}

//----------------------------------------- CentricToggle
inline void SSV_GUI::cb_CentricToggle_i(int val) {
  if(val == 0)
    mviewer->setParam("centric_view", "toggle");
  if(val == 1)
    mviewer->setParam("centric_view", "off");
  if(val == 2)
    mviewer->setParam("centric_view", "on");
  mviewer->redraw();
  updateXY();
}
void SSV_GUI::cb_CentricToggle(Fl_Widget* o, int val) {
  ((SSV_GUI*)(o->parent()->user_data()))->cb_CentricToggle_i(val);
}

//----------------------------------------- Radial
inline void SSV_GUI::cb_Radial_i(int val) {
  if(val >= 0)
    mviewer->setParam("radial_size", (float)(val));
  else
    mviewer->setParam("radial_increment", (float)(val));
  mviewer->redraw();
  updateXY();
}

void SSV_GUI::cb_Radial(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((SSV_GUI*)(o->parent()->user_data()))->cb_Radial_i(val);
}

//----------------------------------------- CycleFocus
inline void SSV_GUI::cb_CycleFocus_i(int val) {
  mviewer->cycleIndex();
  mviewer->redraw();
  updateXY();
}

void SSV_GUI::cb_CycleFocus(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((SSV_GUI*)(o->parent()->user_data()))->cb_CycleFocus_i(val);
}

//----------------------------------------- MOOS_Button
inline void SSV_GUI::cb_MOOS_Button_i(int val) {
  
  string vname = toupper(mviewer->getCurrVName());

  if(val == 0)
    pushPending("DEPLOY_ALL", "true");
  else if(val == 1)
    pushPending("DEPLOY_ALL", "false");
  else if(val == 2)
    pushPending("STATION_KEEP_ALL", "true");
  else if(val == 3)
    pushPending("STATION_KEEP_ALL", "false");
  else if(val == 4)
    pushPending("ENGAGE_ALL", "true");
  else if(val == 5)
    pushPending("ENGAGE_ALL", "false");

  else if(val == 20)
    pushPending("DEPLOY_"+vname, "true");
  else if(val == 21)
    pushPending("DEPLOY_"+vname, "false");
  else if(val == 22)
    pushPending("STATION_KEEP_"+vname, "true");
  else if(val == 23)
    pushPending("STATION_KEEP_"+vname, "false");
  else if(val == 24)
    pushPending("ENGAGE_"+vname, "true");
  else if(val == 25)
    pushPending("ENGAGE_"+vname, "false");

}

void SSV_GUI::cb_MOOS_Button(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((SSV_GUI*)(o->parent()->user_data()))->cb_MOOS_Button_i(val);
}


//----------------------------------------- ButtonView
inline void SSV_GUI::cb_ButtonView_i(int val) {
  string blabel = "";

  if(val == 0) 
    blabel = "ownship";
  if(val == 1)
    blabel = contact_b1->label();
  if(val == 2) 
    blabel = contact_b2->label();
  if(val == 3) 
    blabel = contact_b3->label();
  if(val == 4) 
    blabel = contact_b4->label();
  if(val == 5) 
    blabel = contact_b5->label();
  if(val == 6) 
    blabel = contact_b6->label();

  mviewer->setCurrent(blabel);
  mviewer->redraw();
  updateXY();
}

void SSV_GUI::cb_ButtonView(Fl_Widget* o, int v) {
  ((SSV_GUI*)(o->parent()->user_data()))->cb_ButtonView_i(v);
}

