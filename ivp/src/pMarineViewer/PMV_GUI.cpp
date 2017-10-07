/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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

  m_cmd_gui = 0;
  m_curr_time = 0;
  m_clear_stale_timestamp = 0;

  m_cmd_gui_start_wid = 550;
  m_cmd_gui_start_hgt = 300;
  m_cmd_gui_start_bpost_cnt = 0;
  m_cmd_gui_start_cpost_cnt = 0;
  m_cmd_gui_start_show_posts = true;
  
  mviewer   = new PMV_Viewer(0, 0, 1, 1);
  m_mviewer = mviewer;

  // Configure the AppCasting Browsing Widgets
  m_brw_nodes = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_brw_nodes->callback(cb_SelectNode, 0);
  m_brw_nodes->textfont(FL_COURIER);
  m_brw_nodes->box(FL_DOWN_BOX);
  m_brw_nodes->clear_visible_focus();

  m_brw_procs = new MY_Fl_Hold_Browser(0, 0, 1, 1); 
  m_brw_procs->callback(cb_SelectProc, 0);
  m_brw_procs->textfont(FL_COURIER);
  m_brw_procs->box(FL_DOWN_BOX);
  m_brw_procs->clear_visible_focus();

  m_brw_casts = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_brw_casts->clear_visible_focus();
  m_brw_casts->textfont(FL_COURIER);
  m_brw_casts->box(FL_DOWN_BOX);
  m_brw_casts->clear_visible_focus();
  
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
  
  m_user_button_1 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_2 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_3 = new Fl_Button(0, 0, 1, 1, "Disabled");
  m_user_button_4 = new Fl_Button(0, 0, 1, 1, "Disabled");

  m_user_button_1->clear_visible_focus();
  m_user_button_2->clear_visible_focus();
  m_user_button_3->clear_visible_focus();
  m_user_button_4->clear_visible_focus();

  m_user_button_1->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)1);
  m_user_button_2->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)2);
  m_user_button_3->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)3);
  m_user_button_4->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)4);
  
  m_user_button_1->hide();
  m_user_button_2->hide();
  m_user_button_3->hide();
  m_user_button_4->hide();

  augmentMenu();

  this->end();
  this->resizable(this);
  this->show();
  resizeWidgets();
}

//----------------------------------------------------------
// Procedure: augmentMenu

