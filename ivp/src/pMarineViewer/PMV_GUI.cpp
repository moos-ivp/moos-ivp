/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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


#include <FL/fl_ask.H>
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

  int info_size = 12;
  int small_info_size = 11;
  
  m_curr_time      = 0;
  m_prev_out_time  = 0;

  mviewer  = new PMV_Viewer(0, 30, w(), h()-140);
  cmviewer = mviewer;

  v_nam = new Fl_Output(w()*0.07, h()-100, w()*0.07, 20, "VName:"); 
  //v_nam = new Fl_Output(60, h()-100, 70, 20, "VName:"); 
  v_nam->set_output();
  v_nam->textsize(info_size); 
  v_nam->labelsize(info_size);

  v_typ = new Fl_Output(w()*0.07, h()-70, w()*0.07, 20, "VType:"); 
  //v_typ = new Fl_Output(60, h()-70, 70, 20, "VType:"); 
  v_typ->set_output();
  v_typ->textsize(info_size); 
  v_typ->labelsize(info_size);
  v_typ->value("unknown");

  x_mtr = new Fl_Output(w()*0.20, h()-100, w()*0.06, 20, "X(m):"); 
  //x_mtr = new Fl_Output(175, h()-100, 75, 20, "X(m):"); 
  x_mtr->set_output();
  x_mtr->textsize(info_size); 
  x_mtr->labelsize(info_size);

  y_mtr = new Fl_Output(w()*0.20, h()-70, w()*0.06, 20, "Y(m):"); 
  //y_mtr = new Fl_Output(175, h()-70, 75, 20, "Y(m):"); 
  y_mtr->set_output();
  y_mtr->textsize(info_size); 
  y_mtr->labelsize(info_size);

  v_lat = new Fl_Output(w()*0.31, h()-100, w()*0.11, 20, "Lat:"); 
  //v_lat = new Fl_Output(295, h()-100, 80, 20, "Lat:"); 
  v_lat->set_output();
  v_lat->textsize(info_size); 
  v_lat->labelsize(info_size);

  v_lon = new Fl_Output(w()*0.31, h()-70, w()*0.11, 20, "Long:"); 
  //v_lon = new Fl_Output(295, h()-70, 80, 20, "Long:"); 
  v_lon->set_output();
  v_lon->textsize(info_size); 
  v_lon->labelsize(info_size);

  v_spd = new Fl_Output(w()*0.49, h()-100, w()*0.06, 20, "Spd:"); 
  //v_spd = new Fl_Output(440, h()-100, 50, 20, "Spd(m/s):"); 
  v_spd->set_output();
  v_spd->textsize(info_size); 
  v_spd->labelsize(info_size);

  v_crs = new Fl_Output(w()*0.49, h()-70, w()*0.06, 20, "Hdg:"); 
  //v_crs = new Fl_Output(440, h()-70, 50, 20, "Heading:"); 
  v_crs->set_output();
  v_crs->textsize(info_size); 
  v_crs->labelsize(info_size);

  v_dep = new Fl_Output(w()*0.62, h()-100, w()*0.07, 20, "Depth(m):"); 
  //v_dep = new Fl_Output(565, h()-100, 50, 20, "Dep(m):"); 
  v_dep->set_output();
  v_dep->textsize(info_size); 
  v_dep->labelsize(info_size);

  v_ais = new Fl_Output(w()*0.62, h()-70, w()*0.07, 20, "Report-Age:"); 
  //v_ais = new Fl_Output(565, h()-70, 50, 20, "Report-Age:"); 
  v_ais->set_output();
  v_ais->textsize(info_size); 
  v_ais->labelsize(info_size);

  time = new Fl_Output(w()*0.75, h()-100, w()*0.06, 20, "Time:"); 
  //time = new Fl_Output(660, h()-100, 60, 20, "Time:"); 
  time->set_output();
  time->textsize(info_size); 
  time->labelsize(info_size);

  warp = new Fl_Output(w()*0.75, h()-70, w()*0.06, 20, "Warp:"); 
  //warp = new Fl_Output(660, h()-70, 60, 20, "Warp:"); 
  warp->set_output();
  warp->textsize(info_size); 
  warp->labelsize(info_size);
  
#if 0
  v_range = new Fl_Output(785, h()-100, 60, 20, "Range:"); 
  v_range->set_output();
  v_range->textsize(info_size); 
  v_range->labelsize(info_size);

  v_bearing = new Fl_Output(785, h()-70, 60, 20, "Bearing:"); 
  v_bearing->set_output();
  v_bearing->textsize(info_size); 
  v_bearing->labelsize(info_size);
