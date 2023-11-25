/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

  m_ac_brw_nodes = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_ac_brw_nodes->callback(cb_SelectAppCastNode, 0);
  m_ac_brw_procs = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_ac_brw_procs->callback(cb_SelectAppCastProc, 0);
  m_ac_brw_casts = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_ac_brw_casts->set_output();

  m_rc_brw_nodes = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_rc_brw_nodes->callback(cb_SelectRealmCastNode, 0);
  m_rc_brw_procs = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_rc_brw_procs->callback(cb_SelectRealmCastProc, 0);
  m_rc_brw_casts = new MY_Fl_Hold_Browser(0, 0, 1, 1);
  m_rc_brw_casts->set_output();

  m_rc_button_src = new Fl_Button(0, 0, 1, 1, "SRC");
  m_rc_button_com = new Fl_Button(0, 0, 1, 1, "COM");
  m_rc_button_wrp = new Fl_Button(0, 0, 1, 1, "WRP");
  m_rc_button_sub = new Fl_Button(0, 0, 1, 1, "SUBS");
  m_rc_button_msk = new Fl_Button(0, 0, 1, 1, "MASK");
  m_rc_button_trc = new Fl_Button(0, 0, 1, 1, "TRUNC");
  m_rc_button_utc = new Fl_Button(0, 0, 1, 1, "UTC");

  m_rc_button_src->clear_visible_focus();
  m_rc_button_com->clear_visible_focus();
  m_rc_button_wrp->clear_visible_focus();
  m_rc_button_sub->clear_visible_focus();
  m_rc_button_msk->clear_visible_focus();
  m_rc_button_trc->clear_visible_focus();
  m_rc_button_utc->clear_visible_focus();
  
  m_rc_button_utc->callback((Fl_Callback*)UMV_GUI::cb_REALM_Button,(void*)24);
  m_rc_button_src->callback((Fl_Callback*)UMV_GUI::cb_REALM_Button,(void*)25);
  m_rc_button_com->callback((Fl_Callback*)UMV_GUI::cb_REALM_Button,(void*)26);
  m_rc_button_wrp->callback((Fl_Callback*)UMV_GUI::cb_REALM_Button,(void*)27);

  m_rc_button_sub->callback((Fl_Callback*)UMV_GUI::cb_REALM_Button,(void*)28);
  m_rc_button_msk->callback((Fl_Callback*)UMV_GUI::cb_REALM_Button,(void*)29);
  m_rc_button_trc->callback((Fl_Callback*)UMV_GUI::cb_REALM_Button,(void*)30);

  augmentMenu();

  this->resizable(this);
  this->end();
  this->show();

  m_ac_repo = 0;
  m_rc_repo = 0;
  
  resizeWidgets();
}

//----------------------------------------------------------
// Procedure: augmentMenu