void PMV_GUI::augmentMenu()
{
  m_menubar->add("BackView/full_screen=true",    0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)30,
		 FL_MENU_RADIO);
  m_menubar->add("BackView/full_screen=false",   0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)31,
		 FL_MENU_RADIO);
  m_menubar->add("BackView/    Toggle Full-Screen", 'f',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)32,
		 FL_MENU_DIVIDER);

  m_menubar->add("AppCasting/appcast_viewable=true",   0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)40,
		 FL_MENU_RADIO);    
  m_menubar->add("AppCasting/appcast_viewable=false",  0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)41,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/    Toggle AppCasting", 'a',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)42,
		 FL_MENU_DIVIDER);

  m_menubar->add("AppCasting/AppCasting Pane Wider",   FL_SHIFT+FL_Left,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)100, 0); 
  m_menubar->add("AppCasting/AppCasting Pane Thinner", FL_SHIFT+FL_Right,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)101, 0); 

  m_menubar->add("AppCasting/AppCasting Pane Taller",  FL_SHIFT+FL_Up,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)200, 0); 
  m_menubar->add("AppCasting/AppCasting Pane Shorter", FL_SHIFT+FL_Down,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)201,
		 FL_MENU_DIVIDER); 

  m_menubar->add("AppCasting/AppCasting Pane Wider (Alt)", FL_CTRL+FL_ALT+FL_Left,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)100, 0); 
  m_menubar->add("AppCasting/AppCasting Pane Thinner (Alt)", FL_CTRL+FL_ALT+FL_Right,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)101, 0); 

  m_menubar->add("AppCasting/AppCasting Pane Taller (Alt)", FL_CTRL+FL_ALT+FL_Up,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)200, 0); 
  m_menubar->add("AppCasting/AppCasting Pane Shorter (Alt)", FL_CTRL+FL_ALT+FL_Down,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)201,
		 FL_MENU_DIVIDER); 

  m_menubar->add("AppCasting/refresh_mode=paused",    FL_CTRL+' ',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)50,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/refresh_mode=events",    FL_CTRL+'e',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)51,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/refresh_mode=streaming", FL_CTRL+'s',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)52,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("AppCasting/nodes_font_size=xlarge",  0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)64,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/nodes_font_size=large",  0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)63,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/nodes_font_size=medium", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)62,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/nodes_font_size=small",  0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)61,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/nodes_font_size=xsmall", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)60,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  m_menubar->add("AppCasting/procs_font_size=xlarge", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)74,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/procs_font_size=large",  0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)73,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/procs_font_size=medium", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)72,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/procs_font_size=small",  0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)71,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/procs_font_size=xsmall", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)70,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  m_menubar->add("AppCasting/appcast_font_size=xlarge", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)84,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_font_size=large",  0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)83,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_font_size=medium", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)82,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_font_size=small",  0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)81,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_font_size=xsmall", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)80,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/appcast_font_size bigger",  '}',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)89,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/appcast_font_size smaller", '{',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)88,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  m_menubar->add("AppCasting/appcast_color_scheme=white",     0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)300,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_color_scheme=indigo",      0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)301,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_color_scheme=beige",       0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)302,
		 FL_MENU_RADIO);
  m_menubar->add("AppCasting/    Toggle Color Scheme", FL_ALT+'a',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)310,
		 FL_MENU_DIVIDER);

  m_menubar->add("AppCasting/AppCast Window Width/wider", FL_CTRL+FL_ALT+FL_Left,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)100, 0); 
  m_menubar->add("AppCasting/AppCast Window Width/thinner", FL_CTRL+FL_ALT+FL_Right,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)101, 0); 
  m_menubar->add("AppCasting/AppCast Window Width/reset", FL_CTRL+FL_ALT+'a',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)102,
		 FL_MENU_DIVIDER); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=20", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)120,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=25", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)125,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=30", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)130,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=35", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)135,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=40", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)140,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=45", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)145,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=50", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)150,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=55", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)155,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=60", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)160,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=65", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)165,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Width/appcast_width=70", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)170,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  m_menubar->add("AppCasting/AppCast Window Height/taller",  FL_CTRL+FL_ALT+FL_Up,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)200, 0); 
  m_menubar->add("AppCasting/AppCast Window Height/shorter", FL_CTRL+FL_ALT+FL_Down,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)201, 0); 
  m_menubar->add("AppCasting/AppCast Window Height/reset",   FL_CTRL+FL_ALT+'a',
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)102,
		 FL_MENU_DIVIDER); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=90", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)290,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=85", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)285,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=80", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)280,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=75", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)275,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=70", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)270,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=65", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)265,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=60", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)260,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=55", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)255,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=50", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)250,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=45", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)245,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=40", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)240,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=35", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)235,
		 FL_MENU_RADIO); 
  m_menubar->add("AppCasting/AppCast Window Height/appcast_height=30", 0,
		 (Fl_Callback*)PMV_GUI::cb_AppCastSetting, (void*)230,
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
// Procedure: resize

void PMV_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgets();
}

//----------------------------------------------------------
// Procedure: addButton

bool PMV_GUI::addButton(string btype, string svalue) 
{
  if((btype != "button_one") && (btype != "button_two") &&
     (btype != "button_three") && (btype != "button_four"))
    return(false);
  
  // Set the default label if none is provided in the svalue.
  // The default is the *current* value of the label.
  string button_label = m_user_button_1->label();
  if(btype == "button_two")
    button_label = m_user_button_2->label();
  else if(btype == "button_three")
    button_label = m_user_button_3->label();
  else if(btype == "button_four")
    button_label = m_user_button_4->label();

  bool ok_line = true;
  vector<string> svector = parseStringQ(svalue, '#');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = stripBlankEnds(biteString(svector[i], '='));
    string value = stripBlankEnds(svector[i]);
    if(param == "") 
      ok_line = false;
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

  if(btype == "button_one") { 
    m_user_button_1->copy_label(button_label.c_str());
    m_user_button_1->show();
    m_user_button_1->redraw();
  }    
  else if (btype == "button_two") {
    m_user_button_2->copy_label(button_label.c_str());
    m_user_button_2->show();
    m_user_button_2->redraw();
  }
  else if (btype == "button_three") {
    m_user_button_3->copy_label(button_label.c_str());
    m_user_button_3->show();
    m_user_button_3->redraw();
  }
  else if (btype == "button_four") {
    m_user_button_4->copy_label(button_label.c_str());
    m_user_button_4->show();
    m_user_button_4->redraw();
  }

  return(ok_line);
}

