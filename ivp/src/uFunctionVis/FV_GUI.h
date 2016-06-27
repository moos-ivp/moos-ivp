/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FV_GUI.h                                             */
/*    DATE: May 13th 2006                                        */
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

#ifndef FUNCTION_VIEWER_GUI_HEADER
#define FUNCTION_VIEWER_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "FL/Fl_Output.h"
#include "FL/Fl_Menu_Button.h"
#include "FL/Fl_Button.h"
#include "FV_Viewer.h"
#include "FV_Model.h"
#include "Common_IPF_GUI.h"

class FV_GUI : public Common_IPF_GUI {
public:
  FV_GUI(int w, int h, const char *l=0);
  virtual ~FV_GUI() {};

 public:
  void augmentMenu();
  int  handle(int);
  void setModel(FV_Model* g_model)
    {m_model = g_model; m_fv_viewer->setModel(g_model);}

  void updateFields();
  void addBehaviorSource(std::string source);

  FV_Viewer* getViewer() {return(m_fv_viewer);}

 protected:
  void  resize(int, int, int, int);
  void  initWidgets();
  void  resizeWidgetsShape();
  void  resizeWidgetsText(); 
  
 private:
  inline void cb_BehaviorSelect_i(int);
  static void cb_BehaviorSelect(Fl_Widget*, int);

  inline void cb_StretchRad_i(int);
  static void cb_StretchRad(Fl_Widget*, int);

  inline void cb_ToggleLockIPF_i();
  static void cb_ToggleLockIPF(Fl_Widget*);

  inline void cb_IncCurrFunction_i(int);
  static void cb_IncCurrFunction(Fl_Widget*, int);

  inline void cb_ToggleCollectiveView_i();
  static void cb_ToggleCollectiveView(Fl_Widget*);

  inline void cb_ToggleSet_i();
  static void cb_ToggleSet(Fl_Widget*);

  inline void cb_TogglePin_i();
  static void cb_TogglePin(Fl_Widget*);

protected:
  Fl_Output   *m_fld_curr_plat;

  Fl_Output   *m_fld_curr_pcs;
  Fl_Output   *m_fld_curr_pwt;
  Fl_Output   *m_fld_curr_src;
  Fl_Output   *m_fld_curr_domain;
  Fl_Output   *m_fld_curr_iter;
  FV_Model    *m_model;
  FV_Viewer   *m_fv_viewer;

  Fl_Button   *m_but_ipf_set;
  Fl_Button   *m_but_ipf_pin;

  int m_start_hgt;
  int m_start_wid;

  std::vector<std::string> m_bhv_sources;
};
#endif





