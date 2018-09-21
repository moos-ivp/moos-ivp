/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UMV_GUI.cpp                                          */
/*    DATE: Aug 11th, 2012                                       */
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "UMV_GUI.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "ACTable.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

UMV_GUI::UMV_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(350,250);

  m_menubar = new Fl_Menu_Bar(0, 0, w(), 20);
  m_menubar->textsize(12);

  m_brw_nodes = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_brw_nodes->callback(cb_SelectNode, 0);

  m_brw_procs = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_brw_procs->callback(cb_SelectProc, 0);

  m_brw_casts = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_brw_casts->set_output();

  augmentMenu();

  this->resizable(this);
  this->end();
  this->show();

  resizeWidgets();
}

//----------------------------------------------------------
// Procedure: augmentMenu

void UMV_GUI::augmentMenu() 
{
  m_menubar->add("File/Quit ", FL_CTRL+'q', (Fl_Callback*)UMV_GUI::cb_Quit, 0, 0);

  m_menubar->add("AppCasting/refresh_mode=paused",    ' ', (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)50, FL_MENU_RADIO);
  m_menubar->add("AppCasting/refresh_mode=events",    'e', (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)51, FL_MENU_RADIO);
  m_menubar->add("AppCasting/refresh_mode=streaming", 's', (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)52, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("AppCasting/nodes_font_size=large",  0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)63, FL_MENU_RADIO);
  m_menubar->add("AppCasting/nodes_font_size=medium", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)62, FL_MENU_RADIO);
  m_menubar->add("AppCasting/nodes_font_size=small",  0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)61, FL_MENU_RADIO);
  m_menubar->add("AppCasting/nodes_font_size=xsmall", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)60, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("AppCasting/procs_font_size=large",  0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)73, FL_MENU_RADIO);
  m_menubar->add("AppCasting/procs_font_size=medium", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)72, FL_MENU_RADIO);
  m_menubar->add("AppCasting/procs_font_size=small",  0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)71, FL_MENU_RADIO);
  m_menubar->add("AppCasting/procs_font_size=xsmall", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)70, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("AppCasting/appcast_font_size=large",  0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)83, FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_font_size=medium", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)82, FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_font_size=small",  0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)81, FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_font_size=xsmall", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)80, FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("AppCasting/appcast_color_scheme=default",     0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)300, FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_color_scheme=indigo",      0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)301, FL_MENU_RADIO);
  m_menubar->add("AppCasting/appcast_color_scheme=beige",       0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)302, FL_MENU_RADIO);
  m_menubar->add("AppCasting/    Toggle Color Scheme", FL_ALT+'a', (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)310, FL_MENU_DIVIDER);

  m_menubar->add("AppCasting/AppCast Window Height/taller",  FL_CTRL+FL_ALT+FL_Up,   (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)200, 0);
  m_menubar->add("AppCasting/AppCast Window Height/shorter", FL_CTRL+FL_ALT+FL_Down, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)201, 0);
  m_menubar->add("AppCasting/AppCast Window Height/reset",   FL_CTRL+FL_ALT+'a',     (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)202, FL_MENU_DIVIDER);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=90", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)290, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=85", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)285, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=80", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)280, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=75", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)275, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=70", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)270, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=65", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)265, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=60", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)260, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=55", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)255, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=50", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)250, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=45", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)245, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=40", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)240, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=35", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)235, FL_MENU_RADIO);
  m_menubar->add("AppCasting/AppCast Window Height/appcasting_height=30", 0, (Fl_Callback*)UMV_GUI::cb_AppCastSetting, (void*)230, FL_MENU_RADIO|FL_MENU_DIVIDER);
}

//----------------------------------------------------------
// Procedure: resize

void UMV_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgets();
}

//---------------------------------------------------------- 
// Procedure: updateNodes()