#endif  

  m_scope_variable = new Fl_Output(60, h()-30, 100, 20, "Variable:"); 
  m_scope_variable->set_output();
  m_scope_variable->textsize(small_info_size); 
  m_scope_variable->labelsize(info_size);

  m_scope_time = new Fl_Output(200, h()-30, 60, 20, "Time:"); 
  m_scope_time->set_output();
  m_scope_time->textsize(small_info_size); 
  m_scope_time->labelsize(info_size);

  m_scope_value = new Fl_Output(305, h()-30, w()-350, 20, "Value:"); 
  m_scope_value->set_output();
  m_scope_value->textsize(small_info_size); 
  m_scope_value->labelsize(info_size);

  int hgt_b  = 22;

  user_button_1 = new MY_Button(w()*0.91, h()-100, 
                                w()*0.09, hgt_b, "Disabled");
  user_button_2 = new MY_Button(w()*0.91, h()-70, 
                                w()*0.09, hgt_b, "Disabled");
  user_button_3 = new MY_Button(w()*0.82, h()-100, 
                                w()*0.09, hgt_b, "Disabled");
  user_button_4 = new MY_Button(w()*0.82, h()-70, 
                                w()*0.09, hgt_b, "Disabled");

  user_button_1->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)1);
  user_button_2->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)2);
  user_button_3->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)3);
  user_button_4->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)4);

  user_button_1->labelsize(10);
  user_button_2->labelsize(10);
  user_button_3->labelsize(10);
  user_button_4->labelsize(10);
  user_button_1->hide();
  user_button_2->hide();
  user_button_3->hide();
  user_button_4->hide();

  mbar->add("MOOS-Scope/Add Variable", 'a', 
	    (Fl_Callback*)PMV_GUI::cb_Scope, (void*)0, FL_MENU_DIVIDER);
    
  mbar->add("ClearHistory/All Vehicles", FL_CTRL+'9', 
	    (Fl_Callback*)PMV_GUI::cb_FilterOut, (void*)-1, FL_MENU_DIVIDER);
    
  this->end();
  this->resizable(this);
  this->show();
}

//----------------------------------------------------------
// Procedure: draw
//      Note: 
//            

void PMV_GUI::draw() 
{
  if((h() > 600) && (w() > 750)) {
    resizeWidgets(12);
    v_nam->label("VName:");
    v_typ->label("VType:");
    x_mtr->label("X(m):");
    y_mtr->label("Y(m):");
    v_lon->label("Long:");
    v_dep->label("Depth(m):");
    v_ais->label("Rpt-Age:");
  }
  else if((h() > 800) && (w() > 950)) {
    resizeWidgets(14);
    v_nam->label("VName:");
    v_typ->label("VType:");
    x_mtr->label("X(m):");
    y_mtr->label("Y(m):");
    v_lon->label("Long:");
    v_dep->label("Depth(m):");
    v_ais->label("Rpt-Age:");
  }
  else if((h() > 350) && (w() > 440)) {
    resizeWidgets(10);
    v_nam->label("Name:");
    v_typ->label("Type:");
    x_mtr->label("X:");
    y_mtr->label("Y:");
    v_lon->label("Lon:");
    v_dep->label("Dep:");
    v_ais->label("Age:");
    m_scope_variable->label("Var:");
  }
  else {
    resizeWidgets(8);
    v_nam->label("Name:");
    v_typ->label("Type:");
    x_mtr->label("X:");
    y_mtr->label("Y:");
    v_lon->label("Lon:");
    v_dep->label("Dep:");
    v_ais->label("Age:");
    m_scope_variable->label("Var:");
  }

  MarineVehiGUI::draw();
}

//----------------------------------------------------------
// Procedure: resizeWidgets

void PMV_GUI::resizeWidgets(int wsize) 
{
  user_button_1->labelsize(wsize+1);
  user_button_2->labelsize(wsize+1);
  user_button_3->labelsize(wsize+1);
  user_button_4->labelsize(wsize+1);
  
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
  m_scope_time->textsize(wsize);
  m_scope_value->textsize(wsize);
}