//----------------------------------------------------------
// Procedure: addAction

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
// Procedure: handle
//      Note: As it stands, this method could be eliminated entirely, and the 
//            default behavior of the parent class should work fine. But if
//            we want to tinker with event handling, this method is the place.

int PMV_GUI::handle(int event) 
{
  return(Fl_Window::handle(event));
}

//----------------------------------------------------------
// Procedure: showingAppCasts()

bool PMV_GUI::showingAppCasts() const
{
  if(m_ac_settings.viewable("full_screen"))
    return(false);
  if(!m_ac_settings.viewable("appcast_viewable"))
    return(false);
  
  return(true);
}

//----------------------------------------- augmentTitle
void PMV_GUI::augmentTitle(string ip_str)
{
  string new_title = m_title_base + " " + ip_str;
  label(new_title.c_str());
}

//----------------------------------------------------
// Procedure: syncNodesAtoB
//   Purpose: Used when the user toggles the "active vehicle" shown in the 
//            panes at the bottom of the window. If the new active vehicle
//            is also a known appcast node, adjust the appcast browsers
//            accordingly.

bool PMV_GUI::syncNodesAtoB()
{
  string vname = mviewer->getStringInfo("active_vehicle_name");

  bool valid_node = m_repo->setCurrentNode(vname);
  if(!valid_node)
    return(false);

  updateNodes(true);
  updateProcs(true);
  updateAppCast();
  return(true);
}

//----------------------------------------------------
// Procedure: syncNodesBtoA
//   Purpose: Used when the user selects a new new in the appcast browsers.
//            If the new node is a known vehicle, i.e., we have received
//            node reports for it, then make it the active vehicle in the 
//            panes at the bottom of the window. 

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
// Procedure: clearGeoShapes

void PMV_GUI::clearGeoShapes(string vname, string shape, string stype)
{
  mviewer->clearGeoShapes(vname, shape, stype);
}

