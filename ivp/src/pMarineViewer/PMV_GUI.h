/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PMV_GUI.h                                            */
/*    DATE: Nov 11th 2004                                        */
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

#ifndef PMV_GUI_HEADER
#define PMV_GUI_HEADER

#include "PMV_Viewer.h"
#include "MarineVehiGUI.h"
#include <FL/Fl_Output.H>
#include "MY_Button.h"

class PMV_GUI : public MarineVehiGUI {
public:
  PMV_GUI(int w, int h, const char *l=0);
  virtual ~PMV_GUI() {};

  void draw() {MarineVehiGUI::draw();};

  void updateXY();
  void addButton(std::string button, std::string pairs);
  void addAction(std::string pair, bool separator=false);
  int  handle(int);
  void setCurrTime(double v) {m_curr_time = v;};

  PMV_Viewer *mviewer;

  std::string  getPendingVar(unsigned int index);
  std::string  getPendingVal(unsigned int index);
  void         clearPending();
  void         pushPending(std::string, std::string);
  unsigned int getPendingSize() {return(m_pending_vars.size());};
  bool         addScopeVariable(std::string);
  void         addContext(std::string side, std::string context);
  void         addReferenceVehicle(std::string vname);

 private:
  inline void cb_MOOS_Button_i(int);
  static void cb_MOOS_Button(Fl_Widget*, int);
  inline void cb_DoAction_i(int);
  static void cb_DoAction(Fl_Widget*, int);
  inline void cb_Scope_i(int);
  static void cb_Scope(Fl_Widget*, int);
  inline void cb_LeftContext_i(int);
  static void cb_LeftContext(Fl_Widget*, int);
  inline void cb_RightContext_i(int);
  static void cb_RightContext(Fl_Widget*, int);
  inline void cb_Reference_i(int);
  static void cb_Reference(Fl_Widget*, int);

protected:
  Fl_Output  *v_nam;
  Fl_Output  *v_typ;
  Fl_Output  *x_mtr;
  Fl_Output  *y_mtr;
  Fl_Output  *v_spd;
  Fl_Output  *v_lat;
  Fl_Output  *v_lon;
  Fl_Output  *v_crs;
  Fl_Output  *v_dep;
  Fl_Output  *v_ais;
  Fl_Output  *time;
  Fl_Output  *warp;
  Fl_Output  *v_range;
  Fl_Output  *v_bearing;

  Fl_Output  *m_scope_variable;
  Fl_Output  *m_scope_time;
  Fl_Output  *m_scope_value;

  MY_Button  *user_button_1;
  MY_Button  *user_button_2;
  MY_Button  *user_button_3;
  MY_Button  *user_button_4;

  std::vector<std::string> m_button_keys;
  std::vector<std::string> m_button_vars;
  std::vector<std::string> m_button_vals;

  std::vector<std::string> m_action_vars;
  std::vector<std::string> m_action_vals;
  std::vector<std::string> m_action_keys;

  std::vector<std::string> m_scope_vars;

  std::vector<std::string> m_left_context;
  std::vector<std::string> m_right_context;

  std::vector<std::string> m_reference_tags;

  double     m_curr_time;
  double     m_prev_out_time;
};
#endif