void UMV_GUI::augmentMenu() 
{
  m_menubar->add("File/Quit ", FL_CTRL+'q', (Fl_Callback*)UMV_GUI::cb_Quit, 0, 0);

  m_menubar->add("InfoCasting/content_mode=appcast", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)45,
		 FL_MENU_RADIO);    
  m_menubar->add("InfoCasting/content_mode=realmcast", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)46,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/  Toggle InfoCast Content", 'a',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)47,
		 FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/refresh_mode=paused",    ' ',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)50,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/refresh_mode=events",    'e',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)51,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/refresh_mode=streaming", 's',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)52,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/nodes_font_size=xlarge",  0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)64,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/nodes_font_size=large",  0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)63,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/nodes_font_size=medium", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)62,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/nodes_font_size=small",  0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)61,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/nodes_font_size=xsmall", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)60,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/procs_font_size=xlarge",  0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)74,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/procs_font_size=large",  0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)73,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/procs_font_size=medium", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)72,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/procs_font_size=small",  0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)71,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/procs_font_size=xsmall", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)70,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/infocast_font_size=xlarge",  0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)84,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/infocast_font_size=large",  0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)83,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/infocast_font_size=medium", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)82,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/infocast_font_size=small",  0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)81,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/infocast_font_size=xsmall", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)80,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/infocast_font_size bigger",  '}',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)89,
		 FL_MENU_RADIO); 
  m_menubar->add("InfoCasting/infocast_font_size smaller", '{',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)88,
		 FL_MENU_RADIO|FL_MENU_DIVIDER); 

  
  
  m_menubar->add("InfoCasting/appcast_color_scheme=default",     0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)300,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/appcast_color_scheme=indigo",      0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)301,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/appcast_color_scheme=beige",       0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)302,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/appcast_color_scheme Toggle", FL_ALT+'a',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)310,
		 FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/realmcast_color_scheme=white", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)305,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_color_scheme=indigo", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)306,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_color_scheme=beige", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)307,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_color_scheme=hillside", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)308,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_color_scheme Toggle", FL_SHIFT+FL_ALT+'a',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)309,
		 FL_MENU_DIVIDER);

  
  m_menubar->add("InfoCasting/InfoCast Window Height/taller",  FL_SHIFT+FL_Up,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)200, 0);
  m_menubar->add("InfoCasting/InfoCast Window Height/shorter", FL_SHIFT+FL_Down,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)201, 0);
  m_menubar->add("InfoCasting/InfoCast Window Height/reset",   FL_ALT+'a',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)202,
		 FL_MENU_DIVIDER);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=90", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)290,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=85", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)285,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=80", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)280,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=75", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)275,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=70", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)270,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=65", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)265,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=60", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)260,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=55", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)255,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=50", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)250,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=45", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)245,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=40", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)240,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=35", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)235,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/InfoCast Window Height/infocast_height=30", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)230,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);


  m_menubar->add("InfoCasting/realmcast_show_source/realmcast_show_source=true", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)320,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_source/realmcast_show_source=false", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)321,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_source/realmcast_show_source Toggle", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)322,
		 FL_MENU_RADIO);

  m_menubar->add("InfoCasting/realmcast_show_community/realmcast_show_community=true", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)330,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_community/realmcast_show_community=false", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)331,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_community/realmcast_show_community Toggle", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)332,
		 FL_MENU_RADIO);

  m_menubar->add("InfoCasting/realmcast_show_subscriptions/realmcast_show_subscriptions=true", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)340,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_subscriptions/realmcast_show_subscriptions=false", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)341,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_subscriptions/realmcast_show_subscriptions Toggle", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)342,
		 FL_MENU_RADIO);

  
  m_menubar->add("InfoCasting/realmcast_show_masked/realmcast_show_masked=true", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)350,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_masked/realmcast_show_masked=false", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)351,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_show_masked/realmcast_show_masked Toggle", 'k',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)352,
		 FL_MENU_RADIO);

  m_menubar->add("InfoCasting/realmcast_wrap_content/realmcast_wrap_content=true", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)360,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_wrap_content/realmcast_wrap_content=false", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)361,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_wrap_content/realmcast_wrap_content Toggle", 'w',
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)362,
		 FL_MENU_RADIO);

  m_menubar->add("InfoCasting/realmcast_trunc_content/realmcast_trunc_content=true", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)370,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_trunc_content/realmcast_trunc_content=false", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)371,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_trunc_content/realmcast_trunc_content Toggle", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)372,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  m_menubar->add("InfoCasting/realmcast_time_format/realmcast_time_format_utc=true", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)380,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_time_format/realmcast_time_format_utc=false", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)381,
		 FL_MENU_RADIO);
  m_menubar->add("InfoCasting/realmcast_time_format/realmcast_time_format_utc Toggle", 0,
		 (Fl_Callback*)UMV_GUI::cb_InfoCastSetting, (void*)382,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);

  


}

//----------------------------------------------------------
// Procedure: resize

void UMV_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgets();
}

//---------------------------------------------------------- 
// Procedure: updateAppCastNodes()

