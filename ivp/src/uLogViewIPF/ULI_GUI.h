/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULI_GUI.h                                            */
/*    DATE: Sep 25th, 2011                                       */
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

#ifndef ULI_GUIX_HEADER
#define ULI_GUIX_HEADER

#include <set>
#include <map>
#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Check_Button.H>
#include "IvPFuncViewerX.h"
#include <FL/Fl_Hold_Browser.H>
#include "MY_Output.h"
#include "MY_Menu_Button.h"
#include "MY_Button.h"

class ULI_GUI : public Fl_Window {
public:
  ULI_GUI(int w, int h, const char *l=0);
  ~ULI_GUI();
  
  static Fl_Menu_Item menu_[];
  Fl_Menu_Bar *mbar;

  void addIPF_Plot(const IPF_TPlot &v); 
  void updateXY();

  void setWindowLayout(std::string layout="normal");
  void setCurrTime(double=-1);

  void initChoicesIPF();

 protected:
  int   handle(int);
  void  updateSourceMenuButton();

 private:
  void cb_HandleUpDown_i(int);
  void cb_HandleLeftRight_i(int);
  static void cb_HandleUpDown(Fl_Widget*, int);
  static void cb_HandleLeftRight(Fl_Widget*, int);

  void cb_Zoom_i(int);

  inline void cb_ToggleUTC_i();
  static void cb_ToggleUTC(Fl_Widget*);

  inline void cb_ToggleSet_i();
  static void cb_ToggleSet(Fl_Widget*);

  inline void cb_TogglePin_i();
  static void cb_TogglePin(Fl_Widget*);

  inline void cb_SelectSource_i(int);
  static void cb_SelectSource(Fl_Widget*, int);

  inline void cb_Step_i(int);
  static void cb_Step(Fl_Widget*, int);

public:
  IvPFuncViewerX *ipf_viewer;
  
protected:
  MY_Output   *m_fld_utc_time;
  MY_Output   *m_fld_ipf_iter;
  MY_Output   *m_fld_ipf_pcs;
  MY_Output   *m_fld_ipf_pwt;
  MY_Output   *m_fld_ipf_dom;
  MY_Button   *m_but_ipf_set;
  MY_Button   *m_but_ipf_pin;

  Fl_Check_Button *m_but_time_utc;
  MY_Menu_Button  *m_but_ipf_source;
  std::string      m_but_ipf_source_str;
  
  std::vector<std::string> m_sources;
  std::string              m_source;
  std::string              m_vname;
};
#endif





