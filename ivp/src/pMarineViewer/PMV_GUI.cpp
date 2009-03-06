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
  m_prev_out_time  = 0;

  mviewer  = new PMV_Viewer(0, 30, w(), h()-100);
  cmviewer = mviewer;

  v_nam = new MY_LockableOutput(60, h()-60, 70, 20, "VName:"); 
  v_nam->textsize(info_size); 
  v_nam->labelsize(info_size);
  v_nam->setMutex(&m_pmv_mutex);

  v_typ = new MY_LockableOutput(60, h()-30, 70, 20, "VType:"); 
  v_typ->textsize(info_size); 
  v_typ->labelsize(info_size);
  v_typ->value("unknown");
  v_typ->setMutex(&m_pmv_mutex);

  x_mtr = new MY_LockableOutput(190, h()-60, 70, 20, "X(m):"); 
  x_mtr->textsize(info_size); 
  x_mtr->labelsize(info_size);
  x_mtr->setMutex(&m_pmv_mutex);

  y_mtr = new MY_LockableOutput(190, h()-30, 70, 20, "Y(m):"); 
  y_mtr->textsize(info_size); 
  y_mtr->labelsize(info_size);
  y_mtr->setMutex(&m_pmv_mutex);

  v_lat = new MY_LockableOutput(305, h()-60, 90, 20, "Lat:"); 
  v_lat->textsize(info_size); 
  v_lat->labelsize(info_size);
  v_lat->setMutex(&m_pmv_mutex);

  v_lon = new MY_LockableOutput(305, h()-30, 90, 20, "long:"); 
  v_lon->textsize(info_size); 
  v_lon->labelsize(info_size);
  v_lon->setMutex(&m_pmv_mutex);

  v_spd = new MY_LockableOutput(470, h()-60, 55, 20, "Spd(m/s):"); 
  v_spd->textsize(info_size); 
  v_spd->labelsize(info_size);
  v_spd->setMutex(&m_pmv_mutex);

  v_crs = new MY_LockableOutput(470, h()-30, 55, 20, "Heading:"); 
  v_crs->textsize(info_size); 
  v_crs->labelsize(info_size);
  v_crs->setMutex(&m_pmv_mutex);

  v_dep = new MY_LockableOutput(610, h()-60, 55, 20, "Dep(m):"); 
  v_dep->textsize(info_size); 
  v_dep->labelsize(info_size);
  v_dep->setMutex(&m_pmv_mutex);

  v_ais = new MY_LockableOutput(610, h()-30, 55, 20, "Age-AIS:"); 
  v_ais->textsize(info_size); 
  v_ais->labelsize(info_size);
  v_ais->setMutex(&m_pmv_mutex);

  time = new MY_LockableOutput(720, h()-60, 70, 20, "Time:"); 
  time->textsize(info_size); 
  time->labelsize(info_size);
  time->setMutex(&m_pmv_mutex);

  warp = new MY_LockableOutput(720, h()-30, 70, 20, "Warp:"); 
  warp->textsize(info_size); 
  warp->labelsize(info_size);
  warp->setMutex(&m_pmv_mutex);
  
  int wid_b  = 120;
  int hgt_b  = 24;
  int col_b  = w() - wid_b;
  int col_bb = w() - (wid_b * 2);
  int row_b  = (h() - 20) - (2* hgt_b) ;
  int row_bb = ((h() - 20) - hgt_b) + 2;

  user_button_1 = new MY_Button(col_b+2, row_b, 
				wid_b-4, hgt_b, "Disabled");
  user_button_2 = new MY_Button(col_b+2, row_bb, 
				wid_b-4, hgt_b, "Disabled");
  user_button_3 = new MY_Button(col_bb, row_b, 
				wid_b-4, hgt_b, "Disabled");
  user_button_4 = new MY_Button(col_bb, row_bb, 
				wid_b-4, hgt_b, "Disabled");

  user_button_1->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)1);
  user_button_2->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)2);
  user_button_3->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)3);
  user_button_4->callback((Fl_Callback*)PMV_GUI::cb_MOOS_Button,(void*)4);

  user_button_1->hide();
  user_button_2->hide();
  user_button_3->hide();
  user_button_4->hide();
    
  this->end();
  this->resizable(this);
  this->show();
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

  m_pmv_mutex.Lock();

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
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
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

