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
  int bcol_width = 100;
  int bcol_pos = w()-bcol_width;
  
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

  int a_top = h()-300;
  int a_txt = 25;
  int a_hgt = 75;
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


  int s_top = d_top + d_hgt + 1;
  int s_hgt = 75;
    m_station_box_body = new Fl_Box(FL_BORDER_BOX, col_b+2, s_top, 
				  wid_b-4, s_hgt, 0);
  m_station_box_body->color(FL_DARK_GREEN);
  m_station_all_on  = new MY_Button(col_b+10, s_top+10, 
				    wid_b-20, 25, "STATION ON");
  m_station_all_off = new MY_Button(col_b+10, s_top+40, 
				    wid_b-20, 25, "STATION OFF");




  int ca_top = h()-600;
  int ca_txt = 25;
  int ca_hgt = 75;
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


  int cs_top = cd_top + cd_hgt + 1;
  int cs_hgt = 75;
  m_cstation_box_body = new Fl_Box(FL_BORDER_BOX, col_b+2, cs_top, 
				  wid_b-4, cs_hgt, 0);
  m_cstation_box_body->color(FL_DARK_BLUE);
  m_station_cur_on  = new MY_Button(col_b+10, cs_top+10, 
				    wid_b-20, 25, "STATION ON");
  m_station_cur_off = new MY_Button(col_b+10, cs_top+40, 
				    wid_b-20, 25, "STATION OFF");


  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Procedure: addContactButton

void SSV_GUI::addContactButton(int index, string vname)
{
  vname = toupper(stripBlankEnds(vname));

  int bcol_width = 100;
  int bcol_pos = w()-bcol_width;

  if(index == 0)
    contact_b1->copy_label(vname.c_str());
  if(index == 1)
    contact_b2->copy_label(vname.c_str());
  if(index == 2)
    contact_b3->copy_label(vname.c_str());
  if(index == 3)
    contact_b4->copy_label(vname.c_str());
  if(index == 4)
    contact_b5->copy_label(vname.c_str());
  if(index == 5)
    contact_b6->copy_label(vname.c_str());
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
};


//----------------------------------------------------------
// Procedure: handle
//      Note: As it stands, this method could be eliminated entirely, and the 
//            default behavior of the parent class should work fine. But if
//            we want to tinker with event handling, this method is the place.

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

  if(vname == "") {
    x_mtr->value(" n/a");
    y_mtr->value(" n/a");
    v_spd->value(" n/a");
    v_crs->value(" n/a");
    v_dep->value(" n/a");
    v_ais->value(" n/a");
    v_nam->value(" n/a");
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

  double age_ais = mviewer->getAgeAIS(index);
  string ais_str = doubleToString(age_ais,3);
  if(age_ais == -1)
    ais_str = "n/a";
  v_ais->value(ais_str.c_str());
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
  mviewer->setParam("radial_size", (float)(val));
  mviewer->redraw();
  updateXY();
}

void SSV_GUI::cb_Radial(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((SSV_GUI*)(o->parent()->user_data()))->cb_Radial_i(val);
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