void UMV_GUI::updateAppCastNodes(bool clear) 
{
  if(!m_ac_repo)
    return;

  vector<string> nodes  = m_ac_repo->getCurrentNodes();
  unsigned int i, vsize = nodes.size();

  if(vsize == 0)
    return;

  int    curr_brw_ix  = ((Fl_Browser *)m_ac_brw_nodes)->value();
  string current_node = m_ac_repo->getCurrentNode();

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
    apc_cnt = m_ac_repo->actree().getNodeAppCastCount(node);
    cfg_cnt = m_ac_repo->actree().getNodeCfgWarningCount(node);
    run_cnt = m_ac_repo->actree().getNodeRunWarningCount(node);

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
    m_ac_brw_nodes->clear();

  unsigned int   curr_brw_size = m_ac_brw_nodes->size();
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
      m_ac_brw_nodes->add(line.c_str());
    else
      m_ac_brw_nodes->text(j+1, line.c_str());
  }

  // Part 3: Possibly select an item in the browser under rare circumstances
  // If we've cleared the nodes to build this, or if no userclicks prior
  // to this call, set the browser select to be the current_node.
  if(clear || (curr_brw_ix==0))
    m_ac_brw_nodes->select(brw_item_index, 1);
}

//---------------------------------------------------------- 
// Procedure: updateAppCastProcs()

void UMV_GUI::updateAppCastProcs(bool clear) 
{
  if(!m_ac_repo)
    return;
  
  vector<string> procs  = m_ac_repo->getCurrentProcs();
  unsigned int i, vsize = procs.size();
  
  if(vsize == 0)
    return;

  int    curr_brw_ix  = ((Fl_Browser *)m_ac_brw_procs)->value();
  string current_node = m_ac_repo->getCurrentNode();
  string current_proc = m_ac_repo->getCurrentProc();

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
    apc_cnt = m_ac_repo->actree().getProcAppCastCount(current_node, proc);
    cfg_cnt = m_ac_repo->actree().getProcCfgWarningCount(current_node, proc);
    run_cnt = m_ac_repo->actree().getProcRunWarningCount(current_node, proc);

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
    m_ac_brw_procs->clear();

  unsigned int   curr_brw_size = m_ac_brw_procs->size();
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
      m_ac_brw_procs->add(line.c_str());
    else
      m_ac_brw_procs->text(j+1, line.c_str());
  }

  // Part 3: Possibly select an item in the browser under rare circumstances
  // If we've cleared the procs to build this, or if no userclicks prior
  // to this call, set the browser select to be the current_proc.
  if(clear || (curr_brw_ix==0))
    m_ac_brw_procs->select(brw_item_index, 1);

}


//----------------------------------------------------- 
// Procedure: updateAppCast

void UMV_GUI::updateAppCast() 
{
  if(!m_ac_repo)
    return;

  string node = m_ac_repo->getCurrentNode();
  string proc = m_ac_repo->getCurrentProc();
  if((node == "") || (proc == ""))
    return;

  // Step 1: clear the Fl_Browser contents
  m_ac_brw_casts->clear();

  // Step 2: retrieve the current appcast and its report lines
  AppCast appcast = m_ac_repo->actree().getAppCast(node, proc);
  vector<string> svector = parseString(appcast.getFormattedString(), '\n');
  
  // Step 3: De-colorize the report lines and add to the Browser
  for(unsigned int i=0; i<svector.size(); i++) {
    m_ac_brw_casts->add(removeTermColors(svector[i]).c_str());
  }

  // Step 4: If there are warnings, colorize line 2 of the browser
  //         appropriately. Red if run_warnings, Blue if config warnings
  if(svector.size() >=2) {
    if(appcast.getRunWarningCount() > 0) {
      string txt = m_ac_brw_casts->text(2);
      txt = "@B" + uintToString(m_color_runw) + txt;
      m_ac_brw_casts->text(2, txt.c_str());
    }
    else if(appcast.getCfgWarningCount() > 0) {
      string txt = m_ac_brw_casts->text(2);
      txt = "@B" + uintToString(m_color_cfgw) + txt;
      m_ac_brw_casts->text(2, txt.c_str());
    }
  }
}