//----------------------------------------------------------
// Procedure: addButton
//      Note: 
//            
void PMV_GUI::addButton(string btype, string svalue) 
{
  if((btype != "button_one") && (btype != "button_two") &&
     (btype != "button_three") && (btype != "button_four"))
    return;

  // Set the default label if none is provided in the svalue.
  // The default is the *current* value of the label.
  string button_label = user_button_1->label();
  if(btype == "button_two")
    button_label = user_button_2->label();
  else if(btype == "button_three")
    button_label = user_button_3->label();
  else if(btype == "button_four")
    button_label = user_button_4->label();

  bool ok_line = true;
  vector<string> svector = parseString(svalue, '#');
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
    user_button_1->copy_label(button_label.c_str());
    user_button_1->show();
    user_button_1->redraw();
  }    
  else if (btype == "button_two") {
    user_button_2->copy_label(button_label.c_str());
    user_button_2->show();
    user_button_2->redraw();
  }
  else if (btype == "button_three") {
    user_button_3->copy_label(button_label.c_str());
    user_button_3->show();
    user_button_3->redraw();
  }
  else if (btype == "button_four") {
    user_button_4->copy_label(button_label.c_str());
    user_button_4->show();
    user_button_4->redraw();
  }
}

//----------------------------------------------------------
// Procedure: addAction
//      Note: 
//            
void PMV_GUI::addAction(string svalue, bool separator) 
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
      return;
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
    unsigned int index = pindex + i;
    string left  = truncString(vars[i], 25, "middle");
    string right = vals[i];
    //string right = truncString(vals[i], 25, "middle");
    string label = ("Action/" + left + "=" + right);
    label = padString(label, 25, false);
    if(key != "")
      label += (" <" + key + ">");
    if(separator)
      mbar->add(label.c_str(), 0, (Fl_Callback*)PMV_GUI::cb_DoAction, (void*)index, FL_MENU_DIVIDER);
    else
      mbar->add(label.c_str(), 0, (Fl_Callback*)PMV_GUI::cb_DoAction, (void*)index, 0);
  }
  mbar->redraw();
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
  double dwarp = GetMOOSTimeWarp();
  //if((m_curr_time - m_prev_out_time) < (0.25 * dwarp))
  //  return;

  m_prev_out_time = m_curr_time;

  string time_str = doubleToString(m_curr_time, 1);
  time->value(time_str.c_str());


  string scope_var  = mviewer->getStringInfo("scope_var");
  string scope_time = mviewer->getStringInfo("scope_time");
  string scope_val  = mviewer->getStringInfo("scope_val");

  m_scope_variable->value(scope_var.c_str());
  m_scope_time->value(scope_time.c_str());
  m_scope_value->value(scope_val.c_str());

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
    //v_range->value(" n/a");
    //v_bearing->value(" n/a");
    return;
  }

  string swarp = dstringCompact(doubleToString(dwarp,2));
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
  }
}

void PMV_GUI::cb_FilterOut(Fl_Widget* o, int v) {
  ((PMV_GUI*)(o->parent()->user_data()))->cb_FilterOut_i(v);
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
    mbar->add("MOOS-Scope/Toggle-Previous-Scope", '/', 
	      (Fl_Callback*)PMV_GUI::cb_Scope, (void*)1, FL_MENU_DIVIDER);
    mbar->add("MOOS-Scope/Cycle-Scope-Variables", FL_CTRL+'/', 
	      (Fl_Callback*)PMV_GUI::cb_Scope, (void*)2, FL_MENU_DIVIDER);
  }

  m_scope_vars.push_back(varname);
  unsigned int index = m_scope_vars.size()-1;
  
  string label = "MOOS-Scope/";
  label += (truncString(varname, 25, "middle"));
  mbar->add(label.c_str(), 0, 
	    (Fl_Callback*)PMV_GUI::cb_Scope, (void*)index, 0);
  
  mbar->redraw();
  return(true);
}

//-------------------------------------------------------------------
// Procedure: addMousePoke