//----------------------------------------- UpdateXY
void PMV_GUI::updateXY() 
{
  double dwarp = GetMOOSTimeWarp();
  string time_str = doubleToString(m_curr_time, 1);
  time->value(time_str.c_str());

  string scope_var  = mviewer->getStringInfo("scope_var");
  string scope_time = mviewer->getStringInfo("scope_time");
  string scope_val  = mviewer->getStringInfo("scope_val");
  string swarp      = dstringCompact(doubleToString(dwarp,2));

  m_scope_variable->value(scope_var.c_str());
  m_scope_time->value(scope_time.c_str());
  m_scope_value->value(scope_val.c_str());

  string vname = mviewer->getStringInfo("active_vehicle_name");

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

  warp->value(swarp.c_str());
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
  //v_range->value(range.c_str());
  //v_bearing->value(bearing.c_str());

  warp->redraw();
  v_nam->redraw();
  v_typ->redraw();
  v_spd->redraw();
  v_dep->redraw();
  v_ais->redraw();
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

//----------------------------------------- AppCastSetting
inline void PMV_GUI::cb_AppCastSetting_i(unsigned int v) {  
  // Reset all appcast panes to orig proportions
  if(v==102)  {
    setRadioCastAttrib("appcast_height", "70");
    setRadioCastAttrib("appcast_width",  "30");
  }
  // Handle fullscreen mode switching
  else if(v==30)  setRadioCastAttrib("full_screen", "true");
  else if(v==31)  setRadioCastAttrib("full_screen", "false");
  else if(v==32)  setRadioCastAttrib("full_screen", "toggle");
  // Handle appcast view mode switching
  else if(v==40)  setRadioCastAttrib("appcast_viewable", "true");
  else if(v==41)  setRadioCastAttrib("appcast_viewable", "false");
  else if(v==42)  setRadioCastAttrib("appcast_viewable", "toggle");
  // Handle appcast refresh setting
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
  // Handle appcast browser pane font size
  else if(v==80)  setRadioCastAttrib("appcast_font_size", "xsmall");
  else if(v==81)  setRadioCastAttrib("appcast_font_size", "small");
  else if(v==82)  setRadioCastAttrib("appcast_font_size", "medium");
  else if(v==83)  setRadioCastAttrib("appcast_font_size", "large");
  else if(v==84)  setRadioCastAttrib("appcast_font_size", "xlarge");
  else if(v==88)  setRadioCastAttrib("appcast_font_size", "smaller");
  else if(v==89)  setRadioCastAttrib("appcast_font_size", "bigger");
  // Handle pane width relative adjustment
  else if(v==100) setRadioCastAttrib("appcast_width", "delta:-5");
  else if(v==101) setRadioCastAttrib("appcast_width", "delta:5");
  // Handle pane width explicit setting
  else if(v==120) setRadioCastAttrib("appcast_width", "20");
  else if(v==125) setRadioCastAttrib("appcast_width", "25");
  else if(v==130) setRadioCastAttrib("appcast_width", "30");
  else if(v==135) setRadioCastAttrib("appcast_width", "35");
  else if(v==140) setRadioCastAttrib("appcast_width", "40");
  else if(v==145) setRadioCastAttrib("appcast_width", "45");
  else if(v==150) setRadioCastAttrib("appcast_width", "50");
  else if(v==155) setRadioCastAttrib("appcast_width", "55");
  else if(v==160) setRadioCastAttrib("appcast_width", "60");
  else if(v==165) setRadioCastAttrib("appcast_width", "65");
  else if(v==170) setRadioCastAttrib("appcast_width", "70");
  // Handle AppCast pane height relative adjustment
  else if(v==200) setRadioCastAttrib("appcast_height", "delta:5");
  else if(v==201) setRadioCastAttrib("appcast_height", "delta:-5");
  // Handle AppCast pane height explicit setting
  else if(v==230) setRadioCastAttrib("appcast_height", "30");
  else if(v==235) setRadioCastAttrib("appcast_height", "35");
  else if(v==240) setRadioCastAttrib("appcast_height", "40");
  else if(v==245) setRadioCastAttrib("appcast_height", "45");
  else if(v==250) setRadioCastAttrib("appcast_height", "50");
  else if(v==255) setRadioCastAttrib("appcast_height", "55");
  else if(v==260) setRadioCastAttrib("appcast_height", "60");
  else if(v==265) setRadioCastAttrib("appcast_height", "65");
  else if(v==270) setRadioCastAttrib("appcast_height", "70");
  else if(v==275) setRadioCastAttrib("appcast_height", "75");
  else if(v==280) setRadioCastAttrib("appcast_height", "80");
  else if(v==285) setRadioCastAttrib("appcast_height", "85");
  else if(v==290) setRadioCastAttrib("appcast_height", "90");

  else if(v==300) setRadioCastAttrib("appcast_color_scheme", "white");
  else if(v==301) setRadioCastAttrib("appcast_color_scheme", "indigo");
  else if(v==302) setRadioCastAttrib("appcast_color_scheme", "beige");
  else if(v==310) setRadioCastAttrib("appcast_color_scheme", "toggle");

  resizeWidgets();
  redraw();
}

void PMV_GUI::cb_AppCastSetting(Fl_Widget* o, unsigned int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_AppCastSetting_i(v);
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
    updateNodes(true);
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
    updateNodes(true);
    updateProcs(true);
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
// Procedure: clearStaleVehicles

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
    updateNodes(true);
    updateProcs(true);
  }

  if(vnames.size() > 0)
    m_clear_stale_timestamp = m_curr_time;
  return(true);
}

//----------------------------------------------------
// Procedure: cb_SelectNode_i()

inline void PMV_GUI::cb_SelectNode_i() 
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
  updateNodes();
  updateProcs(true);
  updateAppCast();
  syncNodesBtoA();
  updateXY();
}

//----------------------------------------------------
// Procedure: cb_SelectNode()

void PMV_GUI::cb_SelectNode(Fl_Widget* o, long) 
{
  ((PMV_GUI*)(o->parent()->user_data()))->cb_SelectNode_i();
}


//----------------------------------------------------
// Procedure: cb_SelectProci()

inline void PMV_GUI::cb_SelectProc_i() 
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

  updateProcs();
  updateAppCast();
}

//----------------------------------------------------
// Procedure: cb_SelectProc()