//---------------------------------------------------------- 
// Procedure: updateRealmCastNodes()

void UMV_GUI::updateRealmCastNodes(bool clear) 
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
// Procedure: updateRealmCastProcs()

void UMV_GUI::updateRealmCastProcs(bool clear) 
{
  string curr_cluster_key = m_rc_repo->getCurrClusterKey();

  if(curr_cluster_key != "")
    updateRealmCastProcsWC(clear);
  else
    updateRealmCastProcsRC(clear);
}

//---------------------------------------------------------- 
// Procedure: updateRealmCastProcsWC()

void UMV_GUI::updateRealmCastProcsWC(bool clear) 
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

void UMV_GUI::updateRealmCastProcsRC(bool clear) 
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


//----------------------------------------------------- 
// Procedure: updateRealmCast

void UMV_GUI::updateRealmCast() 
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
    // Step 2: retrieve the current appcast and its report lines
    RealmCast relcast = m_rc_repo->rctree().getRealmCast(node, proc);
    vector<string> svector = parseString(relcast.getFormattedString(), '\n');
    
    // Step 3: De-colorize the report lines and add to the Browser
    for(unsigned int i=0; i<svector.size(); i++) {
      m_rc_brw_casts->add(svector[i].c_str());
    }
  }
  else {
    vector<string> svector = m_rc_repo->getClusterReport(m_icast_settings);
    for(unsigned int i=0; i<svector.size(); i++) {
      m_rc_brw_casts->add(svector[i].c_str());
    }
  }
}


//----------------------------------------------------- 
// Procedure: setRadioCastAttrib
//   Purpose: A way of setting the infocast attribute assuring that 
//            the GUI radio buttons are properly updated.
//  
//  Examples: "nodes_font_size", "xsmall"
//            "refresh_mode", "false"