void UMV_GUI::updateNodes(bool clear) 
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

  unsigned int   curr_brw_size = m_brw_nodes->size();
  vector<string> browser_lines = actab.getTableOutput();
  for(unsigned int j=0; j<browser_lines.size(); j++) {
    string line = browser_lines[j];
    if(j>=2) {
      if(node_has_run_warning[j-2])
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

void UMV_GUI::updateProcs(bool clear) 
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


//----------------------------------------------------- 
// Procedure: updateAppCast

void UMV_GUI::updateAppCast() 
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
// Procedure: setRadioCastAttrib
//   Purpose: A way of setting the appcast attribute assureing that 
//            the GUI radio buttons are properly updated.
//  
//  Examples: "nodes_font_size", "xsmall"
//            "refresh_mode", "false"

bool UMV_GUI::setRadioCastAttrib(string attr, string value) 
{
  // If value="", no update attempted, just update menu radio button
  if(value != "") {
    bool handled = m_ac_settings.setParam(attr, value);
    if(!handled)
      return(false);
  }
  
  string menu = "AppCasting/";
  if(attr == "appcast_height") 
    menu += "AppCast Window Height/";

  // Get latest setting of this attr (esp if this value is "toggle" etc)
  string new_value = m_ac_settings.strvalue(attr);
  //cout << "setRadioCastAttrib: attr:" << attr << " value:" << value << 
  // " new_value:" << new_value << "]"<< endl;

  // Form for example, "AppCasting/nodes_font_size=small"
  string item_str = menu + attr + "=" + new_value;
  const Fl_Menu_Item *item = m_menubar->find_item(item_str.c_str());
  if(item)
    ((Fl_Menu_Item *)item)->setonly();

  resizeWidgets();
  redraw();
  return(true);
}

//----------------------------------------------------
// Procedure: cb_SelectNode_i()

inline void UMV_GUI::cb_SelectNode_i() 
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
}

//----------------------------------------------------
// Procedure: cb_SelectNode()

void UMV_GUI::cb_SelectNode(Fl_Widget* o, long) 
{
  ((UMV_GUI*)(o->parent()->user_data()))->cb_SelectNode_i();
}


//----------------------------------------------------
// Procedure: cb_SelectProci()

inline void UMV_GUI::cb_SelectProc_i() 
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

void UMV_GUI::cb_SelectProc(Fl_Widget* o, long) 
{
  ((UMV_GUI*)(o->parent()->user_data()))->cb_SelectProc_i();
}

//----------------------------------------- AppCastSetting
inline void UMV_GUI::cb_AppCastSetting_i(unsigned int v) {  
  // Handle appcast refresh setting
  if(v==50)  setRadioCastAttrib("refresh_mode", "paused");
  else if(v==51)  setRadioCastAttrib("refresh_mode", "events");
  else if(v==52)  setRadioCastAttrib("refresh_mode", "streaming");  
  // Handle node browser pane font size
  else if(v==60)  setRadioCastAttrib("nodes_font_size", "xsmall");
  else if(v==61)  setRadioCastAttrib("nodes_font_size", "small");
  else if(v==62)  setRadioCastAttrib("nodes_font_size", "medium");
  else if(v==63)  setRadioCastAttrib("nodes_font_size", "large");
  // Handle procs browser pane font size
  else if(v==70)  setRadioCastAttrib("procs_font_size", "xsmall");
  else if(v==71)  setRadioCastAttrib("procs_font_size", "small");
  else if(v==72)  setRadioCastAttrib("procs_font_size", "medium");
  else if(v==73)  setRadioCastAttrib("procs_font_size", "large");
  // Handle appcast browser pane font size
  else if(v==80)  setRadioCastAttrib("appcast_font_size", "xsmall");
  else if(v==81)  setRadioCastAttrib("appcast_font_size", "small");
  else if(v==82)  setRadioCastAttrib("appcast_font_size", "medium");
  else if(v==83)  setRadioCastAttrib("appcast_font_size", "large");
  // Handle AppCast pane height relative adjustment
  else if(v==200) setRadioCastAttrib("appcast_height", "delta:5");
  else if(v==201) setRadioCastAttrib("appcast_height", "delta:-5");
  else if(v==202) setRadioCastAttrib("appcast_height", "70");
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

  else if(v==300) setRadioCastAttrib("appcast_color_scheme", "default");
  else if(v==301) setRadioCastAttrib("appcast_color_scheme", "indigo");
  else if(v==302) setRadioCastAttrib("appcast_color_scheme", "beige");
  else if(v==310) setRadioCastAttrib("appcast_color_scheme", "toggle");

  resizeWidgets();
  redraw();
}

void UMV_GUI::cb_AppCastSetting(Fl_Widget* o, unsigned int v) {
  ((UMV_GUI*)(o->parent()->user_data()))->cb_AppCastSetting_i(v);
}


//---------------------------------------------------
// Procedure: Quit

void UMV_GUI::cb_Quit() {
  exit(0);
}

//----------------------------------------------------- 
// Procedure: updateRadios

void UMV_GUI::updateRadios()
{
  setRadioCastAttrib("nodes_font_size");
  setRadioCastAttrib("procs_font_size");
  setRadioCastAttrib("appcast_font_size");
  setRadioCastAttrib("appcast_width");
  setRadioCastAttrib("refresh_mode");
  setRadioCastAttrib("appcast_color_scheme");
}


//-------------------------------------------------------------------
// Procedure: resizeWidgets()

void UMV_GUI::resizeWidgets()
{
  // Part 1: Figure out the basic pane extents.
  // ================================================================
  // This is the default extent configuration so long as any minimal
  // constraints are not violated.

  double pct_cast_hgt = m_ac_settings.geosize("appcast_height") / 100;
  //double pct_pane_hgt = 0.95;
  double pct_menu_hgt = 0.05;
  
  // Check for violations of minimal constraints
  double now_hgt = h();
  double now_wid = w();
  
  // Make sure the menu bar is at least 20 and 25
  if((pct_menu_hgt * now_hgt) < 20) 
    pct_menu_hgt = 20 / now_hgt;
  if((pct_menu_hgt * now_hgt) > 25) 
    pct_menu_hgt = 25 / now_hgt;
  //pct_pane_hgt = (1 - pct_menu_hgt);
  
  // Part 2: Adjust the extents of the MENU Bar
  // ================================================================  
  double menu_hgt = now_hgt * pct_menu_hgt;

  m_menubar->resize(0, 0, w(), menu_hgt);

  // Part 3: Adjust the extents of the APPCAST browsers
  // ================================================================  
  // Set the extents as if appcasting and datafields were hidden (or
  // if show_full_screen is set to true).
  double bx = 0;
  double by = menu_hgt;
  double bw = now_wid;
  double bh = now_hgt - menu_hgt;

  double cast_wid = bw;
  double node_wid = bw * 0.45;
  double proc_wid = bw * 0.55;
  
  double cast_hgt = bh * pct_cast_hgt;
  double node_hgt = bh * (1 - pct_cast_hgt);
  double proc_hgt = bh * (1 - pct_cast_hgt);
  
  m_brw_nodes->resize(bx, menu_hgt, node_wid, node_hgt);
  m_brw_procs->resize(node_wid, menu_hgt, proc_wid, proc_hgt);
  m_brw_casts->resize(bx, by+node_hgt, cast_wid, cast_hgt);

  int font_large  = 14;
  int font_medium = 12;
  int font_small  = 10;
  int font_xsmall = 8;

  if(bw < 400) {
    font_large  = 10;
    font_medium = 8;
    font_small  = 7;
    font_xsmall = 6;
  }

  else if(bw < 450) {
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
  else                                 m_brw_nodes->textsize(font_large);
  
  if(procs_font_size == "xsmall")      m_brw_procs->textsize(font_xsmall);
  else if(procs_font_size == "small")  m_brw_procs->textsize(font_small);
  else if(procs_font_size == "medium") m_brw_procs->textsize(font_medium);
  else                                 m_brw_procs->textsize(font_large);
  
  if(appcast_font_size == "xsmall")      m_brw_casts->textsize(font_xsmall);
  else if(appcast_font_size == "small")  m_brw_casts->textsize(font_small);
  else if(appcast_font_size == "medium") m_brw_casts->textsize(font_medium);
  else                                   m_brw_casts->textsize(font_large);

  // Part 4: Adjust the color scheme of the APPCAST browsers
  // ================================================================  
  string appcast_color_scheme = m_ac_settings.strvalue("appcast_color_scheme");

  Fl_Color color_back = fl_rgb_color(255, 255, 255);  // white
  Fl_Color color_text = fl_rgb_color(0, 0, 0);        // black
  m_color_runw = fl_rgb_color(205, 71, 71);           // redish
  m_color_cfgw = fl_rgb_color(50, 189, 149);          // greenish

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
  m_brw_casts->position(0);
}