void PMV_GUI::cb_SelectProc(Fl_Widget* o, long) 
{
  ((PMV_GUI*)(o->parent()->user_data()))->cb_SelectProc_i();
}

//-------------------------------------------------------------------
// Procedure: getPendingVar

string PMV_GUI::getPendingVar(unsigned int ix)
{
  if(ix < m_pending_vars.size())
    return(m_pending_vars[ix]);
  else
    return("");
}

//-------------------------------------------------------------------
// Procedure: getPendingVal

string PMV_GUI::getPendingVal(unsigned int ix)
{
  if(ix < m_pending_vals.size())
    return(m_pending_vals[ix]);
  else
    return("");
}

//-------------------------------------------------------------------
// Procedure: clearPending

void PMV_GUI::clearPending()
{
  m_pending_vars.clear();
  m_pending_vals.clear();
}

//-------------------------------------------------------------------
// Procedure: pushPending

void PMV_GUI::pushPending(string var, string val)
{
  m_pending_vars.push_back(var);
  m_pending_vals.push_back(val);
}

//-------------------------------------------------------------------
// Procedure: addScopeVariable
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
// Procedure: addMousePoke

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
// Procedure: addReferenceVehicle
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
// Procedure: addFilterVehicle

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
// Procedure: removeFilterVehicle

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

    cout << "closing wid=" << wid << ", hgt=" << hgt << endl;
    
    delete(m_cmd_gui);
    m_cmd_gui = 0;
  }
}

//---------------------------------------------------------- 
// Procedure: updateNodes()

void PMV_GUI::updateNodes(bool clear) 
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
// Procedure: updateProcs()

void PMV_GUI::updateProcs(bool clear) 
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
// Procedure: updateAppCast

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
  for(unsigned int i=0; i<svector.size(); i++) {
    m_brw_casts->add(removeTermColors(svector[i]).c_str());
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
// Procedure: updateRadios

void PMV_GUI::updateRadios()
{
  MarineVehiGUI::updateRadios();
  setRadioCastAttrib("nodes_font_size");
  setRadioCastAttrib("procs_font_size");
  setRadioCastAttrib("appcast_font_size");
  setRadioCastAttrib("appcast_width");
  setRadioCastAttrib("appcast_width");
  setRadioCastAttrib("full_screen");
  setRadioCastAttrib("refresh_mode");
  setRadioCastAttrib("appcast_viewable");
}


//----------------------------------------------------- 
// Procedure: setMenuItemColors

void PMV_GUI::setMenuItemColors()
{
  MarineVehiGUI::setMenuItemColors();
  setMenuItemColor("AppCasting/appcast_viewable=true");
  setMenuItemColor("AppCasting/appcast_viewable=false");

  setMenuItemColor("AppCasting/refresh_mode=paused");
  setMenuItemColor("AppCasting/refresh_mode=events");
  setMenuItemColor("AppCasting/refresh_mode=streaming");

  setMenuItemColor("AppCasting/nodes_font_size=xlarge");
  setMenuItemColor("AppCasting/nodes_font_size=large");
  setMenuItemColor("AppCasting/nodes_font_size=medium");
  setMenuItemColor("AppCasting/nodes_font_size=small");
  setMenuItemColor("AppCasting/nodes_font_size=xsmall");

  setMenuItemColor("AppCasting/procs_font_size=xlarge");
  setMenuItemColor("AppCasting/procs_font_size=large");
  setMenuItemColor("AppCasting/procs_font_size=medium");
  setMenuItemColor("AppCasting/procs_font_size=small");
  setMenuItemColor("AppCasting/procs_font_size=xsmall");

  setMenuItemColor("AppCasting/appcast_font_size=xlarge");
  setMenuItemColor("AppCasting/appcast_font_size=large");
  setMenuItemColor("AppCasting/appcast_font_size=medium");
  setMenuItemColor("AppCasting/appcast_font_size=small");
  setMenuItemColor("AppCasting/appcast_font_size=xsmall");

  setMenuItemColor("AppCasting/appcast_color_scheme=white");
  setMenuItemColor("AppCasting/appcast_color_scheme=indigo");
  setMenuItemColor("AppCasting/appcast_color_scheme=beige");

  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=20");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=25");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=30");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=35");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=40");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=45");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=50");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=55");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=60");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=65");
  setMenuItemColor("AppCasting/AppCast Window Width/appcast_width=70");

  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=30");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=35");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=40");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=45");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=50");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=55");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=60");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=65");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=70");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=75");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=80");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=85");
  setMenuItemColor("AppCasting/AppCast Window Height/appcast_height=90");

  setMenuItemColor("BackView/full_screen=true");
  setMenuItemColor("BackView/full_screen=false");
}