#if 0
  cout << "Report --------------------------------" << endl;
  vsize = m_button_keys.size();
  for(int h=0; h<vsize; h++) {
    cout << "[" << h << "] key: [" << m_button_keys[h] << "]" << endl;
    cout << "[" << h << "] var: [" << m_button_vars[h] << "]" << endl;
    cout << "[" << h << "] val: [" << m_button_vals[h] << "]" << endl;
  }
  cout << "End------------------------------------" << endl;
#endif

  m_pmv_mutex.UnLock();
}

//----------------------------------------------------------
// Procedure: addAction
//      Note: 
//            
void PMV_GUI::addAction(string svalue, bool separator) 
{
  m_pmv_mutex.Lock();

  vector<string> svector = parseString(svalue, '#');
  unsigned int i, vsize = svector.size();
  unsigned int pindex = m_action_vars.size();

  vector<string> vars, vals;
  string key = "";
  for(i=0; i<vsize; i++) {
    string param = stripBlankEnds(biteString(svector[i], '='));
    string value = stripBlankEnds(svector[i]);
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
    string left  = truncString(vars[i], 16, "middle");
    string right = truncString(vals[i], 16, "middle");
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

  m_pmv_mutex.UnLock();
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
  m_pmv_mutex.Lock();

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
    m_pmv_mutex.UnLock();
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
  if(age_ais == "-1")
    age_ais = "n/a";

  mviewer->mutexLock();
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

  warp->redraw();
  v_nam->redraw();
  v_typ->redraw();
  v_spd->redraw();
  v_dep->redraw();
  v_ais->redraw();

  mviewer->mutexUnLock();
  m_pmv_mutex.UnLock();
}

//----------------------------------------- MOOS_Button
inline void PMV_GUI::cb_MOOS_Button_i(int val) {  
  string skey = "";
  if(val == 1)
    skey = "button_one";
  else if(val == 2)
    skey = "button_two";
  else if(val == 3)
    skey = "button_three";
  else if(val == 4)
    skey = "button_four";
  
  int vsize = m_button_keys.size();
  int varsize = m_button_vars.size();
  int valsize = m_button_vals.size();
  for(int i=0; i<vsize; i++) {
    if(m_button_keys[i] == skey)  {
      pushPending(m_button_vars[i], m_button_vals[i]);
    }
  }
}

void PMV_GUI::cb_MOOS_Button(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((PMV_GUI*)(o->parent()->user_data()))->cb_MOOS_Button_i(val);
}


//----------------------------------------- DoAction
inline void PMV_GUI::cb_DoAction_i(int i) {  
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

void PMV_GUI::cb_DoAction(Fl_Widget* o, int v) {
  int val = (int)(v);
  ((PMV_GUI*)(o->parent()->user_data()))->cb_DoAction_i(val);
}



//-------------------------------------------------------------------
// Procedure: getPendingVar

string PMV_GUI::getPendingVar(unsigned int ix)
{
  m_pmv_mutex.Lock();

  string return_value;
  if((ix >=0) && (ix < m_pending_vars.size()))
    return_value = m_pending_vars[ix];

  m_pmv_mutex.UnLock();
  return(return_value);
}

//-------------------------------------------------------------------
// Procedure: getPendingVal

string PMV_GUI::getPendingVal(unsigned int ix)
{
  m_pmv_mutex.Lock();

  string return_value;
  if((ix >=0) && (ix < m_pending_vals.size()))
    return_value = m_pending_vals[ix];

  m_pmv_mutex.UnLock();
  return(return_value);
}

//-------------------------------------------------------------------
// Procedure: clearPending

void PMV_GUI::clearPending()
{
  m_pmv_mutex.Lock();

  m_pending_vars.clear();
  m_pending_vals.clear();

  m_pmv_mutex.UnLock();
}

//-------------------------------------------------------------------
// Procedure: pushPending

void PMV_GUI::pushPending(string var, string val)
{
  m_pmv_mutex.Lock();

  m_pending_vars.push_back(var);
  m_pending_vals.push_back(val);

  m_pmv_mutex.UnLock();
}

