/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IPP_GUI.h                                            */
/*    DATE: Nov 30th 2014                                        */
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

#ifndef COMMON_IPF_GUI_HEADER
#define COMMON_IPF_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>

#include "IvPProblem.h"
#include "Common_IPFViewer.h"

class Common_IPF_GUI : public Fl_Window {
public:
  Common_IPF_GUI(int w, int h, const char *l=0);
  virtual ~Common_IPF_GUI() {};

  virtual void updateXY()=0;
  
  bool setViewerParam(std::string, std::string);
  bool setViewerParam(std::string, double);
  
 protected:
  void augmentMenu();
  void setMenuColors();
  void setMenuItemColor(std::string);
  int  handle(int);
  
 protected:
  Common_IPFViewer  *m_viewer;
  Fl_Menu_Bar       *m_menubar;

  int m_start_hgt;
  int m_start_wid;
  
 protected:
  inline void cb_Zoom_i(int);
  static void cb_Zoom(Fl_Widget*, int);

  inline void cb_Reset_i(int);
  static void cb_Reset(Fl_Widget*, int);

  inline void cb_RotateX_i(int);
  static void cb_RotateX(Fl_Widget*, int);

  inline void cb_RotateZ_i(int);
  static void cb_RotateZ(Fl_Widget*, int);

  inline void cb_ModScale_i(int);
  static void cb_ModScale(Fl_Widget*, int);

  inline void cb_ModShipScale_i(int);
  static void cb_ModShipScale(Fl_Widget*, int);

  inline void cb_ModBaseIPF_i(int);
  static void cb_ModBaseIPF(Fl_Widget*, int);

  inline void cb_ModFrameBaseIPF_i(int);
  static void cb_ModFrameBaseIPF(Fl_Widget*, int);

  inline void cb_ToggleIPF_i();
  static void cb_ToggleIPF(Fl_Widget*);

  inline void cb_ToggleDrawShip_i();
  static void cb_ToggleDrawShip(Fl_Widget*);

  inline void cb_ToggleFrame_i();
  static void cb_ToggleFrame(Fl_Widget*);

  inline void cb_TogglePieceLines_i();
  static void cb_TogglePieceLines(Fl_Widget*);

  inline void cb_ToggleDrawPcs_i();
  static void cb_ToggleDrawPcs(Fl_Widget*);

  inline void cb_FrameHgt_i(int);
  static void cb_FrameHgt(Fl_Widget*, int);

  inline void cb_FrameShade_i(int);
  static void cb_FrameShade(Fl_Widget*, int);

  inline void cb_ColorMap_i(int);
  static void cb_ColorMap(Fl_Widget*, int);

  inline void cb_ColorBack_i(int);
  static void cb_ColorBack(Fl_Widget*, int);

  inline void cb_Polar_i(int);
  static void cb_Polar(Fl_Widget*, int);

  static void cb_Quit();
};
#endif