//----------------------------------------------------- 
// Procedure: setRadioCastAttrib
//   Purpose: A way of setting the appcast attribute assureing that 
//            the GUI radio buttons are properly updated.
//  
//  Examples: "nodes_font_size", "xsmall"
//            "refresh_mode", "false"

bool PMV_GUI::setRadioCastAttrib(string attr, string value) 
{
  // If value="", no update attempted, just update menu radio button
  if(value != "") {
    bool handled = m_ac_settings.setParam(attr, value);
    if(!handled)
      return(false);
  }
  
  string menu = "AppCasting/";
  if(attr == "appcast_width")  menu += "AppCast Window Width/";
  if(attr == "appcast_height") menu += "AppCast Window Height/";
  if(attr == "full_screen")    menu =  "BackView/";
  if(attr == "refresh_mode")   m_repo->setRefreshMode(value);

  // Get latest setting of this attr (esp if this value is "toggle" etc)
  string new_value = m_ac_settings.strvalue(attr);
  // cout << "setRadioCastAttrib: attr:" << attr << " value:" << value 
  //  << " new_value:" << new_value << "]"<< endl;

  // Form for example, "AppCasting/nodes_font_size=small"
  string item_str = menu + attr + "=" + new_value;
  const Fl_Menu_Item *item = m_menubar->find_item(item_str.c_str());
  if(item)
    ((Fl_Menu_Item *)item)->setonly();

  resizeWidgets();
  redraw();
  return(true);
}


//-------------------------------------------------------------------
// Procedure: showDataFields

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

  string label_1 = m_user_button_1->label();
  string label_2 = m_user_button_2->label();
  string label_3 = m_user_button_3->label();
  string label_4 = m_user_button_4->label();
  if(label_1 != "Disabled")   m_user_button_1->show();
  if(label_2 != "Disabled")   m_user_button_2->show();
  if(label_3 != "Disabled")   m_user_button_3->show();
  if(label_4 != "Disabled")   m_user_button_4->show();
}

//-------------------------------------------------------------------
// Procedure: hideDataFields

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
  m_user_button_1->hide();
  m_user_button_2->hide();
  m_user_button_3->hide();
  m_user_button_4->hide();
}

//----------------------------------------------------------
// Procedure: resizeDataText