bool UMV_GUI::setRadioCastAttrib(string attr, string value) 
{
  Fl_Color fcolor_grayBlu = fl_rgb_color(165,165,225);
  Fl_Color fcolor_gray185 = fl_rgb_color(185,185,185);
  Fl_Color fcolor_grayRed = fl_rgb_color(225,185,185);
  
  string item_str = "InfoCasting/" + attr + "=";

  // Support params deprecated after realmcasting was introduced
  if(attr == "appcast_height")
    attr="infocast_height";
  else if(attr == "appcast_font_size")
    attr="infocast_font_size";
  
  bool ok = false;
  if(attr == "content_mode") {
    ok = m_icast_settings.setContentMode(value);
    item_str += m_icast_settings.getContentMode();
  }
  else if(attr == "refresh_mode") {
    ok = m_icast_settings.setRefreshMode(value);
    item_str += m_icast_settings.getRefreshMode();
  }
  else if(attr == "procs_font_size") {
    ok = m_icast_settings.setProcsFontSize(value);
    item_str += m_icast_settings.getProcsFontSize();
  }
  else if(attr == "nodes_font_size") {
    ok = m_icast_settings.setNodesFontSize(value);
    item_str += m_icast_settings.getNodesFontSize();
  }
  else if(attr == "infocast_font_size") {
    ok = m_icast_settings.setInfoCastFontSize(value);
    item_str += m_icast_settings.getInfoCastFontSize();
  }
  else if(attr == "infocast_height") {
    ok = m_icast_settings.setInfoCastHeight(value);
    item_str += m_icast_settings.getInfoCastHeight();
  }
  else if(attr == "appcast_color_scheme") {
    ok = m_icast_settings.setAppCastColorScheme(value);
    item_str += m_icast_settings.getAppCastColorScheme();
  }
  else if(attr == "realmcast_color_scheme") {
    ok = m_icast_settings.setRealmCastColorScheme(value);
    item_str += m_icast_settings.getRealmCastColorScheme();
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
    updateRealmCast();
  }
  else if(attr == "realmcast_trunc_content") {
    ok = m_icast_settings.setTruncRealmCastContent(value);
    item_str = "InfoCasting/realmcast_trunc_content/realmcast_trunc_content=";
    item_str += boolToString(m_icast_settings.getTruncRealmCastContent());
    if(m_icast_settings.getTruncRealmCastContent())
      m_rc_button_trc->color(fcolor_grayBlu);
    else
      m_rc_button_trc->color(fcolor_gray185);
    updateRealmCast();
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
  
  const Fl_Menu_Item *item = m_menubar->find_item(item_str.c_str());
  if(item)
    ((Fl_Menu_Item *)item)->setonly();

  resizeWidgets();
  redraw();
  if(!ok)
    return(false);

  return(true);
}


//----------------------------------------- REALM_Button
inline void UMV_GUI::cb_REALM_Button_i(unsigned int val) {  
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

void UMV_GUI::cb_REALM_Button(Fl_Widget* o, unsigned int v) {
  ((UMV_GUI*)(o->parent()->user_data()))->cb_REALM_Button_i(v);
}




//----------------------------------------------------
// Procedure: cb_SelectAppCastNode_i()

inline void UMV_GUI::cb_SelectAppCastNode_i() 
{
  vector<string> nodes = m_ac_repo->getCurrentNodes();
  if(nodes.size() == 0)
    return;

  int ix = ((Fl_Browser *)m_ac_brw_nodes)->value();

  // If the user clicks below the list of items, treat as if the
  // last item were clicked. Figure out its index and set it.
  if(ix == 0) {
    ix = 2 + (int)(nodes.size()); // Add 2 due to table header lines
    m_ac_brw_nodes->select(ix,1); 
  }

  // If the user clicks one of the two header lines, treat this
  // as a click of the first item in the list.
  if(ix <= 2) {
    ix = 3;
    m_ac_brw_nodes->select(ix,1);
  }

  unsigned int node_ix = ix - 3;
  m_ac_repo->setCurrentNode(nodes[node_ix]);
  updateAppCastNodes();
  updateAppCastProcs(true);
  updateAppCast();
}

//----------------------------------------------------
// Procedure: cb_SelectAppCastNode()

void UMV_GUI::cb_SelectAppCastNode(Fl_Widget* o, long) 
{
  ((UMV_GUI*)(o->parent()->user_data()))->cb_SelectAppCastNode_i();
}


//----------------------------------------------------
// Procedure: cb_SelectAppCastProci()

inline void UMV_GUI::cb_SelectAppCastProc_i() 
{
  vector<string> procs = m_ac_repo->getCurrentProcs();
  if(procs.size() == 0)
    return;

  int ix = ((Fl_Browser *)m_ac_brw_procs)->value();

  // If the user clicks below the list of items, treat as if the
  // last item were clicked. Figure out its index and set it.
  if(ix == 0) {
    ix = 2 + (int)(procs.size()); // Add 2 due to table header lines
    m_ac_brw_procs->select(ix,1);
  }
  
  // If the user clicks one of the two header lines, treat this
  // as a click of the first item in the list.
  if(ix <= 2) {
    ix = 3;
    m_ac_brw_procs->select(ix,1);
  }

  unsigned int proc_ix = ix - 3;  
  m_ac_repo->setCurrentProc(procs[proc_ix]);

  updateAppCastProcs();
  updateAppCast();
}

//----------------------------------------------------
// Procedure: cb_SelectAppCastProc()

void UMV_GUI::cb_SelectAppCastProc(Fl_Widget* o, long) 
{
  ((UMV_GUI*)(o->parent()->user_data()))->cb_SelectAppCastProc_i();
}




//----------------------------------------------------
// Procedure: cb_SelectRealmCastNode_i()

inline void UMV_GUI::cb_SelectRealmCastNode_i() 
{
  vector<string> nodes = m_rc_repo->getCurrentNodes();
  if(nodes.size() == 0)
    return;

  vector<string> cluster_keys = m_rc_repo->getClusterKeys();

  int ix = ((Fl_Browser *)m_rc_brw_nodes)->value();

  // If the user clicks one of the two header lines, treat this
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
    }
  }

  if(new_node == "error")
    return;
  
  m_rc_repo->setCurrentNode(new_node);
  updateRealmCastNodes();
  updateRealmCastProcs(true);
  updateRealmCast();
}

