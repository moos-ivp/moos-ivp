/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MarineGUI.h                                          */
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

#ifndef COMMON_MARINE_GUI_HEADER
#define COMMON_MARINE_GUI_HEADER

#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "MarineViewer.h"

class MarineGUI : public Fl_Window {
public:
  MarineGUI(int w, int h, const char *l=0);
  ~MarineGUI() {};

  static Fl_Menu_Item menu_[];

  void readTiff(std::string);
  void readTiffB(std::string);
  int  handle(int);
  void addGrid(XYGrid);
  void addPoly(XYPolygon);
  void addCircle(XYCircle);
  virtual void updateXY() {};

  void augmentMenu();

public:
  Fl_Menu_Bar  *mbar;
  MarineViewer *cmviewer;

protected:
  std::vector<std::string> grid_labels;
 
protected:
  inline void cb_Zoom_i(int);
  static void cb_Zoom(Fl_Widget*, int);

  inline void cb_PanY_i(int);
  static void cb_PanY(Fl_Widget*, int);

  inline void cb_PanX_i(int);
  static void cb_PanX(Fl_Widget*, int);

  inline void cb_ToggleTiff_i();
  static void cb_ToggleTiff(Fl_Widget*);

  inline void cb_ToggleTiffType_i();
  static void cb_ToggleTiffType(Fl_Widget*);

  inline void cb_ToggleHash_i();
  static void cb_ToggleHash(Fl_Widget*);

  inline void cb_HashDelta_i(int);
  static void cb_HashDelta(Fl_Widget*, int);

  inline void cb_HashShade_i(int);
  static void cb_HashShade(Fl_Widget*, int);

  inline void cb_MG_SetGeoAttr_i(int);
  static void cb_MG_SetGeoAttr(Fl_Widget*, int);

  inline void cb_ToggleDatum_i();
  static void cb_ToggleDatum(Fl_Widget*);

  inline void cb_ToggleGrids_i();
  static void cb_ToggleGrids(Fl_Widget*);

  inline void cb_DatumSize_i(int);
  static void cb_DatumSize(Fl_Widget*, int);

  inline void cb_BackShade_i(int);
  static void cb_BackShade(Fl_Widget*, int);

  inline void cb_ToggleCross_i();
  static void cb_ToggleCross(Fl_Widget*);

  static void cb_Quit();
};
#endif


