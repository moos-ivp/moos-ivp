/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_GUI.h                                           */
/*    DATE: June 2nd, 2015                                       */
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

#ifndef ZAIC_GUI_HEADER
#define ZAIC_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include "ZAIC_Viewer.h"
#include "ZAIC_Model.h"

class ZAIC_GUI : public Fl_Window {
public:
  ZAIC_GUI(int w, int h, const char *l=0);
  ~ZAIC_GUI() {};
  
  static Fl_Menu_Item menu_[];

  virtual void updateOutput() {};
  
 public:
  void setVerbose(bool v)        {m_zaic_viewer->setVerbose(v);}
  void setDomain(unsigned int v) {m_zaic_model->setDomain(v);}

  void setParam(std::string p, std::string v) {m_zaic_model->setParam(p,v);}
  int  handle(int);
  
 public:
  Fl_Menu_Bar  *mbar;
  ZAIC_Viewer  *m_zaic_viewer;
  ZAIC_Model   *m_zaic_model;
  
 protected:
  static void cb_Quit();

  inline void cb_MoveX_i(int);
  static void cb_MoveX(Fl_Widget*, int);

  inline void cb_CurrMode_i(int);
  static void cb_CurrMode(Fl_Widget*, int);

  inline void cb_GridSize_i(int);
  static void cb_GridSize(Fl_Widget*, int);

  inline void cb_GridShade_i(int);
  static void cb_GridShade(Fl_Widget*, int);

  inline void cb_LineShade_i(int);
  static void cb_LineShade(Fl_Widget*, int);

  inline void cb_BackShade_i(int);
  static void cb_BackShade(Fl_Widget*, int);

  inline void cb_ToggleVerbose_i();
  static void cb_ToggleVerbose(Fl_Widget*);

  inline void cb_ToggleLabels_i();
  static void cb_ToggleLabels(Fl_Widget*);

  inline void cb_ToggleColorScheme_i();
  static void cb_ToggleColorScheme(Fl_Widget*);
};
#endif