//----------------------------------------------------
// Procedure: cb_SelectRealmCastNode()

void UMV_GUI::cb_SelectRealmCastNode(Fl_Widget* o, long) 
{
  ((UMV_GUI*)(o->parent()->user_data()))->cb_SelectRealmCastNode_i();
}


//----------------------------------------------------
// Procedure: cb_SelectRealmCastProci()

inline void UMV_GUI::cb_SelectRealmCastProc_i() 
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

void UMV_GUI::cb_SelectRealmCastProc(Fl_Widget* o, long) 
{
  ((UMV_GUI*)(o->parent()->user_data()))->cb_SelectRealmCastProc_i();
}

//----------------------------------------- InfoCastSetting
inline void UMV_GUI::cb_InfoCastSetting_i(unsigned int v) {  
  // Handle infocast view mode switching
  if(v==45)  setRadioCastAttrib("content_mode", "appcast");
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

  // Handle InfoCast pane height relative adjustment
  else if(v==200) setRadioCastAttrib("infocast_height", "delta:5");
  else if(v==201) setRadioCastAttrib("infocast_height", "delta:-5");
  else if(v==202) setRadioCastAttrib("infocast_height", "70");
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

  else if(v==300) setRadioCastAttrib("appcast_color_scheme", "default");
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

void UMV_GUI::cb_InfoCastSetting(Fl_Widget* o, unsigned int v) {
  ((UMV_GUI*)(o->parent()->user_data()))->cb_InfoCastSetting_i(v);
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
  setRadioCastAttrib("infocast_font_size");
  setRadioCastAttrib("infocast_height");
  setRadioCastAttrib("content_mode");
  setRadioCastAttrib("refresh_mode");
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

void UMV_GUI::setMenuItemColors()
{
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

  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=30");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=35");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=40");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=45");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=50");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=55");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=60");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=65");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=70");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=75");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=80");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=85");
  setMenuItemColor("InfoCasting/InfoCast Window Height/infocast_height=90");

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
}

//----------------------------------------------------- 
// Procedure: setMenuItemColor

void UMV_GUI::setMenuItemColor(string item_str, int r, int g, int b)
{
  Fl_Color new_color = fl_rgb_color(r, g, b);

  const Fl_Menu_Item *item = m_menubar->find_item(item_str.c_str());
  if(item)
    ((Fl_Menu_Item *)item)->labelcolor(new_color);

}





//-------------------------------------------------------------------
// Procedure: resizeWidgets()