void PMV_GUI::resizeDataText(int wsize) 
{
  m_user_button_1->labelsize(wsize);
  m_user_button_2->labelsize(wsize);
  m_user_button_3->labelsize(wsize);
  m_user_button_4->labelsize(wsize);
  
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
  bool show_fullscreen = m_ac_settings.viewable("full_screen");
  bool show_appcasting = m_ac_settings.viewable("appcast_viewable");

  // Part 1: Figure out the basic pane extents.
  // ================================================================
  // This is the default extent configuration so long as any minimal
  // constraints are not violated.

  double pct_cast_wid = m_ac_settings.geosize("appcast_width")  / 100;
  double pct_cast_hgt = m_ac_settings.geosize("appcast_height") / 100;
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

  // Make sure the appcasting pane is at least 300
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

  // Part 3: Adjust the extents of the APPCAST browsers
  // ================================================================  
  if(!show_appcasting) {
    m_brw_nodes->hide();
    m_brw_procs->hide();
    m_brw_casts->hide();
  }   
  else {
    m_brw_nodes->show();
    m_brw_procs->show();
    m_brw_casts->show();

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
    
    m_brw_nodes->resize(bx, menu_hgt, node_wid, node_hgt);
    m_brw_procs->resize(node_wid, menu_hgt, proc_wid, proc_hgt);
    m_brw_casts->resize(bx, by+node_hgt, cast_wid, cast_hgt);
    
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
    
    string nodes_font_size   = m_ac_settings.strvalue("nodes_font_size");
    string procs_font_size   = m_ac_settings.strvalue("procs_font_size");
    string appcast_font_size = m_ac_settings.strvalue("appcast_font_size");
    
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
    
    if(appcast_font_size == "xsmall")      m_brw_casts->textsize(font_xsmall);
    else if(appcast_font_size == "small")  m_brw_casts->textsize(font_small);
    else if(appcast_font_size == "medium") m_brw_casts->textsize(font_medium);
    else if(appcast_font_size == "large")  m_brw_casts->textsize(font_large);
    else                                   m_brw_casts->textsize(font_xlarge);
  }

  // Part 4: Adjust the color scheme of the APPCAST browsers
  // ================================================================  
  string appcast_color_scheme = m_ac_settings.strvalue("appcast_color_scheme");

  Fl_Color color_back = fl_rgb_color(255, 255, 255);  // white
  Fl_Color color_text = fl_rgb_color(0, 0, 0);        // black
  m_color_runw = fl_rgb_color(205, 71, 71);           // redish
  m_color_cfgw = fl_rgb_color(50, 189, 149);          // greenish
  m_color_stlw = fl_rgb_color(125, 125, 0);           // yellowish

  if(appcast_color_scheme == "indigo") {
    color_back = fl_rgb_color(95, 117, 182);   // indigo-lighter (65,87,152)
    color_text = fl_rgb_color(255, 255, 255);  // white
    m_color_runw = fl_rgb_color(205, 71, 71);  // redish
    m_color_cfgw = fl_rgb_color(0, 159, 119);  // greenish
  }
  else if(appcast_color_scheme == "beige") {
    color_back = fl_rgb_color(223, 219, 195);   // beige
    color_text = fl_rgb_color(0, 0, 0);         // black
    m_color_runw = fl_rgb_color(205, 71, 71);   // redish
    m_color_cfgw = fl_rgb_color(0, 189, 149);   // greenish
  }

  m_brw_nodes->color(color_back);
  m_brw_nodes->textcolor(color_text);
  m_brw_nodes->position(0);

  m_brw_procs->color(color_back);
  m_brw_procs->textcolor(color_text);
  m_brw_procs->position(0);

  m_brw_casts->color(color_back);
  m_brw_casts->textcolor(color_text);
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
  if(show_appcasting && !show_fullscreen) {
    double appcast_wid = w() * pct_cast_wid;
    xpos += appcast_wid;
    xwid -= appcast_wid;
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

  if(dw < 800) {
    resizeDataText(10);
    m_scope_value->label("Val:");
  }
  else {
    resizeDataText(12);
    m_scope_value->label("Value:");
  }
  
  // height = 150 is the "baseline" design. All adjustments from there.
  double row1 = dy + 10 * (dh/150);
  double row2 = dy + 55 * (dh/150);
  double row3 = dy + 100 * (dh/150);

  v_nam->resize(dw*0.07, row1, dw*0.07, fld_hgt); 
  v_typ->resize(dw*0.07, row2, dw*0.07, fld_hgt); 
  x_mtr->resize(dw*0.20, row1, dw*0.06, fld_hgt); 
  y_mtr->resize(dw*0.20, row2, dw*0.06, fld_hgt); 
  v_lat->resize(dw*0.31, row1, dw*0.11, fld_hgt); 
  v_lon->resize(dw*0.31, row2, dw*0.11, fld_hgt); 
  v_spd->resize(dw*0.49, row1, dw*0.06, fld_hgt); 
  v_crs->resize(dw*0.49, row2, dw*0.06, fld_hgt); 
  v_dep->resize(dw*0.62, row1, dw*0.07, fld_hgt); 
  v_ais->resize(dw*0.62, row2, dw*0.07, fld_hgt); 
  time->resize(dw*0.75, row1, dw*0.06, fld_hgt); 
  warp->resize(dw*0.75, row2, dw*0.06, fld_hgt); 
  
  m_scope_variable->resize(dw*0.07, row3, dw*0.1, fld_hgt); 
  m_scope_time->resize(dw*0.20, row3, dw*0.1, fld_hgt); 
  m_scope_value->resize(dw*0.35, row3, dw*0.65, fld_hgt); 

  m_user_button_1->resize(dw*0.91, row1, dw*0.09, fld_hgt);
  m_user_button_2->resize(dw*0.91, row2, dw*0.09, fld_hgt);
  m_user_button_3->resize(dw*0.82, row1, dw*0.09, fld_hgt);
  m_user_button_4->resize(dw*0.82, row2, dw*0.09, fld_hgt);
}







