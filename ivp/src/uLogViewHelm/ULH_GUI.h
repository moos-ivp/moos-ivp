/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULH_GUI.h                                            */
/*    DATE: Oct0411                                              */
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

#ifndef ULH_GUIX_HEADER
#define ULH_GUIX_HEADER

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Browser.H>
#include "HelmTPlot.h"
#include "HelmScopeModel.h"
#include "MY_Output.h"

class ULH_GUI : public Fl_Window {
public:
  ULH_GUI(int w, int h, const char *l=0);
  ~ULH_GUI();
  
  static Fl_Menu_Item menu_[];
  Fl_Menu_Bar *mbar;

  int handle(int);

  void setWindowLayout(std::string layout);
  void setHelmPlot(const HelmTPlot&); 
  void setCurrTime(double=-1);

  void updateXY();
  void updateBrowsers();

private:
  inline void cb_Browser_i();
  static void cb_Browser(Fl_Widget*);

  inline void cb_BrowserHeader_i();
  static void cb_BrowserHeader(Fl_Widget*);

  inline void cb_ButtonErrors_i();
  static void cb_ButtonErrors(Fl_Widget*);

  inline void cb_ButtonWarnings_i();
  static void cb_ButtonWarnings(Fl_Widget*);

  inline void cb_ButtonModeTree_i();
  static void cb_ButtonModeTree(Fl_Widget*);

  inline void cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

 public:
  HelmScopeModel hsm;

protected:
  std::string m_vname;

  MY_Output   *m_fld_utc_time;
  MY_Output   *m_fld_loc_time;
  MY_Output   *m_fld_mode;
  MY_Output   *m_fld_iter;
  MY_Output   *m_fld_decision;
  MY_Output   *m_fld_engaged;

  Fl_Check_Button  *m_but_gen_errors;
  Fl_Check_Button  *m_but_gen_warnings;
  Fl_Check_Button  *m_but_gen_modetree;

  Fl_Browser *m_brw_active;
  Fl_Browser *m_brw_running;
  Fl_Browser *m_brw_idle;
  Fl_Browser *m_brw_completed;
  
  Fl_Browser *m_brw_general;

  unsigned int m_cnt_active;
  unsigned int m_cnt_running;
  unsigned int m_cnt_idle;
  unsigned int m_cnt_completed;
  unsigned int m_cnt_general;
};
#endif