void UMV_GUI::resizeWidgets()
{
  string content_mode  = m_icast_settings.getContentMode();
  
  // ================================================================
  // Part 1: Figure out the basic pane extents.
  // ================================================================

  double pct_cast_hgt = m_icast_settings.getInfoCastHeight() / 100;
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
  
  // ================================================================
  // Part 2: Adjust the extents of the MENU Bar
  // ================================================================  
  double menu_hgt = now_hgt * pct_menu_hgt;

  m_menubar->resize(0, 0, w(), menu_hgt);

  // ================================================================
  // Part 3: Adjust the extents of the InfoCast browsers
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

  if(content_mode == "realmcast")
    cast_hgt -= 40;
  
  m_ac_brw_nodes->resize(bx, menu_hgt, node_wid, node_hgt);
  m_ac_brw_procs->resize(node_wid, menu_hgt, proc_wid, proc_hgt);
  m_ac_brw_casts->resize(bx, by+node_hgt, cast_wid, cast_hgt);

  m_rc_brw_nodes->resize(bx, menu_hgt, node_wid, node_hgt);
  m_rc_brw_procs->resize(node_wid, menu_hgt, proc_wid, proc_hgt);
  m_rc_brw_casts->resize(bx, by+node_hgt, cast_wid, cast_hgt);

  int font_xlarge = 16;
  int font_large  = 14;
  int font_medium = 12;
  int font_small  = 10;
  int font_xsmall = 8;

  if(bw < 400) {
    font_xlarge = 12;
    font_large  = 10;
    font_medium = 8;
    font_small  = 7;
    font_xsmall = 6;
  }

  else if(bw < 450) {
    font_xlarge = 14;
    font_large  = 12;
    font_medium = 10;
    font_small  = 8;
    font_xsmall = 7;
  }

  // ================================================================
  // Part 4: Adjust the font size of the InfoCast browsers
  // ================================================================
  string nodes_font_size   = m_icast_settings.getNodesFontSize();
  string procs_font_size   = m_icast_settings.getProcsFontSize();
  string infocast_font_size = m_icast_settings.getInfoCastFontSize();

  int node_font_isize = font_xsmall;
  if(nodes_font_size == "small") 
    node_font_isize = font_small;
  else if(nodes_font_size == "medium") 
    node_font_isize = font_medium;
  else if(nodes_font_size == "large") 
    node_font_isize = font_large;
  else 
    node_font_isize = font_xlarge;
  m_ac_brw_nodes->textsize(node_font_isize);
  m_rc_brw_nodes->textsize(node_font_isize);

  
  int proc_font_isize = font_xsmall;
  if(procs_font_size == "small") 
    proc_font_isize = font_small;
  else if(procs_font_size == "medium") 
    proc_font_isize = font_medium;
  else if(procs_font_size == "large") 
    proc_font_isize = font_large;
  else 
    proc_font_isize = font_xlarge;
  m_ac_brw_procs->textsize(proc_font_isize);
  m_rc_brw_procs->textsize(proc_font_isize);

  
  int cast_font_isize = font_xsmall;
  if(infocast_font_size == "small")  
    cast_font_isize = font_small;
  else if(infocast_font_size == "medium") 
    cast_font_isize = font_medium;
  else if(infocast_font_size == "large")  
    cast_font_isize = font_large;
  else
    cast_font_isize = font_xlarge;
  m_ac_brw_casts->textsize(cast_font_isize);
  m_rc_brw_casts->textsize(cast_font_isize);

  // ================================================================
  // Part 5: Adjust the color scheme of the Infocast browsers
  // ================================================================  
  string appcast_color_scheme = m_icast_settings.getAppCastColorScheme();
  string realmcast_color_scheme = m_icast_settings.getRealmCastColorScheme();

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

  m_ac_brw_nodes->color(color_back);
  m_ac_brw_nodes->textcolor(color_text);
  m_ac_brw_nodes->position(0);

  m_ac_brw_procs->color(color_back);
  m_ac_brw_procs->textcolor(color_text);
  m_ac_brw_procs->position(0);

  m_ac_brw_casts->color(color_back);
  m_ac_brw_casts->textcolor(color_text);
  m_ac_brw_casts->position(0);

  if(realmcast_color_scheme == "indigo") {
    color_back = fl_rgb_color(95, 117, 182);   // indigo-lighter (65,87,152)
    color_text = fl_rgb_color(255, 255, 255);  // white
    m_color_runw = fl_rgb_color(205, 71, 71);  // redish
    m_color_cfgw = fl_rgb_color(0, 159, 119);  // greenish
  }
  else if(realmcast_color_scheme == "beige") {
    color_back = fl_rgb_color(223, 219, 195);   // beige
    color_text = fl_rgb_color(0, 0, 0);         // black
    m_color_runw = fl_rgb_color(205, 71, 71);   // redish
    m_color_cfgw = fl_rgb_color(0, 189, 149);   // greenish
  }
  else if(realmcast_color_scheme == "hillside") {
    color_back = fl_rgb_color(209, 203, 127);   // hillside
    color_text = fl_rgb_color(0, 0, 0);         // black
    m_color_runw = fl_rgb_color(205, 71, 71);   // redish
    m_color_cfgw = fl_rgb_color(0, 189, 149);   // greenish
  }

  m_rc_brw_nodes->color(color_back);
  m_rc_brw_nodes->textcolor(color_text);
  m_rc_brw_nodes->position(0);

  m_rc_brw_procs->color(color_back);
  m_rc_brw_procs->textcolor(color_text);
  m_rc_brw_procs->position(0);

  m_rc_brw_casts->color(color_back);
  m_rc_brw_casts->textcolor(color_text);
  m_rc_brw_casts->position(0);


  // ================================================================
  // Part 6: Handle the Button position and size
  // ================================================================
  int colsep = 10;
  int buttons_wid = w() - (colsep * 7); // 6 buttons need 7 margins
  int lft_buttons_wid = buttons_wid * 0.38;
  int rgt_buttons_wid = buttons_wid * 0.62;
  int bwid1 = lft_buttons_wid / 3;
  int bwid2 = rgt_buttons_wid / 4;
  
  int row1 = h() - 35;
  int col1 = 10;
  int col2 = col1 + bwid1 + colsep;
  int col3 = col2 + bwid1 + colsep;
  int col4 = col3 + bwid1 + colsep;
  int col5 = col4 + bwid2 + colsep;
  int col6 = col5 + bwid2 + colsep;
  int col7 = col6 + bwid2 + colsep;
  int fld_hgt = 22;

  int button_font_size = 12;
  if(now_wid < 800)
    button_font_size = 10;
  
  m_rc_button_src->labelsize(button_font_size);
  m_rc_button_com->labelsize(button_font_size);
  m_rc_button_wrp->labelsize(button_font_size);
  m_rc_button_sub->labelsize(button_font_size);
  m_rc_button_msk->labelsize(button_font_size);
  m_rc_button_trc->labelsize(button_font_size);
  m_rc_button_utc->labelsize(button_font_size);

  m_rc_button_src->resize(col1, row1, bwid1, fld_hgt);
  m_rc_button_com->resize(col2, row1, bwid1, fld_hgt);
  m_rc_button_wrp->resize(col3, row1, bwid1, fld_hgt);
  m_rc_button_sub->resize(col4, row1, bwid2, fld_hgt);
  m_rc_button_msk->resize(col5, row1, bwid2, fld_hgt);
  m_rc_button_trc->resize(col6, row1, bwid2, fld_hgt);
  m_rc_button_utc->resize(col7, row1, bwid2, fld_hgt);

  // ================================================================
  // Part 7: Based on content mode, enable/disable right widgets
  // ================================================================
  if(content_mode == "appcast") {
    m_rc_brw_nodes->hide();
    m_rc_brw_procs->hide();
    m_rc_brw_casts->hide();

    m_ac_brw_nodes->show();
    m_ac_brw_procs->show();
    m_ac_brw_casts->show();

    m_rc_button_src->hide();
    m_rc_button_com->hide();
    m_rc_button_sub->hide();
    m_rc_button_msk->hide();
    m_rc_button_wrp->hide();
    m_rc_button_trc->hide();
    m_rc_button_utc->hide();
  }
  else {
    m_ac_brw_nodes->hide();
    m_ac_brw_procs->hide();
    m_ac_brw_casts->hide();

    m_rc_brw_nodes->show();
    m_rc_brw_procs->show();
    m_rc_brw_casts->show();

    m_rc_button_src->show();
    m_rc_button_com->show();
    m_rc_button_sub->show();
    m_rc_button_msk->show();
    m_rc_button_wrp->show();
    m_rc_button_trc->show();
    m_rc_button_utc->show();
  }

}


