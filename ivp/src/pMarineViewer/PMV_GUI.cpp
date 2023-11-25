/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMV_GUI.cpp                                          */
/*    DATE: November, 2004                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <FL/fl_ask.H>
#include "MOOS/libMOOS/MOOSLib.h"
#include "PMV_GUI.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

PMV_GUI::PMV_GUI(int g_w, int g_h, const char *g_l)
  : MarineVehiGUI(g_w, g_h, g_l) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(600,400);

  m_start_hgt = g_h;
  m_start_wid = g_w;

  m_cmd_gui = 0;
  m_curr_time = 0;
  m_clear_stale_timestamp = 0;

  m_show_title_ip = true;
  m_show_title_user = true;
  m_show_title_mhash = true;
  
  m_cmd_gui_start_wid = 850;
  m_cmd_gui_start_hgt = 600;
  m_cmd_gui_start_bpost_cnt = 0;
  m_cmd_gui_start_cpost_cnt = 0;
  m_cmd_gui_start_show_posts = true;

  m_button_cols = 0;
  
  mviewer   = new PMV_Viewer(0, 0, 1, 1);
  m_mviewer = mviewer;

  // Configure the AppCasting Browsing Widgets
  m_brw_nodes = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_brw_nodes->callback(cb_SelectAppCastNode, 0);
  m_brw_nodes->textfont(FL_COURIER);
  m_brw_nodes->box(FL_DOWN_BOX);
  m_brw_nodes->clear_visible_focus();

  m_brw_procs = new MY_Fl_Hold_Browser(0, 0, 1, 1); 
  m_brw_procs->callback(cb_SelectAppCastProc, 0);
  m_brw_procs->textfont(FL_COURIER);
  m_brw_procs->box(FL_DOWN_BOX);
  m_brw_procs->clear_visible_focus();

  m_brw_casts = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_brw_casts->clear_visible_focus();
  m_brw_casts->textfont(FL_COURIER);
  m_brw_casts->box(FL_DOWN_BOX);
  m_brw_casts->clear_visible_focus();
  
  // Configure the RealmCasting Browsing Widgets
  m_rc_brw_nodes = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_rc_brw_nodes->callback(cb_SelectRealmCastNode, 0);
  m_rc_brw_nodes->textfont(FL_COURIER);
  m_rc_brw_nodes->box(FL_DOWN_BOX);
  m_rc_brw_nodes->clear_visible_focus();

  m_rc_brw_procs = new MY_Fl_Hold_Browser(0, 0, 1, 1); 
  m_rc_brw_procs->callback(cb_SelectRealmCastProc, 0);
  m_rc_brw_procs->textfont(FL_COURIER);
  m_rc_brw_procs->box(FL_DOWN_BOX);
  m_rc_brw_procs->clear_visible_focus();

  m_rc_brw_casts = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_rc_brw_casts->clear_visible_focus();
  m_rc_brw_casts->textfont(FL_COURIER);
  m_rc_brw_casts->box(FL_DOWN_BOX);
  m_rc_brw_casts->clear_visible_focus();
  
  // Configure the DataField Widgets  
  v_nam = new Fl_Output(0, 0, 1, 1, "VName:"); 
  v_typ = new Fl_Output(0, 0, 1, 1, "VType:"); 
  x_mtr = new Fl_Output(0, 0, 1, 1, "X(m):"); 
  y_mtr = new Fl_Output(0, 0, 1, 1, "Y(m):"); 
  v_lat = new Fl_Output(0, 0, 1, 1, "Lat:"); 
  v_lon = new Fl_Output(0, 0, 1, 1, "Lon:"); 
  v_spd = new Fl_Output(0, 0, 1, 1, "Spd:"); 
  v_crs = new Fl_Output(0, 0, 1, 1, "Hdg:"); 
  v_dep = new Fl_Output(0, 0, 1, 1, "Dep(m):"); 
  v_ais = new Fl_Output(0, 0, 1, 1, "Age(s):"); 
  time  = new Fl_Output(0, 0, 1, 1, "Time:"); 
  warp  = new Fl_Output(0, 0, 1, 1, "Warp:"); 

  v_nam->set_output();
  v_typ->set_output();
  x_mtr->set_output();
  y_mtr->set_output();
  v_lat->set_output();
  v_lon->set_output();
  v_spd->set_output();
  v_crs->set_output();
  v_dep->set_output();
  v_ais->set_output();
  time->set_output();
  warp->set_output();

  m_scope_variable = new Fl_Output(0, 0, 1, 1, "Variable:"); 
  m_scope_time = new Fl_Output(0, 0, 1, 1, "Tm:"); 
  m_scope_value = new Fl_Output(0, 0, 1, 1, "Value:"); 
  m_scope_variable->set_output();
  m_scope_time->set_output();
  m_scope_value->set_output();
  
  m_rc_button_src = new Fl_Button(0, 0, 1, 1, "SRC");
  m_rc_button_com = new Fl_Button(0, 0, 1, 1, "COM");
  m_rc_button_utc = new Fl_Button(0, 0, 1, 1, "UTC");
  m_rc_button_wrp = new Fl_Button(0, 0, 1, 1, "WRP");
  m_rc_button_sub = new Fl_Button(0, 0, 1, 1, "SUBS");
  m_rc_button_msk = new Fl_Button(0, 0, 1, 1, "MASK");
  m_rc_button_trc = new Fl_Button(0, 0, 1, 1, "TRUNC");

  m_user_button_1 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_2 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_3 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_4 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_5 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_6 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_7 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_8 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_9 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_10 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_11 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_12 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_13 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_14 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_15 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_16 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_17 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_18 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_19 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_20 = new Fl_Button(0, 0, 1, 1, "Disabled");

  m_buttons.push_back(m_user_button_1);
  m_buttons.push_back(m_user_button_2);
  m_buttons.push_back(m_user_button_3);
  m_buttons.push_back(m_user_button_4);
  m_buttons.push_back(m_user_button_5);
  m_buttons.push_back(m_user_button_6);
  m_buttons.push_back(m_user_button_7);
  m_buttons.push_back(m_user_button_8);
  m_buttons.push_back(m_user_button_9);
  m_buttons.push_back(m_user_button_10);
  m_buttons.push_back(m_user_button_11);
  m_buttons.push_back(m_user_button_12);
  m_buttons.push_back(m_user_button_13);
  m_buttons.push_back(m_user_button_14);
  m_buttons.push_back(m_user_button_15);
  m_buttons.push_back(m_user_button_16);
  m_buttons.push_back(m_user_button_17);
  m_buttons.push_back(m_user_button_18);
  m_buttons.push_back(m_user_button_19);
  m_buttons.push_back(m_user_button_20);

  m_buttons.push_back(m_rc_button_src);
  m_buttons.push_back(m_rc_button_com);
  m_buttons.push_back(m_rc_button_utc);
  m_buttons.push_back(m_rc_button_wrp);

  m_buttons.push_back(m_rc_button_sub);
  m_buttons.push_back(m_rc_button_msk);
  m_buttons.push_back(m_rc_button_trc);

  
  for(unsigned int i=0; i<m_buttons.size(); i++)
    m_buttons[i]->clear_visible_focus();

  m_rc_button_utc->callback((Fl_Callback*)PMV_GUI::cb_REALM_Button,(void*)24);
  m_rc_button_src->callback((Fl_Callback*)PMV_GUI::cb_REALM_Button,(void*)25);
  m_rc_button_com->callback((Fl_Callback*)PMV_GUI::cb_REALM_Button,(void*)26);
  m_rc_button_wrp->callback((Fl_Callback*)PMV_GUI::cb_REALM_Button,(void*)27);

  m_rc_button_sub->callback((Fl_Callback*)PMV_GUI::cb_REALM_Button,(void*)28);
  m_rc_button_msk->callback((Fl_Callback*)PMV_GUI::cb_REALM_Button,(void*)29);
  m_rc_button_trc->callback((Fl_Callback*)PMV_GUI::cb_REALM_Button,(void*)30);

  m_user_button_1->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)1);
  m_user_button_2->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)2);
  m_user_button_3->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)3);
  m_user_button_4->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)4);
  m_user_button_5->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)5);
  m_user_button_6->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)6);
  m_user_button_7->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)7);  
  m_user_button_8->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)8);  
  m_user_button_9->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)9);  
  m_user_button_10->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)10);  
  m_user_button_11->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)11);  
  m_user_button_12->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)12);  
  m_user_button_13->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)13);  
  m_user_button_14->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)14);  
  m_user_button_15->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)15);  
  m_user_button_16->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)16);  
  m_user_button_17->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)17);  
  m_user_button_18->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)18);  
  m_user_button_19->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)19);  
  m_user_button_20->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)20);  
  
  for(unsigned int i=0; i<m_buttons.size(); i++)
    m_buttons[i]->clear_visible_focus();

  augmentMenu();

  this->end();
  this->resizable(this);
  this->show();
  resizeWidgets();
}

//----------------------------------------------------------
// Procedure: augmentMenu()

void PMV_GUI::augmentMenu()
{
  m_menubar->add("BackView/full_screen=true", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)30,
		 FL_MENU_RADIO);
  m_menubar->add("BackView/full_screen=false", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)31,
		 FL_MENU_RADIO);
  m_menubar->add("BackView/    Toggle Full-Screen", 'f',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)32,
		 FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/infocast_viewable=true", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)40,
		 FL_MENU_RADIO);    
  m_menubar->add("InfoCasting/infocast_viewable=false", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)41,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/    Toggle InfoCasting", FL_CTRL+'a',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)42,
		 FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/content_mode=appcast", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)45,
		 FL_MENU_RADIO);    
  m_menubar->add("InfoCasting/content_mode=realmcast", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)46,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/  Toggle InfoCast Content", 'a',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)47,
		 FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/refresh_mode=paused",    FL_CTRL+' ',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)50,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/refresh_mode=events",    FL_CTRL+'e',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)51,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/refresh_mode=streaming", FL_CTRL+'s',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)52,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/nodes_font_size=xlarge",  0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)64,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/nodes_font_size=large",  0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)63,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/nodes_font_size=medium", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)62,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/nodes_font_size=small",  0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)61,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/nodes_font_size=xsmall", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)60,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  m_menubar->add("InfoCasting/procs_font_size=xlarge", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)74,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/procs_font_size=large",  0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)73,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/procs_font_size=medium", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)72,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/procs_font_size=small",  0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)71,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/procs_font_size=xsmall", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)70,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  m_menubar->add("InfoCasting/infocast_font_size=xlarge", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)84,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/infocast_font_size=large",  0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)83,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/infocast_font_size=medium", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)82,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/infocast_font_size=small",  0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)81,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/infocast_font_size=xsmall", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)80,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/infocast_font_size bigger",  '}',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)89,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/infocast_font_size smaller", '{',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)88,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  m_menubar->add("InfoCasting/appcast_color_scheme=white",     0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)300,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/appcast_color_scheme=indigo",      0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)301,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/appcast_color_scheme=beige",       0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)302,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/appcast_color_scheme Toggle", FL_ALT+'a',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)310,
		 FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/realmcast_color_scheme=white", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)305,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_color_scheme=indigo", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)306,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_color_scheme=beige", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)307,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_color_scheme=hillside", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)308,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_color_scheme Toggle", FL_SHIFT+FL_ALT+'a',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)309,
		 FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/InfoCast Pane Thinner",   FL_SHIFT+FL_Left,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)100, 0); 
  m_menubar->add("InfoCasting/InfoCast Pane Wider", FL_SHIFT+FL_Right,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)101, 0); 

  m_menubar->add("InfoCasting/InfoCast Pane Taller",  FL_SHIFT+FL_Up,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)200, 0); 
  m_menubar->add("InfoCasting/InfoCast Pane Shorter", FL_SHIFT+FL_Down,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)201,
		 FL_MENU_DIVIDER); 

  m_menubar->add("InfoCasting/InfoCast Pane Reset Size", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)102,
		 FL_MENU_DIVIDER); 

  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=20", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)120,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=25", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)125,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=30", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)130,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=35", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)135,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=40", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)140,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=45", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)145,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=50", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)150,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=55", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)155,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=60", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)160,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=65", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)165,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Width/infocast_width=70", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)170,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  m_menubar->add("InfoCasting/InfoCast Pane Height/reset", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)102,
		 FL_MENU_DIVIDER); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=90", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)290,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=85", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)285,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=80", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)280,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=75", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)275,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=70", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)270,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=65", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)265,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=60", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)260,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=55", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)255,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=50", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)250,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=45", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)245,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=40", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)240,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=35", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)235,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/InfoCast Pane Height/infocast_height=30", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)230,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  m_menubar->add("InfoCasting/realmcast_show_source/realmcast_show_source=true", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)320,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_source/realmcast_show_source=false", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)321,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_source/realmcast_show_source Toggle", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)322,
		 FL_MENU_RADIO);

  m_menubar->add("InfoCasting/realmcast_show_community/realmcast_show_community=true", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)330,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_community/realmcast_show_community=false", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)331,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_community/realmcast_show_community Toggle", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)332,
		 FL_MENU_RADIO);

  m_menubar->add("InfoCasting/realmcast_show_subscriptions/realmcast_show_subscriptions=true", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)340,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_subscriptions/realmcast_show_subscriptions=false", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)341,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_subscriptions/realmcast_show_subscriptions Toggle", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)342,
		 FL_MENU_RADIO);

  
  m_menubar->add("InfoCasting/realmcast_show_masked/realmcast_show_masked=true", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)350,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_masked/realmcast_show_masked=false", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)351,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_masked/realmcast_show_masked Toggle", 'k',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)352,
		 FL_MENU_RADIO);

  m_menubar->add("InfoCasting/realmcast_wrap_content/realmcast_wrap_content=true", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)360,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_wrap_content/realmcast_wrap_content=false", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)361,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_wrap_content/realmcast_wrap_content Toggle", 'w',
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)362,
		 FL_MENU_RADIO);

  m_menubar->add("InfoCasting/realmcast_trunc_content/realmcast_trunc_content=true", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)370,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_trunc_content/realmcast_trunc_content=false", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)371,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_trunc_content/realmcast_trunc_content Toggle", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)372,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/realmcast_time_format_utc/realmcast_time_format_utc=true", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)380,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_time_format_utc/realmcast_time_format_utc=false", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)381,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_time_format_utc/realmcast_time_format_utc Toggle", 0,
		 (Fl_Callback*)PMV_GUI::cb_InfoCastSetting, (void*)382,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  
  m_menubar->add("MOOS-Scope/Add Variable", 'A',
		 (Fl_Callback*)PMV_GUI::cb_Scope, (void*)0,
		 FL_MENU_DIVIDER);
    
  m_menubar->add("Vehicles/ClearHistory/All Vehicles", FL_CTRL+'9',
		 (Fl_Callback*)PMV_GUI::cb_FilterOut, (void*)-1,
		 FL_MENU_DIVIDER);
  m_menubar->add("Vehicles/ClearHistory/Selected", FL_CTRL+'k',
		 (Fl_Callback*)PMV_GUI::cb_DeleteActiveNode, (void*)0,
		 FL_MENU_DIVIDER);
  m_menubar->add("Vehicles/ClearHistory/Reset All", FL_ALT+'k',
		 (Fl_Callback*)PMV_GUI::cb_DeleteActiveNode, (void*)1,
		 FL_MENU_DIVIDER);

  m_menubar->add("Action/Open Command GUI", ' ',
		 (Fl_Callback*)PMV_GUI::cb_CommandGUI, (void*)0,
		 FL_MENU_DIVIDER);
}

//----------------------------------------------------------
// Procedure: resize()

void PMV_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgets();
}

//----------------------------------------------------------
// Procedure: setVerbose()

void PMV_GUI::setVerbose(bool bval)
{
  if(m_mviewer)
    m_mviewer->setVerbose(bval);
}

//----------------------------------------------------------
// Procedure: calcButtonColumns()

void PMV_GUI::calcButtonColumns()
{
  string label_1  = m_user_button_1->label();
  string label_2  = m_user_button_2->label();
  string label_3  = m_user_button_3->label();
  string label_4  = m_user_button_4->label();
  string label_5  = m_user_button_5->label();
  string label_6  = m_user_button_6->label();
  string label_7  = m_user_button_7->label();
  string label_8  = m_user_button_8->label();
  string label_9  = m_user_button_9->label();
  string label_10 = m_user_button_10->label();
  string label_11 = m_user_button_11->label();
  string label_12 = m_user_button_12->label();
  string label_13 = m_user_button_13->label();
  string label_14 = m_user_button_14->label();
  string label_15 = m_user_button_15->label();
  string label_16 = m_user_button_16->label();
  string label_17 = m_user_button_17->label();
  string label_18 = m_user_button_18->label();
  string label_19 = m_user_button_19->label();
  string label_20 = m_user_button_20->label();
  
  if((label_20 != "Disabled") || (label_19 != "Disabled"))
    m_button_cols = 10;
  else if((label_18 != "Disabled") || (label_17 != "Disabled"))
    m_button_cols = 9;
  else if((label_16 != "Disabled") || (label_15 != "Disabled"))
    m_button_cols = 8;
  else if((label_14 != "Disabled") || (label_13 != "Disabled"))
    m_button_cols = 7;
  else if((label_12 != "Disabled") || (label_11 != "Disabled"))
    m_button_cols = 6;
  else if((label_10 != "Disabled") || (label_9 != "Disabled"))
    m_button_cols = 5;
  else if((label_8 != "Disabled") || (label_7 != "Disabled"))
    m_button_cols = 4;
  else if((label_6 != "Disabled") || (label_5 != "Disabled"))
    m_button_cols = 3;

  else if((label_4 != "Disabled") || (label_3 != "Disabled"))
    m_button_cols = 2;
  else if((label_2 != "Disabled") || (label_1 != "Disabled"))
    m_button_cols = 1;
  else 
    m_button_cols = 0;
}

//----------------------------------------------------------
// Procedure: addButtonAction()

string PMV_GUI::getButtonAction(string btype) const
{
  string action_str;
  for(unsigned int i=0; i<m_button_keys.size(); i++) {
    if(m_button_keys[i] == btype) {
      if(action_str != "")
	action_str += " # ";
      action_str += m_button_vars[i] + "=";
      action_str += m_button_vals[i];
    }
  }

  return(action_str);
}

//----------------------------------------------------------
// Procedure: addButton()

bool PMV_GUI::addButton(string btype, string svalue) 
{
  Fl_Button *button = 0;
  if(btype == "button_one") button = m_user_button_1;
  else if(btype == "button_two")      button = m_user_button_2;
  else if(btype == "button_three")    button = m_user_button_3;
  else if(btype == "button_four")     button = m_user_button_4;
  else if(btype == "button_five")     button = m_user_button_5;
  else if(btype == "button_six")      button = m_user_button_6;
  else if(btype == "button_seven")    button = m_user_button_7;
  else if(btype == "button_eight")    button = m_user_button_8;
  else if(btype == "button_nine")     button = m_user_button_9;
  else if(btype == "button_ten")      button = m_user_button_10;
  else if(btype == "button_eleven")   button = m_user_button_11;
  else if(btype == "button_twelve")   button = m_user_button_12;
  else if(btype == "button_thirteen") button = m_user_button_13;
  else if(btype == "button_fourteen") button = m_user_button_14;
  else if(btype == "button_fifteen")  button = m_user_button_15;
  else if(btype == "button_sixteen")  button = m_user_button_16;
  else if(btype == "button_seventeen") button = m_user_button_17;
  else if(btype == "button_eighteen") button = m_user_button_18;
  else if(btype == "button_nineteen") button = m_user_button_19;
  else if(btype == "button_twenty")   button = m_user_button_20;
  else
    return(false);

  // Set the default label if none is provided in the svalue.
  // The default is the *current* value of the label.
  string button_label = button->label();
  string bcolor;
  
  bool ok_line = true;
  vector<string> svector = parseStringQ(svalue, '#');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = stripBlankEnds(biteString(svector[i], '='));
    string value = stripBlankEnds(svector[i]);
    if(param == "") 
      ok_line = false;
    else if(param == "color") 
      bcolor = value;
    else {
      if(value == "")
        button_label = param;
      else {
        m_button_keys.push_back(btype);
        m_button_vars.push_back(param);
        m_button_vals.push_back(value);
      }
    }
  }
  
  button->copy_label(button_label.c_str());
  button->show();

  string tooltip = getButtonAction(btype);
  button->copy_tooltip(tooltip.c_str());

  if(isColor(bcolor)) {
    ColorPack cpack(bcolor);
    int red_int = cpack.red() * 255;
    int grn_int = cpack.grn() * 255;
    int blu_int = cpack.blu() * 255;
    Fl_Color fcolor = fl_rgb_color(red_int, grn_int, blu_int);
    button->color(fcolor);
  }
  button->redraw();
  
  calcButtonColumns();
  resizeWidgets();
  return(ok_line);
}

//----------------------------------------------------------
// Procedure: addAction()

bool PMV_GUI::addAction(string svalue, bool separator) 
{
  // We call the Q-version of parseString to allow separators in vals
  vector<string> svector = parseStringQ(svalue, '#');
  unsigned int i, vsize = svector.size();
  unsigned int pindex = m_action_vars.size();

  vector<string> vars, vals;
  string key = "";
  for(i=0; i<vsize; i++) {
    string param = stripBlankEnds(biteString(svector[i], '='));
    string value = stripBlankEnds(svector[i]);
    if(isQuoted(value))
      value = stripQuotes(value);
    if(param == "")
      return(false);
    if(tolower(param)=="menu_key") 
      key = value;
    else {
      vars.push_back(param);
      vals.push_back(value);
    }
  }
  
  unsigned int psize = vars.size();
  for(i=0; i<psize; i++) {
    m_action_vars.push_back(vars[i]);
    m_action_vals.push_back(vals[i]);
    m_action_keys.push_back(key);
  }

  for(i=0; i<psize; i++) {
    // Use special unsigned int type having same size as pointer (void*)
    uintptr_t index = pindex + i;
    string left  = truncString(vars[i], 25, "middle");
    string right = vals[i];
    //string right = truncString(vals[i], 25, "middle");
    string label = ("Action/" + left + "=" + right);
    label = padString(label, 25, false);
    if(key != "")
      label += (" <" + key + ">");
    if(separator)
      m_menubar->add(label.c_str(), 0, (Fl_Callback*)PMV_GUI::cb_DoAction, (void*)index, FL_MENU_DIVIDER);
    else
      m_menubar->add(label.c_str(), 0, (Fl_Callback*)PMV_GUI::cb_DoAction, (void*)index, 0);
  }
  m_menubar->redraw();
  return(true);
}

//----------------------------------------------------------
// Procedure: handle()
//      Note: As it stands, this method could be eliminated
//            entirely, and the default behavior of the parent
//            class should work fine. But if we want to tinker
//            with event handling, this method is the place.

int PMV_GUI::handle(int event) 
{
  return(Fl_Window::handle(event));
}

//----------------------------------------------------------
// Procedure: showingInfoCasts()

bool PMV_GUI::showingInfoCasts() const
{
  if(m_icast_settings.getContentMode() == "off")
    return(false);

  return(true);
}

//----------------------------------------------------------
// Procedure:  augmentTitleWithIP()

bool PMV_GUI::augmentTitleWithIP(string ip_str)
{
  if(!isValidIPAddress(ip_str))
    return(false);

  m_title_ip = ip_str;
  refreshTitle();
  return(true);
}

//----------------------------------------------------------
// Procedure:  augmentTitleWithMHash()

void PMV_GUI::augmentTitleWithMHash(string mhash)
{
  if(mhash.length() != 9) 
    return;
  m_title_mhash = mhash;
  refreshTitle();
}

//----------------------------------------------------------
// Procedure:  augmentTitleWithUser()

void PMV_GUI::augmentTitleWithUser(string user)
{
  m_title_user = user;
  refreshTitle();
}

//----------------------------------------------------------
// Procedure:  refreshTitle()

void PMV_GUI::refreshTitle()
{
  string new_title = m_title_base;

  if(m_show_title_user && (m_title_user != ""))
    new_title += " [" + m_title_user + "]";
  if(m_show_title_mhash && (m_title_mhash != ""))
    new_title += " [" + m_title_mhash + "]";
  if(m_show_title_ip && (m_title_ip != ""))
    new_title += " [" + m_title_ip + "]";

  label(new_title.c_str());
}

//----------------------------------------------------
// Procedure: syncNodesAtoB()
//   Purpose: Used when the user toggles the "active vehicle"
//            shown in the panes at the bottom of the window.
//            If the new active vehicle is also a known appcast
//            node, adjust the appcast browsers accordingly.

bool PMV_GUI::syncNodesAtoB()
{
  string vname = mviewer->getStringInfo("active_vehicle_name");

  bool valid_node = m_repo->setCurrentNode(vname);
  if(!valid_node)
    return(false);

  updateAppCastNodes(true);
  updateAppCastProcs(true);
  updateAppCast();
  return(true);
}

//----------------------------------------------------
// Procedure: syncNodesBtoA()
//   Purpose: Used when user selects a new node in the
//            appcast browsers. If the new node is a known
//            vehicle, i.e., we have received node reports
//            for it, then make it the active vehicle in 
//            the panes at the bottom of the window. 

bool PMV_GUI::syncNodesBtoA()
{
  string curr_node = m_repo->getCurrentNode();
  if(curr_node == "")
    return(false);

  //  vector<string> vnames = m_vehiset.getVehiNames();
  //if(vectorContains(vnames, curr_node))
    mviewer->setParam("active_vehicle_name", curr_node);

  return(true);
}

//----------------------------------------------------
// Procedure: clearGeoShapes()

void PMV_GUI::clearGeoShapes(string vname, string shape, string stype)
{
  mviewer->clearGeoShapes(vname, shape, stype);
}

//----------------------------------------- UpdateXY
void PMV_GUI::updateXY() 
{
  if(m_icast_settings.getFullScreen())
    return;

  
  double dwarp = GetMOOSTimeWarp();
  string time_str = doubleToString(m_curr_time, 1);

  string swarp      = dstringCompact(doubleToString(dwarp,2));
  string scope_var  = mviewer->getStringInfo("scope_var");
  string scope_time = mviewer->getStringInfo("scope_time");
  string scope_val  = mviewer->getStringInfo("scope_val");
  string vname = mviewer->getStringInfo("active_vehicle_name");

  m_scope_variable->value(scope_var.c_str());
  m_scope_time->value(scope_time.c_str());
  m_scope_value->value(scope_val.c_str());
  
  if((vname == "") || (vname == "error")) {
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
    warp->value(swarp.c_str());
    //v_range->value(" n/a");
    //v_bearing->value(" n/a");
    return;
  }

  string vtype = mviewer->getStringInfo("body");
  string xpos = mviewer->getStringInfo("xpos", 1);
  string ypos = mviewer->getStringInfo("ypos", 1);
  string lat = mviewer->getStringInfo("lat", 6);
  string lon = mviewer->getStringInfo("lon", 6);
  string spd = mviewer->getStringInfo("speed", 1);
  string crs = mviewer->getStringInfo("course", 1);
  string dep = mviewer->getStringInfo("depth", 1);
  string age_ais = mviewer->getStringInfo("age_ais", 2);
  string range = mviewer->getStringInfo("range", 1);
  string bearing = mviewer->getStringInfo("bearing", 2);
  if(age_ais == "-1")
    age_ais = "n/a";

  v_nam->value(vname.c_str());
  v_typ->value(vtype.c_str());
  x_mtr->value(xpos.c_str());
  y_mtr->value(ypos.c_str());
  v_lat->value(lat.c_str());
  v_lon->value(lon.c_str());
  v_spd->value(spd.c_str());
  v_crs->value(crs.c_str());
  v_dep->value(dep.c_str());
  v_ais->value(age_ais.c_str());

  time->value(time_str.c_str());
  warp->value(swarp.c_str());

  //v_range->value(range.c_str());
  //v_bearing->value(bearing.c_str());

  time->redraw();
  warp->redraw();
  v_nam->redraw();
  v_typ->redraw();
  v_spd->redraw();
  v_dep->redraw();
  v_ais->redraw();
}



//----------------------------------------- REALM_Button
inline void PMV_GUI::cb_REALM_Button_i(unsigned int val) {  

  if(val == 24)
    setRadioCastAttrib("realmcast_time_format_utc", "toggle");
  else if(val == 25)
    setRadioCastAttrib("realmcast_show_source", "toggle");
  else if(val == 26)
    setRadioCastAttrib("realmcast_show_community", "toggle");
  else if(val == 27)
    setRadioCastAttrib("realmcast_wrap_content", "toggle");
  else if(val == 28) 
    setRadioCastAttrib("realmcast_show_subscriptions", "toggle");
  else if(val == 29)
    setRadioCastAttrib("realmcast_show_masked", "toggle");
  else if(val == 30) 
    setRadioCastAttrib("realmcast_trunc_content", "toggle");
}

void PMV_GUI::cb_REALM_Button(Fl_Widget* o, unsigned int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_REALM_Button_i(v);
}


//----------------------------------------- MOOS_Button
inline void PMV_GUI::cb_MOOS_Button_i(unsigned int val) {  

  string skey = "";
  if(val == 1)
    skey = "button_one";
  else if(val == 2)
    skey = "button_two";
  else if(val == 3)
    skey = "button_three";
  else if(val == 4)
    skey = "button_four";
  else if(val == 5)
    skey = "button_five";
  else if(val == 6)
    skey = "button_six";
  else if(val == 7)
    skey = "button_seven";
  else if(val == 8)
    skey = "button_eight";
  else if(val == 9)
    skey = "button_nine";
  else if(val == 10)
    skey = "button_ten";
  else if(val == 11)
    skey = "button_eleven";
  else if(val == 12)
    skey = "button_twelve";
  else if(val == 13)
    skey = "button_thirteen";
  else if(val == 14)
    skey = "button_fourteen";
  else if(val ==15 )
    skey = "button_fifteen";
  else if(val == 16)
    skey = "button_sixteen";
  else if(val == 17)
    skey = "button_seventeen";
  else if(val == 18)
    skey = "button_eighteen";
  else if(val == 19)
    skey = "button_nineteen";
  else if(val == 20)
    skey = "button_twenty";
  
  unsigned int i, vsize = m_button_keys.size();
  for(i=0; i<vsize; i++) {
    if(m_button_keys[i] == skey)  {
      pushPending(m_button_vars[i], m_button_vals[i]);
    }
  }
}

void PMV_GUI::cb_MOOS_Button(Fl_Widget* o, unsigned int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_MOOS_Button_i(v);
}


//----------------------------------------- DoAction
inline void PMV_GUI::cb_DoAction_i(unsigned int i) {  
  string key = m_action_keys[i];
  if(key == "") {
    cout << m_action_vars[i] << " = " << m_action_vals[i] << endl;
    pushPending(m_action_vars[i], m_action_vals[i]);
  }
  else {
    unsigned int j, vsize = m_action_vars.size();
    for(j=0; j<vsize; j++) {
      if(m_action_keys[j] == key) {
        cout << m_action_vars[j] << " = " << m_action_vals[j] << endl;
        pushPending(m_action_vars[j], m_action_vals[j]);
      }
    }
  }
}

void PMV_GUI::cb_DoAction(Fl_Widget* o, unsigned int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_DoAction_i(v);
}


//----------------------------------------- Scope
inline void PMV_GUI::cb_Scope_i(unsigned int i) {  
  if(i==0) {
    const char *str = fl_input("Enter a new MOOS variable for scoping:", 
			       "DB_CLIENTS");
    if(str != 0) {
      string new_var = str;
      new_var = stripBlankEnds(new_var);
      if(!strContainsWhite(new_var)) {
	bool added = addScopeVariable(new_var);
	if(added) {
	  mviewer->addScopeVariable(new_var);
	  pushPending("scope_register", new_var);
	}
	if(mviewer->isScopeVariable(new_var))
	  mviewer->setActiveScope(new_var);
      }
    }
    return;
  }

  if(i>=m_scope_vars.size())
    return;
  
  string varname = m_scope_vars[i];
  mviewer->setActiveScope(varname);
}

void PMV_GUI::cb_Scope(Fl_Widget* o, unsigned int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_Scope_i(v);
}

//----------------------------------------- InfoCastSetting
inline void PMV_GUI::cb_InfoCastSetting_i(unsigned int v) {  
  // Reset all infocast panes to orig proportions
  if(v==102)  {
    setRadioCastAttrib("infocast_height", "70");
    setRadioCastAttrib("infocast_width",  "30");
  }
  // Handle fullscreen mode switching
  else if(v==30)  setRadioCastAttrib("full_screen", "true");
  else if(v==31)  setRadioCastAttrib("full_screen", "false");
  else if(v==32)  setRadioCastAttrib("full_screen", "toggle");
  // Handle infocast view mode switching
  else if(v==40)  setRadioCastAttrib("infocast_viewable", "true");
  else if(v==41)  setRadioCastAttrib("infocast_viewable", "false");
  else if(v==42)  setRadioCastAttrib("infocast_viewable", "toggle");
  // Handle infocast view mode switching
  else if(v==45)  setRadioCastAttrib("content_mode", "appcast");
  else if(v==46)  setRadioCastAttrib("content_mode", "realmcast");
  else if(v==47)  setRadioCastAttrib("content_mode", "toggle");
  // Handle infocast refresh setting
  else if(v==50)  setRadioCastAttrib("refresh_mode", "paused");
  else if(v==51)  setRadioCastAttrib("refresh_mode", "events");
  else if(v==52)  setRadioCastAttrib("refresh_mode", "streaming");  
  // Handle node browser pane font size
  else if(v==60)  setRadioCastAttrib("nodes_font_size", "xsmall");
  else if(v==61)  setRadioCastAttrib("nodes_font_size", "small");
  else if(v==62)  setRadioCastAttrib("nodes_font_size", "medium");
  else if(v==63)  setRadioCastAttrib("nodes_font_size", "large");
  else if(v==64)  setRadioCastAttrib("nodes_font_size", "xlarge");
  // Handle procs browser pane font size
  else if(v==70)  setRadioCastAttrib("procs_font_size", "xsmall");
  else if(v==71)  setRadioCastAttrib("procs_font_size", "small");
  else if(v==72)  setRadioCastAttrib("procs_font_size", "medium");
  else if(v==73)  setRadioCastAttrib("procs_font_size", "large");
  else if(v==74)  setRadioCastAttrib("procs_font_size", "xlarge");
  // Handle infocast browser pane font size
  else if(v==80)  setRadioCastAttrib("infocast_font_size", "xsmall");
  else if(v==81)  setRadioCastAttrib("infocast_font_size", "small");
  else if(v==82)  setRadioCastAttrib("infocast_font_size", "medium");
  else if(v==83)  setRadioCastAttrib("infocast_font_size", "large");
  else if(v==84)  setRadioCastAttrib("infocast_font_size", "xlarge");
  else if(v==88)  setRadioCastAttrib("infocast_font_size", "smaller");
  else if(v==89)  setRadioCastAttrib("infocast_font_size", "bigger");
  // Handle pane width relative adjustment
  else if(v==100) setRadioCastAttrib("infocast_width", "delta:-5");
  else if(v==101) setRadioCastAttrib("infocast_width", "delta:5");
  // Handle pane width explicit setting
  else if(v==120) setRadioCastAttrib("infocast_width", "20");
  else if(v==125) setRadioCastAttrib("infocast_width", "25");
  else if(v==130) setRadioCastAttrib("infocast_width", "30");
  else if(v==135) setRadioCastAttrib("infocast_width", "35");
  else if(v==140) setRadioCastAttrib("infocast_width", "40");
  else if(v==145) setRadioCastAttrib("infocast_width", "45");
  else if(v==150) setRadioCastAttrib("infocast_width", "50");
  else if(v==155) setRadioCastAttrib("infocast_width", "55");
  else if(v==160) setRadioCastAttrib("infocast_width", "60");
  else if(v==165) setRadioCastAttrib("infocast_width", "65");
  else if(v==170) setRadioCastAttrib("infocast_width", "70");
  // Handle InfoCast pane height relative adjustment
  else if(v==200) setRadioCastAttrib("infocast_height", "delta:5");
  else if(v==201) setRadioCastAttrib("infocast_height", "delta:-5");
  // Handle InfoCast pane height explicit setting
  else if(v==230) setRadioCastAttrib("infocast_height", "30");
  else if(v==235) setRadioCastAttrib("infocast_height", "35");
  else if(v==240) setRadioCastAttrib("infocast_height", "40");
  else if(v==245) setRadioCastAttrib("infocast_height", "45");
  else if(v==250) setRadioCastAttrib("infocast_height", "50");
  else if(v==255) setRadioCastAttrib("infocast_height", "55");
  else if(v==260) setRadioCastAttrib("infocast_height", "60");
  else if(v==265) setRadioCastAttrib("infocast_height", "65");
  else if(v==270) setRadioCastAttrib("infocast_height", "70");
  else if(v==275) setRadioCastAttrib("infocast_height", "75");
  else if(v==280) setRadioCastAttrib("infocast_height", "80");
  else if(v==285) setRadioCastAttrib("infocast_height", "85");
  else if(v==290) setRadioCastAttrib("infocast_height", "90");

  else if(v==300) setRadioCastAttrib("appcast_color_scheme", "white");
  else if(v==301) setRadioCastAttrib("appcast_color_scheme", "indigo");
  else if(v==302) setRadioCastAttrib("appcast_color_scheme", "beige");
  else if(v==310) setRadioCastAttrib("appcast_color_scheme", "toggle");

  else if(v==305) setRadioCastAttrib("realmcast_color_scheme", "white");
  else if(v==306) setRadioCastAttrib("realmcast_color_scheme", "indigo");
  else if(v==307) setRadioCastAttrib("realmcast_color_scheme", "beige");
  else if(v==308) setRadioCastAttrib("realmcast_color_scheme", "hillside");
  else if(v==309) setRadioCastAttrib("realmcast_color_scheme", "toggle");

  else if(v==320) setRadioCastAttrib("realmcast_show_source", "true");
  else if(v==321) setRadioCastAttrib("realmcast_show_source", "false");
  else if(v==322) setRadioCastAttrib("realmcast_show_source", "toggle");

  else if(v==330) setRadioCastAttrib("realmcast_show_community", "true");
  else if(v==331) setRadioCastAttrib("realmcast_show_community", "false");
  else if(v==332) setRadioCastAttrib("realmcast_show_community", "toggle");

  else if(v==340) setRadioCastAttrib("realmcast_show_subscriptions", "true");
  else if(v==341) setRadioCastAttrib("realmcast_show_subscriptions", "false");
  else if(v==342) setRadioCastAttrib("realmcast_show_subscriptions", "toggle");

  else if(v==350) setRadioCastAttrib("realmcast_show_masked", "true");
  else if(v==351) setRadioCastAttrib("realmcast_show_masked", "false");
  else if(v==352) setRadioCastAttrib("realmcast_show_masked", "toggle");

  else if(v==360) setRadioCastAttrib("realmcast_wrap_content", "true");
  else if(v==361) setRadioCastAttrib("realmcast_wrap_content", "false");
  else if(v==362) setRadioCastAttrib("realmcast_wrap_content", "toggle");

  else if(v==370) setRadioCastAttrib("realmcast_trunc_content", "true");
  else if(v==371) setRadioCastAttrib("realmcast_trunc_content", "false");
  else if(v==372) setRadioCastAttrib("realmcast_trunc_content", "toggle");

  else if(v==380) setRadioCastAttrib("realmcast_time_format_utc", "true");
  else if(v==381) setRadioCastAttrib("realmcast_time_format_utc", "false");
  else if(v==382) setRadioCastAttrib("realmcast_time_format_utc", "toggle");
  
  resizeWidgets();
  redraw();
}

void PMV_GUI::cb_InfoCastSetting(Fl_Widget* o, unsigned int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_InfoCastSetting_i(v);
}

//----------------------------------------- LeftContext
inline void PMV_GUI::cb_LeftContext_i(unsigned int i) {  
  if(i>=m_left_mouse_keys.size())
    return;
  string key_str = m_left_mouse_keys[i];
  mviewer->setLeftMouseKey(key_str);
}

void PMV_GUI::cb_LeftContext(Fl_Widget* o, unsigned int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_LeftContext_i(v);
}

//----------------------------------------- RightContext
inline void PMV_GUI::cb_RightContext_i(unsigned int i) {  
  if(i>=m_right_mouse_keys.size())
    return;
  string key_str = m_right_mouse_keys[i];
  mviewer->setRightMouseKey(key_str);
}

void PMV_GUI::cb_RightContext(Fl_Widget* o, unsigned int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_RightContext_i(v);
}

//----------------------------------------- Reference
inline void PMV_GUI::cb_Reference_i(unsigned int i) {  
  if(i>=m_reference_tags.size())
    return;
  string str = m_reference_tags[i];
  mviewer->setParam("reference_tag", str);
}

void PMV_GUI::cb_Reference(Fl_Widget* o, unsigned int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_Reference_i(v);
}

//----------------------------------------- FilterOut
inline void PMV_GUI::cb_FilterOut_i(int i) {  
  if(i >= (int)(m_filter_tags.size()))
    return;
  if(i < 0)
    mviewer->setParam("filter_out_tag", "all");
  else {
    string str = m_filter_tags[i];
    mviewer->setParam("filter_out_tag", str);
    if(m_repo)
      m_repo->removeNode(str);
    updateAppCastNodes(true);
  }
}

void PMV_GUI::cb_FilterOut(Fl_Widget* o, int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_FilterOut_i(v);
}


//----------------------------------------- cb_DeleteActiveNode
inline void PMV_GUI::cb_DeleteActiveNode_i(int i) {  
  if(!m_repo)
    return;
  if(i==0) {
    string active_node = m_repo->getCurrentNode();
    if(active_node == "")
      return;
  
    mviewer->setParam("filter_out_tag", active_node);
    m_repo->removeNode(active_node);
    updateAppCastNodes(true);
    updateAppCastProcs(true);
    m_clear_stale_timestamp = m_curr_time;
  }
  if(i==1)
    clearStaleVehicles(true);
}

void PMV_GUI::cb_DeleteActiveNode(Fl_Widget* o, int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_DeleteActiveNode_i(v);
}

//----------------------------------------- cb_CommandGUI
inline void PMV_GUI::cb_CommandGUI_i()
{
  if(m_cmd_gui) {
    if(m_cmd_gui->isVisible()) {
      m_cmd_gui->makeTop();
      return;
    }
    else
      delete(m_cmd_gui);
  }

  if(m_cmd_folio.size() == 0)
    return;
  
  int  wid = m_cmd_gui_start_wid;
  int  hgt = m_cmd_gui_start_hgt;
  bool posts = m_cmd_gui_start_show_posts;
  
  UCMD_GUI *cmd_gui = new UCMD_GUI(wid, hgt, "Commander", posts);
  cmd_gui->setCommandFolio(m_cmd_folio);
  cmd_gui->setButPostCount(m_cmd_gui_start_bpost_cnt);
  cmd_gui->setCmdPostCount(m_cmd_gui_start_cpost_cnt);

  m_cmd_gui = cmd_gui;

  vector<string> command_report = m_cmd_summary.getCommandReport();
  m_cmd_gui->setPostSummary(command_report);
  
  cout << "Launching Command GUI" << endl;
  m_cmd_folio.print();
}

void PMV_GUI::cb_CommandGUI(Fl_Widget* o) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_CommandGUI_i();
}


//----------------------------------------------------------
// Procedure: clearStaleVehicles()

bool PMV_GUI::clearStaleVehicles(bool force) 
{
  if(!m_repo)
    return(false);

  double stale_report_thresh = mviewer->getStaleReportThresh();
  double stale_remove_thresh = mviewer->getStaleRemoveThresh();

  if(!force && (stale_remove_thresh < 0))
    return(false);

  double total_thresh = stale_report_thresh + stale_remove_thresh;
  if(force)
    total_thresh = stale_report_thresh;

  vector<string> vnames = mviewer->getStaleVehicles(total_thresh);

  for(unsigned int i=0; i<vnames.size(); i++) {
    mviewer->setParam("filter_out_tag", vnames[i]);
    m_repo->removeNode(vnames[i]);
    updateAppCastNodes(true);
    updateAppCastProcs(true);
  }

  if(vnames.size() > 0)
    m_clear_stale_timestamp = m_curr_time;
  return(true);
}



//----------------------------------------------------
// Procedure: cb_SelectAppCastNode_i()

inline void PMV_GUI::cb_SelectAppCastNode_i() 
{
  vector<string> nodes = m_repo->getCurrentNodes();
  if(nodes.size() == 0)
    return;

  int ix = ((Fl_Browser *)m_brw_nodes)->value();

  // If the user clicks below the list of items, treat as if the
  // last item were clicked. Figure out its index and set it.
  if(ix == 0) {
    ix = 2 + (int)(nodes.size()); // Add 2 due to table header lines
    m_brw_nodes->select(ix,1); 
  }

  // If the user clicks one of the two header lines, treat this
  // as a click of the first item in the list.
  if(ix <= 2) {
    ix = 3;
    m_brw_nodes->select(ix,1);
  }

  unsigned int node_ix = ix - 3;
  m_repo->setCurrentNode(nodes[node_ix]);
  updateAppCastNodes();
  updateAppCastProcs(true);
  updateAppCast();
  syncNodesBtoA();
  updateXY();
}

//----------------------------------------------------
// Procedure: cb_SelectAppCastNode()

void PMV_GUI::cb_SelectAppCastNode(Fl_Widget* o, long) 
{
  ((PMV_GUI*)(o->parent()->user_data()))->cb_SelectAppCastNode_i();
}


//----------------------------------------------------
// Procedure: cb_SelectAppCastProci()

inline void PMV_GUI::cb_SelectAppCastProc_i() 
{
  vector<string> procs = m_repo->getCurrentProcs();
  if(procs.size() == 0)
    return;

  int ix = ((Fl_Browser *)m_brw_procs)->value();

  // If the user clicks below the list of items, treat as if the
  // last item were clicked. Figure out its index and set it.
  if(ix == 0) {
    ix = 2 + (int)(procs.size()); // Add 2 due to table header lines
    m_brw_procs->select(ix,1);
  }
  
  // If the user clicks one of the two header lines, treat this
  // as a click of the first item in the list.
  if(ix <= 2) {
    ix = 3;
    m_brw_procs->select(ix,1);
  }

  unsigned int proc_ix = ix - 3;  
  m_repo->setCurrentProc(procs[proc_ix]);

  updateAppCastProcs();
  updateAppCast();
}

//----------------------------------------------------
// Procedure: cb_SelectAppCastProc()

void PMV_GUI::cb_SelectAppCastProc(Fl_Widget* o, long) 
{
  ((PMV_GUI*)(o->parent()->user_data()))->cb_SelectAppCastProc_i();
}


//----------------------------------------------------
// Procedure: cb_SelectRealmCastNode_i()

inline void PMV_GUI::cb_SelectRealmCastNode_i() 
{
  vector<string> nodes = m_rc_repo->getCurrentNodes();
  if(nodes.size() == 0)
    return;

  vector<string> cluster_keys = m_rc_repo->getClusterKeys();

  // Note the first item on the browser list has index=1 (not 0)
  int ix = ((Fl_Browser *)m_rc_brw_nodes)->value();

  // If the user clicked one of the two header lines, treat this
  // as a click of the first item in the list.
  if(ix <= 2) {
    ix = 3;
    m_rc_brw_nodes->select(ix,1);
  }

  string new_node = "error";
  
  unsigned int node_ix = ix - 3;
  if(node_ix < nodes.size())
    new_node = nodes[node_ix];

  else if(cluster_keys.size() > 0) {
    if(ix == (int)(3 + nodes.size())) {
      ix = 4 + nodes.size();
      m_rc_brw_nodes->select(ix,1);
    }
    if(ix >= (int)((nodes.size() + 4))) {
      unsigned int key_ix = ix - (nodes.size() +4);
      new_node = cluster_keys[key_ix];
      m_rc_repo->setForceRefreshWC();
    }
  }

  if(new_node == "error")
    return;

  m_rc_repo->setCurrentNode(new_node);

  updateRealmCastNodes();
  updateRealmCastProcs(true);
  updateRealmCast();
  updateXY();
}


//----------------------------------------------------
// Procedure: cb_SelectRealmCastNode()

void PMV_GUI::cb_SelectRealmCastNode(Fl_Widget* o, long) 
{
  ((PMV_GUI*)(o->parent()->user_data()))->cb_SelectRealmCastNode_i();
}


//----------------------------------------------------
// Procedure: cb_SelectRealmCastProci()

inline void PMV_GUI::cb_SelectRealmCastProc_i() 
{
  vector<string> procs = m_rc_repo->getCurrentProcs();
  if(procs.size() == 0)
    return;

  int ix = ((Fl_Browser *)m_rc_brw_procs)->value();

  // If the user clicks below the list of items, treat as if the
  // last item were clicked. Figure out its index and set it.
  if(ix == 0) {
    ix = 2 + (int)(procs.size()); // Add 2 due to table header lines
    m_rc_brw_procs->select(ix,1);
  }
  
  // If the user clicks one of the two header lines, treat this
  // as a click of the first item in the list.
  if(ix <= 2) {
    ix = 3;
    m_rc_brw_procs->select(ix,1);
  }

  unsigned int proc_ix = ix - 3;  
  m_rc_repo->setCurrentProc(procs[proc_ix]);

  updateRealmCastProcs();
  updateRealmCast();
}

//----------------------------------------------------
// Procedure: cb_SelectRealmCastProc()

void PMV_GUI::cb_SelectRealmCastProc(Fl_Widget* o, long) 
{
  ((PMV_GUI*)(o->parent()->user_data()))->cb_SelectRealmCastProc_i();
}


//-------------------------------------------------------------------
// Procedure: getPendingVar()

string PMV_GUI::getPendingVar(unsigned int ix)
{
  if(ix < m_pending_vars.size())
    return(m_pending_vars[ix]);
  else
    return("");
}

//-------------------------------------------------------------------
// Procedure: getPendingVal()

string PMV_GUI::getPendingVal(unsigned int ix)
{
  if(ix < m_pending_vals.size())
    return(m_pending_vals[ix]);
  else
    return("");
}

//-------------------------------------------------------------------
// Procedure: clearPending()

void PMV_GUI::clearPending()
{
  m_pending_vars.clear();
  m_pending_vals.clear();
}

//-------------------------------------------------------------------
// Procedure: pushPending()

void PMV_GUI::pushPending(string var, string val)
{
  m_pending_vars.push_back(var);
  m_pending_vals.push_back(val);
}

//-------------------------------------------------------------------
// Procedure: addScopeVariable()
//   Returns: false if no variable added
//            true if the given variable is added

bool PMV_GUI::addScopeVariable(string varname)
{
  // We dont allow white space in MOOS variables
  if(strContainsWhite(varname))
    return(false);

  // If the varname is already added, just return false now.
  unsigned int i, vsize = m_scope_vars.size();
  for(i=0; i<vsize; i++) {
    if(varname == m_scope_vars[i])
      return(false);
  }

  // The index/ordering in the m_scope_vars vector is important. It is
  // correlated to the cb_Scope callback index in each of the new menu
  // items. The zero'th index is meaningless since zero is handled 
  // as a special case in cb_Scope. 
  if(vsize == 0) {
    m_scope_vars.push_back("_add_moos_var_");
    m_scope_vars.push_back("_previous_scope_var_");
    m_scope_vars.push_back("_cycle_scope_var_");
    m_menubar->add("MOOS-Scope/Toggle-Previous-Scope", '/', 
	      (Fl_Callback*)PMV_GUI::cb_Scope, (void*)1, FL_MENU_DIVIDER);
    m_menubar->add("MOOS-Scope/Cycle-Scope-Variables", FL_CTRL+'/', 
	      (Fl_Callback*)PMV_GUI::cb_Scope, (void*)2, FL_MENU_DIVIDER);
  }

  m_scope_vars.push_back(varname);
  uintptr_t index = m_scope_vars.size()-1;
  
  string label = "MOOS-Scope/";
  label += (truncString(varname, 25, "middle"));
  m_menubar->add(label.c_str(), 0, 
	    (Fl_Callback*)PMV_GUI::cb_Scope, (void*)index, 0);
  
  m_menubar->redraw();
  return(true);
}

//-------------------------------------------------------------------
// Procedure: addMousePoke()

bool PMV_GUI::addMousePoke(string side, string key, string vardata_pair)
{
  if(!mviewer)
    return(false);
  if(key == "error")
    return(false);

  side = tolower(side);

  if(side == "left") {
    mviewer->addMousePoke(side, key, vardata_pair);
    if(!vectorContains(m_left_mouse_keys, key)) {
      if(m_left_mouse_keys.size() == 0) {
	m_left_mouse_keys.push_back("no-action");
	m_menubar->add("Mouse-Context/Left/no-action", 0, 
		  (Fl_Callback*)PMV_GUI::cb_LeftContext, (void*)0, 
		  FL_MENU_RADIO);
      }
      m_left_mouse_keys.push_back(key);
      uintptr_t index = m_left_mouse_keys.size()-1;
      string label = "Mouse-Context/Left/";
      label += (truncString(key, 40, "middle"));

      // If this was the first left context mode, make it the active
      if(m_left_mouse_keys.size() == 2) {
	mviewer->setLeftMouseKey(key);
	m_menubar->add(label.c_str(), 0, 
		  (Fl_Callback*)PMV_GUI::cb_LeftContext, 
		  (void*)index, FL_MENU_RADIO|FL_MENU_VALUE);
      }
      else {
	m_menubar->add(label.c_str(), 0, (Fl_Callback*)PMV_GUI::cb_LeftContext, 
		       (void*)index, FL_MENU_RADIO);
      }
      m_menubar->redraw();
    }
  }
  else if(side == "right") {
    mviewer->addMousePoke(side, key, vardata_pair);
    if(!vectorContains(m_right_mouse_keys, key)) {
      if(m_right_mouse_keys.size() == 0) {
	m_right_mouse_keys.push_back("no-action");
	m_menubar->add("Mouse-Context/Right/no-action", 0, 
		  (Fl_Callback*)PMV_GUI::cb_RightContext, (void*)0, 
		  FL_MENU_RADIO);
      }
      m_right_mouse_keys.push_back(key);
      uintptr_t index = m_right_mouse_keys.size()-1;
      string label = "Mouse-Context/Right/";
      label += (truncString(key, 40, "middle"));
      // If this was the first right context mode, make it the active
      if(m_right_mouse_keys.size() == 2) {
	mviewer->setRightMouseKey(key);
	m_menubar->add(label.c_str(), 0, 
		  (Fl_Callback*)PMV_GUI::cb_RightContext, 
		  (void*)index, FL_MENU_RADIO|FL_MENU_VALUE);
      }
      else {
	m_menubar->add(label.c_str(), 0, 
		  (Fl_Callback*)PMV_GUI::cb_RightContext, 
		  (void*)index, FL_MENU_RADIO);
      }
      m_menubar->redraw();
    }
  }
  else  // side must be either left or right
    return(false);

  return(true);
}

//-------------------------------------------------------------------
// Procedure: addReferenceVehicle()
//      NOte: Add the vehicle with the given name as one of the possible
//            reference vehicles. When none are given the datum is the
//            the default reference point. So when the first vehicle is
//            given, the datum is added as one of the menu choices. 

bool PMV_GUI::addReferenceVehicle(string vehicle_name)
{
  // First check the current list of vehicles, ignore duplicates
  unsigned int i, vsize = m_reference_tags.size();
  for(i=0; i<vsize; i++) {
    if(vehicle_name == m_reference_tags[i])
      return(false);
  }
  
  // If the list is currently empty, create the "datum" as a choice first.
  if(vsize == 0) {
    m_reference_tags.push_back("bearing-absolute");
    string label = "ReferencePoint/Bearing-Absolute";
    m_menubar->add(label.c_str(), FL_CTRL+'a', 
	      (Fl_Callback*)PMV_GUI::cb_Reference, 
	      (void*)0, FL_MENU_RADIO|FL_MENU_VALUE);
    m_reference_tags.push_back("bearing-relative");
    label = "ReferencePoint/Bearing-Relative";
    m_menubar->add(label.c_str(), FL_CTRL+'r', 
	      (Fl_Callback*)PMV_GUI::cb_Reference, 
	      (void*)1, FL_MENU_DIVIDER|FL_MENU_RADIO);

    m_reference_tags.push_back("datum");
    label = "ReferencePoint/Datum";
    m_menubar->add(label.c_str(), 0, (Fl_Callback*)PMV_GUI::cb_Reference, 
	      (void*)2, FL_MENU_RADIO|FL_MENU_VALUE);
  }

  // Add the new vehicle name as a menu choice
  m_reference_tags.push_back(vehicle_name);
  intptr_t index = m_reference_tags.size()-1;
  string label = "ReferencePoint/";
  label += (truncString(vehicle_name, 25, "middle"));
  m_menubar->add(label.c_str(), 0, 
	    (Fl_Callback*)PMV_GUI::cb_Reference, (void*)index, FL_MENU_RADIO);

  m_menubar->redraw();
  return(true);
}

//-------------------------------------------------------------------
// Procedure: addFilterVehicle()

bool PMV_GUI::addFilterVehicle(string vehicle_name)
{
  // First check the current list of vehicles, ignore duplicates
  unsigned int i, vsize = m_filter_tags.size();
  for(i=0; i<vsize; i++) {
    if(vehicle_name == m_filter_tags[i])
      return(false);
  }

  // Add the new vehicle name as a menu choice
  m_filter_tags.push_back(vehicle_name);
  uintptr_t index = m_filter_tags.size()-1;
  string label = "Vehicles/ClearHistory/";
  label += (truncString(vehicle_name, 25, "middle"));
  m_menubar->add(label.c_str(), 0, 
	    (Fl_Callback*)PMV_GUI::cb_FilterOut, (void*)index, 0);

  m_menubar->redraw();
  return(true);
}

//-------------------------------------------------------------------
// Procedure: removeFilterVehicle()

void PMV_GUI::removeFilterVehicle(string vehicle_name)
{
  // First check the current list of vehicles, ignore duplicates
  unsigned int i, vsize = m_filter_tags.size();
  for(i=0; i<vsize; i++) {
    if(vehicle_name == m_filter_tags[i])
      m_menubar->remove(i+1);
  }

  m_menubar->redraw();
}

//---------------------------------------------------------- 
// Procedure: closeCmdGUI()

void PMV_GUI::closeCmdGUI()
{
  if(m_cmd_gui) {
    int wid = m_cmd_gui->getWid();
    int hgt = m_cmd_gui->getHgt();

    m_cmd_gui_start_wid = wid;
    m_cmd_gui_start_hgt = hgt;
    m_cmd_gui_start_bpost_cnt  = m_cmd_gui->getButPostCount();
    m_cmd_gui_start_cpost_cnt  = m_cmd_gui->getCmdPostCount();
    m_cmd_gui_start_show_posts = m_cmd_gui->getShowPosts();

    delete(m_cmd_gui);
    m_cmd_gui = 0;
  }
}

//---------------------------------------------------------- 
// Procedure: updateRealmCastNodes()

void PMV_GUI::updateRealmCastNodes(bool clear) 
{
  if(!m_rc_repo)
    return;

  vector<string> nodes  = m_rc_repo->getCurrentNodes();
  if(nodes.size() == 0)
    return;

  // Part 1A: build the table which will be the items in the browser.
  // Note which line in the table corresponds to the "current_node".
  ACTable actab(2,2);
  actab << "Node | RC";
  actab.addHeaderLines();
  
  for(unsigned int i=0; i<nodes.size(); i++) {
    string node = nodes[i];
    unsigned int rc_cnt = m_rc_repo->rctree().getNodeRealmCastCount(node);
    actab << node;
    actab << rc_cnt;
  }

  // Part 1B: Add the cluster keys if there are any
  vector<string> cluster_keys = m_rc_repo->getClusterKeys();
  if(cluster_keys.size() > 0) {
    actab.addHeaderLines();
    for(unsigned int i=0; i<cluster_keys.size(); i++) {
      string key = cluster_keys[i];
      unsigned int cluster_wcast_cnt = m_rc_repo->getWatchCastCount(key);
      actab << key << cluster_wcast_cnt;
    }
  }
  
  // Part 2: Build up the browser lines from the previously gen'ed table.
  if(clear)
    m_rc_brw_nodes->clear();

  vector<string> browser_lines = actab.getTableOutput();

  string current_node    = m_rc_repo->getCurrentNode();
  string current_cluster = m_rc_repo->getCurrClusterKey();

  int brw_item_index = 3;
  for(unsigned int j=0; j<browser_lines.size(); j++) {
    string line = browser_lines[j];
    if(clear)
      m_rc_brw_nodes->add(line.c_str());
    else
      m_rc_brw_nodes->text(j+1, line.c_str());

    if(strBegins(line, current_cluster))
      brw_item_index = j+1;
    else if(strBegins(line, current_node))
      brw_item_index = j+1;
  }

  if(clear)
    m_rc_brw_nodes->select(brw_item_index, 1);
}

//---------------------------------------------------------- 
// Procedure: updateAppCastNodes()

void PMV_GUI::updateAppCastNodes(bool clear) 
{
  if(!m_repo)
    return;

  vector<string> nodes  = m_repo->getCurrentNodes();
  unsigned int i, vsize = nodes.size();

  if(vsize == 0)
    return;

  int    curr_brw_ix  = ((Fl_Browser *)m_brw_nodes)->value();
  string current_node = m_repo->getCurrentNode();

  vector<bool> node_has_cfg_warning(vsize, false);
  vector<bool> node_has_run_warning(vsize, false);
  
  // Part one: build the table which will be the items in the browser.
  // Note which line in the table corresponds to the "current_node".
  ACTable actab(4,2);
  actab << "Node | AC | CW | RW";
  actab.addHeaderLines();
  
  int brw_item_index   = 0;   // Zero indicates current_node not found
  for(i=0; i<vsize; i++) {
    string node = nodes[i];

    if(current_node == node)  // +1 because browser indices go 1-N
      brw_item_index = i+3;   // +2 because of the two header lines

    unsigned int apc_cnt, cfg_cnt, run_cnt;
    apc_cnt = m_repo->actree().getNodeAppCastCount(node);
    cfg_cnt = m_repo->actree().getNodeCfgWarningCount(node);
    run_cnt = m_repo->actree().getNodeRunWarningCount(node);

    actab << node;
    actab << apc_cnt;
    actab << cfg_cnt;
    actab << run_cnt;

    if(cfg_cnt > 0)
      node_has_cfg_warning[i] = true;
    if(run_cnt > 0)
      node_has_run_warning[i] = true;
  }

  // Part 2: Build up the browser lines from the previously gen'ed table.
  if(clear)
    m_brw_nodes->clear();
  
  double stale_thresh = mviewer->getStaleReportThresh();
  vector<string> stale_names = mviewer->getStaleVehicles(stale_thresh);

  unsigned int   curr_brw_size = m_brw_nodes->size();
  vector<string> browser_lines = actab.getTableOutput();

  for(unsigned int j=0; j<browser_lines.size(); j++) {
    string line = browser_lines[j];
    if(j>=2) {

      string line_copy = line;
      string node  = biteString(line_copy, ' ');
      bool   stale = vectorContains(stale_names, node);
      
      if(stale) // Draw as yellow if the node is stale
      	line = "@B" + uintToString(m_color_stlw) + line;
      
      else if(node_has_run_warning[j-2])
	line = "@B" + uintToString(m_color_runw) + line;
      else if(node_has_cfg_warning[j-2])
	line = "@B" + uintToString(m_color_cfgw) + line;
    }
    if((j+1) > curr_brw_size)
      m_brw_nodes->add(line.c_str());
    else
      m_brw_nodes->text(j+1, line.c_str());
  }

  // Part 3: Possibly select an item in the browser under rare circumstances
  // If we've cleared the nodes to build this, or if no userclicks prior
  // to this call, set the browser select to be the current_node.
  if(clear || (curr_brw_ix==0))
    m_brw_nodes->select(brw_item_index, 1);
}

//---------------------------------------------------------- 
// Procedure: updateRealmCastProcs()

void PMV_GUI::updateRealmCastProcs(bool clear) 
{
  string curr_cluster_key = m_rc_repo->getCurrClusterKey();

  if(curr_cluster_key != "")
    updateRealmCastProcsWC(clear);
  else
    updateRealmCastProcsRC(clear);
}

//---------------------------------------------------------- 
// Procedure: updateRealmCastProcsWC()

void PMV_GUI::updateRealmCastProcsWC(bool clear) 
{
  if(!m_rc_repo)
    return;

  string curr_cluster_key = m_rc_repo->getCurrClusterKey();
  string curr_cluster_var = m_rc_repo->getCurrClusterVar();
  if(curr_cluster_key == "")
    return;
  
  vector<string> vars = m_rc_repo->getClusterVars(curr_cluster_key);
  if(vars.size() == 0)
    return;

  int curr_brw_ix = ((Fl_Browser *)m_rc_brw_procs)->value();

  // Part one: build the table which will be the items in the browser.
  // Note which line in the table corresponds to the "current_proc".
  ACTable actab(2,2);
  actab << "Variable | Updates ";
  actab.addHeaderLines();

  int brw_item_index = 0;     // Zero indicates curr_cluster_var not found.
  for(unsigned int i=0; i<vars.size(); i++) {
    string var = vars[i];

    if(curr_cluster_key == var) // +1 because browser indices go 1-N
      brw_item_index = i+3;     // +2 because of the two header lines

    unsigned int watch_cnt = 0;
    //watch_cnt = m_rc_repo->getWatchCount(curr_cluster_key, curr_cluster_var);
    watch_cnt = m_rc_repo->getWatchCount(curr_cluster_key, var);
    
    actab << var;
    actab << watch_cnt;
  }

  // Part 2: Build up the browser lines from the previously gen'ed table.
  if(clear)
    m_rc_brw_procs->clear();

  unsigned int curr_brw_size = m_rc_brw_procs->size();

  vector<string> browser_lines = actab.getTableOutput();
  for(unsigned int j=0; j<browser_lines.size(); j++) {
    string line = browser_lines[j];
    if((j+1) > curr_brw_size)
      m_rc_brw_procs->add(line.c_str());
    else
      m_rc_brw_procs->text(j+1, line.c_str());

    if(strBegins(line, curr_cluster_var))
      brw_item_index = j+1;    
  }

  // Part 3: Possibly select an item in the browser under rare circumstances
  // If we've cleared the procs to build this, or if no userclicks prior
  // to this call, set the browser select to be the current_proc.
  if(clear || (curr_brw_ix==0))
    m_rc_brw_procs->select(brw_item_index, 1);

}

//---------------------------------------------------------- 
// Procedure: updateRealmCastProcsRC()

void PMV_GUI::updateRealmCastProcsRC(bool clear) 
{
  if(!m_rc_repo)
    return;
  
  vector<string> procs = m_rc_repo->getCurrentProcs();
  if(procs.size() == 0)
    return;

  int    curr_brw_ix  = ((Fl_Browser *)m_rc_brw_procs)->value();
  string current_node = m_rc_repo->getCurrentNode();
  string current_proc = m_rc_repo->getCurrentProc();

  // Part one: build the table which will be the items in the browser.
  // Note which line in the table corresponds to the "current_proc".
  ACTable actab(2,2);
  actab << "App | RC ";
  actab.addHeaderLines();

  int brw_item_index = 0;     // Zero indicates curr_proc not found.
  for(unsigned int i=0; i<procs.size(); i++) {
    string proc = procs[i];

    if(current_proc == proc)  // +1 because browser indices go 1-N
      brw_item_index = i+3;   // +2 because of the two header lines

    unsigned int apc_cnt = 0;
    apc_cnt = m_rc_repo->rctree().getProcRealmCastCount(current_node, proc);

    actab << proc;
    actab << apc_cnt;
  }

  // Part 2: Build up the browser lines from the previously gen'ed table.
  if(clear)
    m_rc_brw_procs->clear();

  unsigned int curr_brw_size = m_rc_brw_procs->size();

  vector<string> browser_lines = actab.getTableOutput();
  for(unsigned int j=0; j<browser_lines.size(); j++) {
    string line = browser_lines[j];
    if((j+1) > curr_brw_size)
      m_rc_brw_procs->add(line.c_str());
    else
      m_rc_brw_procs->text(j+1, line.c_str());
  }

  // Part 3: Possibly select an item in the browser under rare circumstances
  // If we've cleared the procs to build this, or if no userclicks prior
  // to this call, set the browser select to be the current_proc.
  if(clear || (curr_brw_ix==0))
    m_rc_brw_procs->select(brw_item_index, 1);

}

//---------------------------------------------------------- 
// Procedure: updateAppCastProcs()

void PMV_GUI::updateAppCastProcs(bool clear) 
{
  if(!m_repo)
    return;
  
  vector<string> procs  = m_repo->getCurrentProcs();
  unsigned int i, vsize = procs.size();
  
  if(vsize == 0)
    return;

  int    curr_brw_ix  = ((Fl_Browser *)m_brw_procs)->value();
  string current_node = m_repo->getCurrentNode();
  string current_proc = m_repo->getCurrentProc();

  vector<bool> proc_has_cfg_warning(vsize, false);
  vector<bool> proc_has_run_warning(vsize, false);

  // Part one: build the table which will be the items in the browser.
  // Note which line in the table corresponds to the "current_proc".
  ACTable actab(4,2);
  actab << "App | AC | CW | RW";
  actab.addHeaderLines();

  int brw_item_index = 0;     // Zero indicates curr_proc not found.
  for(i=0; i<vsize; i++) {
    string proc = procs[i];

    if(current_proc == proc)  // +1 because browser indices go 1-N
      brw_item_index = i+3;   // +2 because of the two header lines

    unsigned int apc_cnt, cfg_cnt, run_cnt;
    apc_cnt = m_repo->actree().getProcAppCastCount(current_node, proc);
    cfg_cnt = m_repo->actree().getProcCfgWarningCount(current_node, proc);
    run_cnt = m_repo->actree().getProcRunWarningCount(current_node, proc);

    actab << proc;
    actab << apc_cnt;
    actab << cfg_cnt;
    actab << run_cnt;

    if(cfg_cnt > 0)
      proc_has_cfg_warning[i] = true;
    if(run_cnt > 0)
      proc_has_run_warning[i] = true;
  }

  // Part 2: Build up the browser lines from the previously gen'ed table.
  if(clear)
    m_brw_procs->clear();

  unsigned int   curr_brw_size = m_brw_procs->size();
  vector<string> browser_lines = actab.getTableOutput();
  for(unsigned int j=0; j<browser_lines.size(); j++) {
    string line = browser_lines[j];
    if(j>=2) {
      if(proc_has_run_warning[j-2])
	line = "@B" + uintToString(m_color_runw) + line;
      else if(proc_has_cfg_warning[j-2])
	line = "@B" + uintToString(m_color_cfgw) + line;
    }

    if((j+1) > curr_brw_size)
      m_brw_procs->add(line.c_str());
    else
      m_brw_procs->text(j+1, line.c_str());
  }

  // Part 3: Possibly select an item in the browser under rare circumstances
  // If we've cleared the procs to build this, or if no userclicks prior
  // to this call, set the browser select to be the current_proc.
  if(clear || (curr_brw_ix==0))
    m_brw_procs->select(brw_item_index, 1);

}

//----------------------------------------- 
// Procedure: updateRealmCast()

void PMV_GUI::updateRealmCast() 
{
  if(!m_rc_repo)
    return;

  string node = m_rc_repo->getCurrentNode();
  string proc = m_rc_repo->getCurrentProc();
  if((node == "") || (proc == ""))
    return;

  // Step 1: clear the Fl_Browser contents
  m_rc_brw_casts->clear();

  if(m_rc_repo->getCurrClusterKey() == "") {
    // Step 2: retrieve the current realmcast and its report lines
    RealmCast relcast = m_rc_repo->rctree().getRealmCast(node, proc);
    vector<string> svector = parseString(relcast.getFormattedString(), '\n');
    
    // Step 3: De-colorize the report lines and add to the Browser
    // Disable the removeTermColors() call by mikerb Oct 11, 2020. 
    for(unsigned int i=0; i<svector.size(); i++) {
      m_rc_brw_casts->add(svector[i].c_str());
      //cout << svector[i].c_str() << endl; // mikerb
      //m_brw_casts->add(removeTermColors(svector[i]).c_str());
    }
  }
  else {
    vector<string> svector = m_rc_repo->getClusterReport(m_icast_settings);
    for(unsigned int i=0; i<svector.size(); i++) {
      m_rc_brw_casts->add(svector[i].c_str());
    }
  }
}

//----------------------------------------- 
// Procedure: updateAppCast()

void PMV_GUI::updateAppCast() 
{
  if(!m_repo)
    return;

  string node = m_repo->getCurrentNode();
  string proc = m_repo->getCurrentProc();
  if((node == "") || (proc == ""))
    return;

  // Step 1: clear the Fl_Browser contents
  m_brw_casts->clear();

  // Step 2: retrieve the current appcast and its report lines
  AppCast appcast = m_repo->actree().getAppCast(node, proc);
  vector<string> svector = parseString(appcast.getFormattedString(), '\n');



  // Step 3: De-colorize the report lines and add to the Browser
  // Disable the removeTermColors() call by mikerb Oct 11, 2020. 
  for(unsigned int i=0; i<svector.size(); i++) {
    m_brw_casts->add(svector[i].c_str());
    //m_brw_casts->add(removeTermColors(svector[i]).c_str());
  }

  // Step 4: If there are warnings, colorize line 2 of the browser
  //         appropriately. Red if run_warnings, Blue if config warnings
  if(svector.size() >=2) {
    if(appcast.getRunWarningCount() > 0) {
      string txt = m_brw_casts->text(2);
      txt = "@B" + uintToString(m_color_runw) + txt;
      m_brw_casts->text(2, txt.c_str());
    }
    else if(appcast.getCfgWarningCount() > 0) {
      string txt = m_brw_casts->text(2);
      txt = "@B" + uintToString(m_color_cfgw) + txt;
      m_brw_casts->text(2, txt.c_str());
    }
  }
}


//----------------------------------------------------- 
// Procedure: updateRadios()

void PMV_GUI::updateRadios()
{
  MarineVehiGUI::updateRadios();
  setRadioCastAttrib("nodes_font_size");
  setRadioCastAttrib("procs_font_size");
  setRadioCastAttrib("infocast_font_size");
  setRadioCastAttrib("infocast_width");
  setRadioCastAttrib("infocast_width");
  setRadioCastAttrib("full_screen");
  setRadioCastAttrib("refresh_mode");
  setRadioCastAttrib("content_mode");
  setRadioCastAttrib("appcast_viewable");
  setRadioCastAttrib("appcast_color_scheme");
  setRadioCastAttrib("realmcast_color_scheme");
  setRadioCastAttrib("realmcast_show_source");
  setRadioCastAttrib("realmcast_show_community");
  setRadioCastAttrib("realmcast_show_subscriptions");
  setRadioCastAttrib("realmcast_show_masked");
  setRadioCastAttrib("realmcast_wrap_content");
  setRadioCastAttrib("realmcast_trunc_content");
  setRadioCastAttrib("realmcast_time_format_utc");
}


//----------------------------------------------------- 
// Procedure: setMenuItemColors()

void PMV_GUI::setMenuItemColors()
{
  MarineVehiGUI::setMenuItemColors();
  setMenuItemColor("InfoCasting/infocast_viewable=true");
  setMenuItemColor("InfoCasting/infocast_viewable=false");

  setMenuItemColor("InfoCasting/refresh_mode=paused");
  setMenuItemColor("InfoCasting/refresh_mode=events");
  setMenuItemColor("InfoCasting/refresh_mode=streaming");

  setMenuItemColor("InfoCasting/content_mode=appcast");
  setMenuItemColor("InfoCasting/content_mode=realmcast");

  setMenuItemColor("InfoCasting/nodes_font_size=xlarge");
  setMenuItemColor("InfoCasting/nodes_font_size=large");
  setMenuItemColor("InfoCasting/nodes_font_size=medium");
  setMenuItemColor("InfoCasting/nodes_font_size=small");
  setMenuItemColor("InfoCasting/nodes_font_size=xsmall");

  setMenuItemColor("InfoCasting/procs_font_size=xlarge");
  setMenuItemColor("InfoCasting/procs_font_size=large");
  setMenuItemColor("InfoCasting/procs_font_size=medium");
  setMenuItemColor("InfoCasting/procs_font_size=small");
  setMenuItemColor("InfoCasting/procs_font_size=xsmall");

  setMenuItemColor("InfoCasting/infocast_font_size=xlarge");
  setMenuItemColor("InfoCasting/infocast_font_size=large");
  setMenuItemColor("InfoCasting/infocast_font_size=medium");
  setMenuItemColor("InfoCasting/infocast_font_size=small");
  setMenuItemColor("InfoCasting/infocast_font_size=xsmall");

  setMenuItemColor("InfoCasting/appcast_color_scheme=white");
  setMenuItemColor("InfoCasting/appcast_color_scheme=indigo");
  setMenuItemColor("InfoCasting/appcast_color_scheme=beige");

  setMenuItemColor("InfoCasting/realmcast_color_scheme=white");
  setMenuItemColor("InfoCasting/realmcast_color_scheme=indigo");
  setMenuItemColor("InfoCasting/realmcast_color_scheme=beige");
  setMenuItemColor("InfoCasting/realmcast_color_scheme=hillside");

  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=20");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=25");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=30");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=35");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=40");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=45");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=50");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=55");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=60");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=65");
  setMenuItemColor("InfoCasting/InfoCast Pane Width/infocast_width=70");

  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=30");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=35");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=40");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=45");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=50");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=55");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=60");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=65");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=70");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=75");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=80");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=85");
  setMenuItemColor("InfoCasting/InfoCast Pane Height/infocast_height=90");

  setMenuItemColor("InfoCasting/realmcast_show_source/realmcast_show_source=true");
  setMenuItemColor("InfoCasting/realmcast_show_source/realmcast_show_source=false");
  
  setMenuItemColor("InfoCasting/realmcast_show_community/realmcast_show_community=true");
  setMenuItemColor("InfoCasting/realmcast_show_community/realmcast_show_community=false");
  
  setMenuItemColor("InfoCasting/realmcast_show_subscriptions/realmcast_show_subscriptions=true");
  setMenuItemColor("InfoCasting/realmcast_show_subscriptions/realmcast_show_subscriptions=false");

  setMenuItemColor("InfoCasting/realmcast_show_masked/realmcast_show_masked=true");
  setMenuItemColor("InfoCasting/realmcast_show_masked/realmcast_show_masked=false");
  
  setMenuItemColor("InfoCasting/realmcast_wrap_content/realmcast_wrap_content=true");
  setMenuItemColor("InfoCasting/realmcast_wrap_content/realmcast_wrap_content=false");
  
  setMenuItemColor("InfoCasting/realmcast_trunc_content/realmcast_trunc_content=true");
  setMenuItemColor("InfoCasting/realmcast_trunc_content/realmcast_trunc_content=false");

  setMenuItemColor("InfoCasting/realmcast_time_format_utc/realmcast_time_format_utc=true");
  setMenuItemColor("InfoCasting/realmcast_time_format_utc/realmcast_time_format_utc=false");

  setMenuItemColor("BackView/full_screen=true");
  setMenuItemColor("BackView/full_screen=false");
}


//----------------------------------------------------- 
// Procedure: setRadioCastAttrib()
//   Purpose: A way of setting the appcast attribute assuring that 
//            the GUI radio buttons are properly updated.
//  
//  Examples: "nodes_font_size", "xsmall"
//            "refresh_mode", "false"

bool PMV_GUI::setRadioCastAttrib(string attr, string value) 
{
  Fl_Color fcolor_grayBlu = fl_rgb_color(165,165,225);
  Fl_Color fcolor_gray185 = fl_rgb_color(185,185,185);
  Fl_Color fcolor_grayRed = fl_rgb_color(225,185,185);
  
  string item_str = "InfoCasting/" + attr + "=";

  bool ok = false;
  if(attr == "full_screen") {
    ok = m_icast_settings.setFullScreen(value);
    item_str = "BackView/full_screen=";
    item_str += boolToString(m_icast_settings.getFullScreen());
  }
  else if(attr == "infocast_viewable") {
    ok = m_icast_settings.setInfoCastViewable(value);
    item_str += boolToString(m_icast_settings.getInfoCastViewable());
  }
  else if(attr == "content_mode") {
    ok = m_icast_settings.setContentMode(value);
    item_str += m_icast_settings.getContentMode();
  }
  else if(attr == "refresh_mode") {
    m_repo->setRefreshMode(value);
    ok = m_icast_settings.setRefreshMode(value);
    item_str += m_icast_settings.getRefreshMode();
  }
  else if(attr == "infocast_font_size") {
    ok = m_icast_settings.setInfoCastFontSize(value);
    item_str += m_icast_settings.getInfoCastFontSize();
  }
  else if(attr == "procs_font_size") {
    ok = m_icast_settings.setProcsFontSize(value);
    item_str += m_icast_settings.getProcsFontSize();
  }
  else if(attr == "nodes_font_size") {
    ok = m_icast_settings.setNodesFontSize(value);
    item_str += m_icast_settings.getNodesFontSize();
  }
  else if(attr == "infocast_height") {
    ok = m_icast_settings.setInfoCastHeight(value);
    item_str = "InfoCasting/InfoCast Window Height/infocast_height=";
    item_str += intToString(m_icast_settings.getInfoCastHeight());
  }

  else if(attr == "infocast_width") {
    ok = m_icast_settings.setInfoCastWidth(value);
    item_str = "InfoCasting/InfoCast Window Width/infocast_width=";
    item_str += intToString(m_icast_settings.getInfoCastWidth());
  }
  
  else if(attr == "realmcast_show_source") {
    ok = m_icast_settings.setShowRealmCastSource(value);
    item_str = "InfoCasting/realmcast_show_source/realmcast_show_source=";
    item_str += boolToString(m_icast_settings.getShowRealmCastSource());
    if(m_icast_settings.getShowRealmCastSource())
      m_rc_button_src->color(fcolor_gray185);
    else
      m_rc_button_src->color(fcolor_grayRed);
    updateRealmCast();
  }
  
  else if(attr == "realmcast_show_community") {
    ok = m_icast_settings.setShowRealmCastCommunity(value);
    item_str = "InfoCasting/realmcast_show_community/realmcast_show_community=";
    item_str += boolToString(m_icast_settings.getShowRealmCastCommunity());
    if(m_icast_settings.getShowRealmCastCommunity())
      m_rc_button_com->color(fcolor_gray185);
    else
      m_rc_button_com->color(fcolor_grayRed);
    updateRealmCast();
  }
  
  else if(attr == "realmcast_show_subscriptions") {
    ok = m_icast_settings.setShowRealmCastSubs(value);
    item_str = "InfoCasting/realmcast_show_subscriptions/realmcast_show_subscriptions=";
    item_str += boolToString(m_icast_settings.getShowRealmCastSubs());
    if(m_icast_settings.getShowRealmCastSubs())
      m_rc_button_sub->color(fcolor_gray185);
    else
      m_rc_button_sub->color(fcolor_grayRed);
  }
  
  else if(attr == "realmcast_show_masked") {
    ok = m_icast_settings.setShowRealmCastMasked(value);
    item_str = "InfoCasting/realmcast_show_masked/realmcast_show_masked=";
    item_str += boolToString(m_icast_settings.getShowRealmCastMasked());
    if(m_icast_settings.getShowRealmCastMasked())
      m_rc_button_msk->color(fcolor_gray185);
    else
      m_rc_button_msk->color(fcolor_grayRed);
  }
  
  else if(attr == "realmcast_wrap_content") {
    ok = m_icast_settings.setWrapRealmCastContent(value);
    item_str = "InfoCasting/realmcast_wrap_content/realmcast_wrap_content=";
    item_str += boolToString(m_icast_settings.getWrapRealmCastContent());
    if(m_icast_settings.getWrapRealmCastContent())
      m_rc_button_wrp->color(fcolor_grayBlu);
    else
      m_rc_button_wrp->color(fcolor_gray185);
  }
  
  else if(attr == "realmcast_trunc_content") {
    ok = m_icast_settings.setTruncRealmCastContent(value);
    item_str = "InfoCasting/realmcast_trunc_content/realmcast_trunc_content=";
    item_str += boolToString(m_icast_settings.getTruncRealmCastContent());
    if(m_icast_settings.getTruncRealmCastContent())
      m_rc_button_trc->color(fcolor_grayBlu);
    else
      m_rc_button_trc->color(fcolor_gray185);
  }
  
  else if(attr == "realmcast_time_format_utc") {
    ok = m_icast_settings.setRealmCastTimeFormatUTC(value);
    item_str = "InfoCasting/realmcast_time_format_utc/realmcast_time_format_utc=";
    item_str += boolToString(m_icast_settings.getRealmCastTimeFormatUTC());
    if(m_icast_settings.getRealmCastTimeFormatUTC())
      m_rc_button_utc->color(fcolor_grayBlu);
    else
      m_rc_button_utc->color(fcolor_gray185);
    updateRealmCast();
  }
  
  else if(attr == "appcast_color_scheme") {
    ok = m_icast_settings.setAppCastColorScheme(value);
    item_str += m_icast_settings.getAppCastColorScheme();
  }
  
  else if(attr == "realmcast_color_scheme") {
    ok = m_icast_settings.setRealmCastColorScheme(value);
    item_str += m_icast_settings.getRealmCastColorScheme();
  }

  // Note: Even though perhaps ok=false, the attempted setting will
  // have no effect. Also when value="". In this case we just want
  // to update the item radio button. 
  
  const Fl_Menu_Item *item = m_menubar->find_item(item_str.c_str());
  if(item)
    ((Fl_Menu_Item *)item)->setonly();

  resizeWidgets();
  redraw();
  return(ok);
}


//-------------------------------------------------------------------
// Procedure: showDataFields()

void PMV_GUI::showDataFields()
{
  v_nam->show();
  v_typ->show();
  x_mtr->show();
  y_mtr->show();
  v_lat->show();
  v_lon->show();
  v_spd->show();
  v_crs->show();
  v_dep->show();
  v_ais->show();
  time->show();
  warp->show();
  m_scope_variable->show();
  m_scope_time->show();
  m_scope_value->show();

  for(unsigned int i=0; i<m_buttons.size(); i++) {
    string label = m_buttons[i]->label();

    if((label != "SRC") && (label != "COM") && (label != "WRP") &&
       (label != "SUBS") && (label != "MASK") && (label != "TRUNC")) {
      if(label != "Disabled")
	m_buttons[i]->show();
      else
	m_buttons[i]->hide();
    }
  }
}

//-------------------------------------------------------------------
// Procedure: hideDataFields()

void PMV_GUI::hideDataFields()
{
  v_nam->hide();
  v_typ->hide();
  x_mtr->hide();
  y_mtr->hide();
  v_lat->hide();
  v_lon->hide();
  v_spd->hide();
  v_crs->hide();
  v_dep->hide();
  v_ais->hide();
  time->hide();
  warp->hide();
  m_scope_variable->hide();
  m_scope_time->hide();
  m_scope_value->hide();

  for(unsigned int i=0; i<m_buttons.size(); i++) {
    string label = m_buttons[i]->label();
    m_buttons[i]->hide();
  }

}

//----------------------------------------------------------
// Procedure: resizeDataText()

void PMV_GUI::resizeDataText(int wsize) 
{
  for(unsigned int i=0; i<m_buttons.size(); i++) 
    m_buttons[i]->labelsize(wsize);
  
  v_nam->labelsize(wsize);
  v_typ->labelsize(wsize);
  x_mtr->labelsize(wsize);
  y_mtr->labelsize(wsize);
  v_lat->labelsize(wsize);
  v_lon->labelsize(wsize);
  v_spd->labelsize(wsize);
  v_crs->labelsize(wsize);
  v_dep->labelsize(wsize);
  v_ais->labelsize(wsize);
  time->labelsize(wsize);
  warp->labelsize(wsize);
  m_scope_variable->labelsize(wsize);
  m_scope_time->labelsize(wsize);
  m_scope_value->labelsize(wsize);
  
  v_nam->textsize(wsize);
  v_typ->textsize(wsize);
  x_mtr->textsize(wsize);
  y_mtr->textsize(wsize);
  v_lat->textsize(wsize);
  v_lon->textsize(wsize);
  v_spd->textsize(wsize);
  v_crs->textsize(wsize);
  v_dep->textsize(wsize);
  v_ais->textsize(wsize);
  time->textsize(wsize);
  warp->textsize(wsize);
  m_scope_variable->textsize(wsize-1);
  m_scope_time->textsize(wsize-1);
  m_scope_value->textsize(wsize-1);
}

//-------------------------------------------------------------------
// Procedure: resizeWidgets()

void PMV_GUI::resizeWidgets()
{
  bool show_fullscreen  = m_icast_settings.getFullScreen();
  bool show_infocasting = m_icast_settings.getInfoCastViewable();
  string content_mode  = m_icast_settings.getContentMode();
  
  // Part 1: Figure out the basic pane extents.
  // ================================================================
  // This is the default extent configuration so long as any minimal
  // constraints are not violated.

  double pct_cast_wid = m_icast_settings.getInfoCastWidth()  / 100;
  double pct_cast_hgt = m_icast_settings.getInfoCastHeight() / 100;

  //double pct_view_wid = 1 - pct_cast_wid;

  double pct_view_hgt = 0.875;
  double pct_menu_hgt = 0.035;
  double pct_data_hgt = 0.09; // Above three add up to 1.0

  // Check for violations of minimal constraints
  double now_hgt = h();
  double now_wid = w();

  // Make sure the menu bar is at least 15 and at most 25
  if((pct_menu_hgt * now_hgt) < 15) 
    pct_menu_hgt = 15 / now_hgt;
  if((pct_menu_hgt * now_hgt) > 25) 
    pct_menu_hgt = 25 / now_hgt;
  pct_view_hgt = (1 - (pct_menu_hgt + pct_data_hgt));
    
  // Make sure the data field is between 80 and 120
  if((pct_data_hgt * now_hgt) < 80) 
    pct_data_hgt = 80 / now_hgt;
  if((pct_data_hgt * now_hgt) > 120) 
    pct_data_hgt = 120 / now_hgt;
  pct_view_hgt = (1 - (pct_menu_hgt + pct_data_hgt));

  // Make sure the infocast pane is at least 300
  if((pct_cast_wid * now_wid) < 300) {
    pct_cast_wid = 300 / now_wid;
    //pct_view_wid = (1 - pct_cast_wid);
  }

  
  // Part 2: Adjust the extents of the MENU Bar
  // ================================================================  
  double menu_hgt = h() * pct_menu_hgt;

  m_menubar->resize(0, 0, w(), menu_hgt);
  
  int menu_font_size = 14;
  if(now_hgt < 550) 
    menu_font_size = 10;
  else if(now_hgt < 800) 
    menu_font_size = 12;
  
  int  msize = m_menubar->size();

  const Fl_Menu_Item* itemsx = m_menubar->menu();
  Fl_Menu_Item* items = (Fl_Menu_Item*)(itemsx);

  //Fl_Color blue = fl_rgb_color(71, 71, 205);    
  for(int i=0; i<msize; i++) {
    //items[i].labelcolor(blue);
    items[i].labelsize(menu_font_size);
  }

  // Part 3A: Pre-calculations for AppCast and RealmCast
  // ================================================================  
  double bx = 0;
  double by = menu_hgt;
  double bw = w() * pct_cast_wid;
  double bh = h() * pct_view_hgt;
  
  double cast_wid = bw;
  double node_wid = bw * 0.45;
  double proc_wid = bw * 0.55;
  
  double cast_hgt = bh * pct_cast_hgt;
  double node_hgt = bh * (1 - pct_cast_hgt);
  double proc_hgt = bh * (1- pct_cast_hgt);
  
  int font_xlarge  = 16;
  int font_large  = 14;
  int font_medium = 12;
  int font_small  = 10;
  int font_xsmall = 8;
  if(bw < 400) {
    font_xlarge = 14;
    font_large  = 12;
    font_medium = 10;
    font_small  = 8;
    font_xsmall = 7;
  }
  
  // Part 3: Adjust the extents of the InfoCast browsers
  // ================================================================  
  if(!show_infocasting) {
    m_brw_nodes->hide();
    m_brw_procs->hide();
    m_brw_casts->hide();
    m_rc_brw_nodes->hide();
    m_rc_brw_procs->hide();
    m_rc_brw_casts->hide();
  }   
  else if(content_mode == "appcast") {
    m_rc_brw_nodes->hide();
    m_rc_brw_procs->hide();
    m_rc_brw_casts->hide();

    m_rc_button_src->hide();
    m_rc_button_com->hide();
    m_rc_button_utc->hide();
    m_rc_button_sub->hide();
    m_rc_button_msk->hide();
    m_rc_button_wrp->hide();
    m_rc_button_trc->hide();

    m_brw_nodes->show();
    m_brw_procs->show();
    m_brw_casts->show();

   m_brw_nodes->resize(bx, menu_hgt, node_wid, node_hgt);
    m_brw_procs->resize(node_wid, menu_hgt, proc_wid, proc_hgt);
    m_brw_casts->resize(bx, by+node_hgt, cast_wid, cast_hgt);
    
    string nodes_font_size   = m_icast_settings.getNodesFontSize();
    string procs_font_size   = m_icast_settings.getProcsFontSize();
    string infocast_font_size = m_icast_settings.getInfoCastFontSize();
    
    if(nodes_font_size == "xsmall")      m_brw_nodes->textsize(font_xsmall);
    else if(nodes_font_size == "small")  m_brw_nodes->textsize(font_small);
    else if(nodes_font_size == "medium") m_brw_nodes->textsize(font_medium);
    else if(nodes_font_size == "large")  m_brw_nodes->textsize(font_large);
    else                                 m_brw_nodes->textsize(font_xlarge);
    
    if(procs_font_size == "xsmall")      m_brw_procs->textsize(font_xsmall);
    else if(procs_font_size == "small")  m_brw_procs->textsize(font_small);
    else if(procs_font_size == "medium") m_brw_procs->textsize(font_medium);
    else if(procs_font_size == "large")  m_brw_procs->textsize(font_large);
    else                                 m_brw_procs->textsize(font_xlarge);
    
    if(infocast_font_size == "xsmall")      m_brw_casts->textsize(font_xsmall);
    else if(infocast_font_size == "small")  m_brw_casts->textsize(font_small);
    else if(infocast_font_size == "medium") m_brw_casts->textsize(font_medium);
    else if(infocast_font_size == "large")  m_brw_casts->textsize(font_large);
    else                                    m_brw_casts->textsize(font_xlarge);
  }
  else if(content_mode == "realmcast") {
    m_brw_nodes->hide();
    m_brw_procs->hide();
    m_brw_casts->hide();

    m_rc_brw_nodes->show();
    m_rc_brw_procs->show();
    m_rc_brw_casts->show();

    m_rc_button_src->show();
    m_rc_button_com->show();
    m_rc_button_utc->show();
    m_rc_button_sub->show();
    m_rc_button_msk->show();
    m_rc_button_wrp->show();
    m_rc_button_trc->show();

    m_rc_brw_nodes->resize(bx, menu_hgt, node_wid, node_hgt);
    m_rc_brw_procs->resize(node_wid, menu_hgt, proc_wid, proc_hgt);
    m_rc_brw_casts->resize(bx, by+node_hgt, cast_wid, cast_hgt);
    
    string nodes_font_size   = m_icast_settings.getNodesFontSize();
    string procs_font_size   = m_icast_settings.getProcsFontSize();
    string infocast_font_size = m_icast_settings.getInfoCastFontSize();
    
    if(nodes_font_size == "xsmall")      m_rc_brw_nodes->textsize(font_xsmall);
    else if(nodes_font_size == "small")  m_rc_brw_nodes->textsize(font_small);
    else if(nodes_font_size == "medium") m_rc_brw_nodes->textsize(font_medium);
    else if(nodes_font_size == "large")  m_rc_brw_nodes->textsize(font_large);
    else                                 m_rc_brw_nodes->textsize(font_xlarge);
    
    if(procs_font_size == "xsmall")      m_rc_brw_procs->textsize(font_xsmall);
    else if(procs_font_size == "small")  m_rc_brw_procs->textsize(font_small);
    else if(procs_font_size == "medium") m_rc_brw_procs->textsize(font_medium);
    else if(procs_font_size == "large")  m_rc_brw_procs->textsize(font_large);
    else                                 m_rc_brw_procs->textsize(font_xlarge);
    
    if(infocast_font_size == "xsmall")      m_rc_brw_casts->textsize(font_xsmall);
    else if(infocast_font_size == "small")  m_rc_brw_casts->textsize(font_small);
    else if(infocast_font_size == "medium") m_rc_brw_casts->textsize(font_medium);
    else if(infocast_font_size == "large")  m_rc_brw_casts->textsize(font_large);
    else                                    m_rc_brw_casts->textsize(font_xlarge);
  }

  // Part 4: Adjust the color scheme of the InfoCast browsers
  // ================================================================  
  string infocast_color_scheme = m_icast_settings.getAppCastColorScheme();
  if(content_mode == "realmcast")
    infocast_color_scheme = m_icast_settings.getRealmCastColorScheme();
  
  Fl_Color color_back = fl_rgb_color(255, 255, 255);  // white
  Fl_Color color_text = fl_rgb_color(0, 0, 0);        // black
  m_color_runw = fl_rgb_color(205, 71, 71);           // redish
  m_color_cfgw = fl_rgb_color(50, 189, 149);          // greenish
  m_color_stlw = fl_rgb_color(125, 125, 0);           // yellowish

  if(infocast_color_scheme == "indigo") {
    color_back = fl_rgb_color(95, 117, 182);   // indigo-lighter (65,87,152)
    color_text = fl_rgb_color(255, 255, 255);  // white
    m_color_runw = fl_rgb_color(205, 71, 71);  // redish
    m_color_cfgw = fl_rgb_color(0, 159, 119);  // greenish
  }
  else if(infocast_color_scheme == "beige") {
    color_back = fl_rgb_color(223, 219, 195);   // beige
    color_text = fl_rgb_color(0, 0, 0);         // black
    m_color_runw = fl_rgb_color(205, 71, 71);   // redish
    m_color_cfgw = fl_rgb_color(0, 189, 149);   // greenish
  }
  else if(infocast_color_scheme == "hillside") {
    color_back = fl_rgb_color(209, 203, 127);   // hillside
    color_text = fl_rgb_color(0, 0, 0);         // black
    m_color_runw = fl_rgb_color(205, 71, 71);   // redish
    m_color_cfgw = fl_rgb_color(0, 189, 149);   // greenish
  }

  if(content_mode == "appcast") {
    m_brw_nodes->color(color_back);
    m_brw_nodes->textcolor(color_text);
    m_brw_nodes->position(0);    
    m_brw_procs->color(color_back);
    m_brw_procs->textcolor(color_text);
    m_brw_procs->position(0);
    m_brw_casts->color(color_back);
    m_brw_casts->textcolor(color_text);
  }
  else if(content_mode == "realmcast") {
    m_rc_brw_nodes->color(color_back);
    m_rc_brw_nodes->textcolor(color_text);
    m_rc_brw_nodes->position(0);    
    m_rc_brw_procs->color(color_back);
    m_rc_brw_procs->textcolor(color_text);
    m_rc_brw_procs->position(0);
    m_rc_brw_casts->color(color_back);
    m_rc_brw_casts->textcolor(color_text);
  }

  //m_brw_casts->position(0);


  // Part 5: Adjust the extents of the VIEWER window
  // ================================================================
  // Set the extents as if appcasting and datafields were hidden (or
  // if show_full_screen is set to true).
  double xpos = 0;
  double ypos = 0 + menu_hgt;
  double xwid = w();
  double yhgt = h() - (int)(menu_hgt);

  // shrink the viewer width if showing appcasts
  if(show_infocasting && !show_fullscreen) {
    double infocast_wid = w() * pct_cast_wid;
    xpos += infocast_wid;
    xwid -= infocast_wid;
  }

  // shrink the viewer height if not in fullscreen
  if(!show_fullscreen) {
    int datafld_hgt = h() * pct_data_hgt;
    yhgt -= datafld_hgt;
  }

  mviewer->resize(xpos, ypos, xwid, yhgt);


  // Part 6: Adjust the extents of the DATA block of widgets
  // ------------------------------------------------------------
  if(show_fullscreen)
    hideDataFields();
  else {
    showDataFields();
  }
  double dh = h() * pct_data_hgt;
  double dy = h()-dh;
  double dw = w();

  double fld_hgt = dh * 0.24; // height of data field output

  if(dw < 900) {
    resizeDataText(6);
    m_scope_value->label("Val:");
  }
  else if(dw < 1100) {
    resizeDataText(8);
    m_scope_value->label("Val:");
  }
  else if(dw < 1400) {
    resizeDataText(10);
    m_scope_value->label("Valx:");
  }
  else {
    resizeDataText(12);
    m_scope_value->label("Value:");
  }

  // height = 150 is the "baseline" design. All adjustments from there.
  double row1 = dy + 10 * (dh/150);
  double row2 = dy + 55 * (dh/150);
  double row3 = dy + 100 * (dh/150);

  // Calculate the Data columns and wids

  int col_offset = 0;
  if(m_icast_settings.getInfoCastViewable() &&
     (m_icast_settings.getContentMode() == "realmcast"))
    col_offset = 140;
  
  
  int col1_pos = 60 + col_offset;
  int col1_wid = 150;
  
  int col2_pos = col1_pos + col1_wid + 50;
  int col2_wid = 55;
  
  int col3_pos = col2_pos + col2_wid + 50;
  int col3_wid = 80;

  int col4_pos = col3_pos + col3_wid + 50;
  int col4_wid = 50;

  int col5_pos = col4_pos + col4_wid + 60;
  int col5_wid = 50;

  int col6_pos = col5_pos + col5_wid + 50;
  int col6_wid = 55;

  int lft_min_wid = col6_pos + col6_wid + 20;
  int rgt_min_wid = m_button_cols * 110 + 10;

  int extra_wid = w() - (lft_min_wid + rgt_min_wid);

  int extra_rgt = extra_wid / 2;
  int max_extra_rgt = (m_button_cols * 75);
  if(extra_rgt > max_extra_rgt)
    extra_rgt = max_extra_rgt;

  int extra_lft = extra_wid - extra_rgt;
  int dwid = extra_lft / 6;

  col2_pos += (dwid * 1);
  col3_pos += (dwid * 2);
  col4_pos += (dwid * 3);
  col5_pos += (dwid * 4);
  col6_pos += (dwid * 5);
  col1_wid += dwid;
  col2_wid += dwid;
  col3_wid += dwid;
  col4_wid += dwid;
  col5_wid += dwid;
  col6_wid += dwid;

  int extra_bwid = 0;
  if(m_button_cols > 0)
    extra_bwid = (extra_rgt / (int)(m_button_cols));

  m_rc_button_src->resize(10, row1, 40, fld_hgt);
  m_rc_button_com->resize(55, row1, 40, fld_hgt);
  m_rc_button_utc->resize(100, row1, 40, fld_hgt);

  m_rc_button_wrp->resize(10, row3, 60, fld_hgt);
  m_rc_button_sub->resize(10, row2, 60, fld_hgt);
  m_rc_button_msk->resize(80, row2, 60, fld_hgt);
  m_rc_button_trc->resize(80, row3, 60, fld_hgt);
    
  int button_wid  = 100 + extra_bwid;
  int col_b1_pos  = w() - (button_wid + 10);  
  int col_b3_pos  = w() - ((button_wid + 10) * 2);  
  int col_b5_pos  = w() - ((button_wid + 10) * 3);  
  int col_b7_pos  = w() - ((button_wid + 10) * 4);  
  int col_b9_pos  = w() - ((button_wid + 10) * 5);  
  int col_b11_pos = w() - ((button_wid + 10) * 6);  
  int col_b13_pos = w() - ((button_wid + 10) * 7);  
  int col_b15_pos = w() - ((button_wid + 10) * 8);  
  int col_b17_pos = w() - ((button_wid + 10) * 9);  
  int col_b19_pos = w() - ((button_wid + 10) * 10);  
  
  int col1_xwid = (col2_pos + col2_wid) - col1_pos;
  int col4_xwid = w() - col4_pos - 10;
  
  v_nam->resize(col1_pos, row1, col1_wid, fld_hgt); 
  v_typ->resize(col1_pos, row2, col1_wid, fld_hgt); 

  x_mtr->resize(col2_pos, row1, col2_wid, fld_hgt); 
  y_mtr->resize(col2_pos, row2, col2_wid, fld_hgt); 

  v_lat->resize(col3_pos, row1, col3_wid, fld_hgt); 
  v_lon->resize(col3_pos, row2, col3_wid, fld_hgt); 

  v_spd->resize(col4_pos, row1, col4_wid, fld_hgt); 
  v_crs->resize(col4_pos, row2, col4_wid, fld_hgt); 

  v_dep->resize(col5_pos, row1, col5_wid, fld_hgt); 
  v_ais->resize(col5_pos, row2, col5_wid, fld_hgt); 

  time->resize(col6_pos, row1, col6_wid, fld_hgt); 
  warp->resize(col6_pos, row2, col6_wid, fld_hgt); 
  
  m_scope_variable->resize(col1_pos, row3, col1_xwid, fld_hgt); 
  m_scope_time->resize(col3_pos, row3, col3_wid, fld_hgt); 
  m_scope_value->resize(col4_pos, row3, col4_xwid, fld_hgt); 

  m_user_button_1->resize(col_b1_pos, row1, button_wid, fld_hgt);
  m_user_button_2->resize(col_b1_pos, row2, button_wid, fld_hgt);
  m_user_button_3->resize(col_b3_pos, row1, button_wid, fld_hgt);
  m_user_button_4->resize(col_b3_pos, row2, button_wid, fld_hgt);
  m_user_button_5->resize(col_b5_pos, row1, button_wid, fld_hgt);
  m_user_button_6->resize(col_b5_pos, row2, button_wid, fld_hgt);

  m_user_button_7->resize(col_b7_pos, row1, button_wid, fld_hgt);
  m_user_button_8->resize(col_b7_pos, row2, button_wid, fld_hgt);
  m_user_button_9->resize(col_b9_pos, row1, button_wid, fld_hgt);
  m_user_button_10->resize(col_b9_pos, row2, button_wid, fld_hgt);
  m_user_button_11->resize(col_b11_pos, row1, button_wid, fld_hgt);
  m_user_button_12->resize(col_b11_pos, row2, button_wid, fld_hgt);
  m_user_button_13->resize(col_b13_pos, row1, button_wid, fld_hgt);
  m_user_button_14->resize(col_b13_pos, row2, button_wid, fld_hgt);
  m_user_button_15->resize(col_b15_pos, row1, button_wid, fld_hgt);
  m_user_button_16->resize(col_b15_pos, row2, button_wid, fld_hgt);
  m_user_button_17->resize(col_b17_pos, row1, button_wid, fld_hgt);
  m_user_button_18->resize(col_b17_pos, row2, button_wid, fld_hgt);
  m_user_button_19->resize(col_b19_pos, row1, button_wid, fld_hgt);
  m_user_button_20->resize(col_b19_pos, row2, button_wid, fld_hgt);
}