void PMV_GUI::addMousePoke(string side, string key, string vardata_pair)
{
  if(!mviewer)
    return;

  if(side == "left") {
    mviewer->addMousePoke(key, vardata_pair);
    if(!vectorContains(m_left_mouse_keys, key)) {
      if(m_left_mouse_keys.size() == 0) {
	m_left_mouse_keys.push_back("no-action");
	mbar->add("Mouse-Context/Left/no-action", 0, 
		  (Fl_Callback*)PMV_GUI::cb_LeftContext, (void*)0, 
		  FL_MENU_RADIO);
      }
      m_left_mouse_keys.push_back(key);
      unsigned int index = m_left_mouse_keys.size()-1;
      string label = "Mouse-Context/Left/";
      label += (truncString(key, 40, "middle"));

      // If this was the first left context mode, make it the active
      if(m_left_mouse_keys.size() == 2) {
	mviewer->setLeftMouseKey(key);
	mbar->add(label.c_str(), 0, 
		  (Fl_Callback*)PMV_GUI::cb_LeftContext, 
		  (void*)index, FL_MENU_RADIO|FL_MENU_VALUE);
      }
      else {
	mbar->add(label.c_str(), 0, 
		  (Fl_Callback*)PMV_GUI::cb_LeftContext, 
		  (void*)index, FL_MENU_RADIO);
      }
      mbar->redraw();
    }
  }
  else if(side == "right") {
    mviewer->addMousePoke(key, vardata_pair);
    if(!vectorContains(m_right_mouse_keys, key)) {
      if(m_right_mouse_keys.size() == 0) {
	m_right_mouse_keys.push_back("no-action");
	mbar->add("Mouse-Context/Right/no-action", 0, 
		  (Fl_Callback*)PMV_GUI::cb_RightContext, (void*)0, 
		  FL_MENU_RADIO);
      }
      m_right_mouse_keys.push_back(key);
      unsigned int index = m_right_mouse_keys.size()-1;
      string label = "Mouse-Context/Right/";
      label += (truncString(key, 40, "middle"));
      // If this was the first right context mode, make it the active
      if(m_right_mouse_keys.size() == 2) {
	mviewer->setRightMouseKey(key);
	mbar->add(label.c_str(), 0, 
		  (Fl_Callback*)PMV_GUI::cb_RightContext, 
		  (void*)index, FL_MENU_RADIO|FL_MENU_VALUE);
      }
      else {
	mbar->add(label.c_str(), 0, 
		  (Fl_Callback*)PMV_GUI::cb_RightContext, 
		  (void*)index, FL_MENU_RADIO);
      }
      mbar->redraw();
    }
  }
}

//-------------------------------------------------------------------
// Procedure: addReferenceVehicle
//      NOte: Add the vehicle with the given name as one of the possible
//            reference vehicles. When none are given the datum is the
//            the default reference point. So when the first vehicle is
//            given, the datum is added as one of the menu choices. 

void PMV_GUI::addReferenceVehicle(string vehicle_name)
{
  // First check the current list of vehicles, ignore duplicates
  unsigned int i, vsize = m_reference_tags.size();
  for(i=0; i<vsize; i++) {
    if(vehicle_name == m_reference_tags[i])
      return;
  }

  // If the list is currently empty, create the "datum" as a choice first.
  if(vsize == 0) {
    m_reference_tags.push_back("bearing-absolute");
    string label = "ReferencePoint/Bearing-Absolute";
    mbar->add(label.c_str(), FL_CTRL+'a', 
	      (Fl_Callback*)PMV_GUI::cb_Reference, 
	      (void*)0, FL_MENU_RADIO|FL_MENU_VALUE);
    m_reference_tags.push_back("bearing-relative");
    label = "ReferencePoint/Bearing-Relative";
    mbar->add(label.c_str(), FL_CTRL+'r', 
	      (Fl_Callback*)PMV_GUI::cb_Reference, 
	      (void*)1, FL_MENU_DIVIDER|FL_MENU_RADIO);

    m_reference_tags.push_back("datum");
    label = "ReferencePoint/Datum";
    mbar->add(label.c_str(), 0, (Fl_Callback*)PMV_GUI::cb_Reference, 
	      (void*)2, FL_MENU_RADIO|FL_MENU_VALUE);
  }

  // Add the new vehicle name as a menu choice
  m_reference_tags.push_back(vehicle_name);
  int index = m_reference_tags.size()-1;
  string label = "ReferencePoint/";
  label += (truncString(vehicle_name, 25, "middle"));
  mbar->add(label.c_str(), 0, 
	    (Fl_Callback*)PMV_GUI::cb_Reference, (void*)index, FL_MENU_RADIO);

  mbar->redraw();
}

//-------------------------------------------------------------------
// Procedure: addFilterVehicle

void PMV_GUI::addFilterVehicle(string vehicle_name)
{
  // First check the current list of vehicles, ignore duplicates
  unsigned int i, vsize = m_filter_tags.size();
  for(i=0; i<vsize; i++) {
    if(vehicle_name == m_filter_tags[i])
      return;
  }

  // Add the new vehicle name as a menu choice
  m_filter_tags.push_back(vehicle_name);
  unsigned int index = m_filter_tags.size()-1;
  string label = "ClearHistory/";
  label += (truncString(vehicle_name, 25, "middle"));
  mbar->add(label.c_str(), 0, 
	    (Fl_Callback*)PMV_GUI::cb_FilterOut, (void*)index, 0);

  mbar->redraw();
}

//-------------------------------------------------------------------
// Procedure: removeFilterVehicle

void PMV_GUI::removeFilterVehicle(string vehicle_name)
{
  // First check the current list of vehicles, ignore duplicates
  unsigned int i, vsize = m_filter_tags.size();
  for(i=0; i<vsize; i++) {
    if(vehicle_name == m_filter_tags[i])
      mbar->remove(i+1);
  }

  mbar->redraw();
}


