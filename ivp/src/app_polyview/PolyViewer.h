/*****************************************************************
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PolyViewer.h                                         */
/*    DATE: May 1st, 2005                                        */
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

#ifndef POLY_VIEWER_HEADER
#define POLY_VIEWER_HEADER

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "MarineViewer.h"
#include "XYCircle.h"
#include "XYArc.h"

class PolyViewer : public MarineViewer
{
 public:
  PolyViewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  handle_left_mouse(int, int);
  void  handle_right_mouse(int, int);

public:
  float getImgPctX(int);
  float getImgPctY(int);
  float getMetersX(int);
  float getMetersY(int);
  float getLonX(int);
  float getLatY(int);
  std::string getPolySpec();

  void  adjustActive(int);
  void  createNew(); 
  void  shiftHorzPoly(double);
  void  shiftVertPoly(double);
  void  rotatePoly(int);
  void  growPoly(int);
  void  reversePoly();
  void  duplicateActive();
  void  clearActivePoly();
  void  clearPoly()               {m_poly.clear(); m_drop_mode=0;};
  void  setMode(int v)            {m_drop_mode = v;};
  int   getMode()                 {return(m_drop_mode);};
  void  setSnap(float v)          {m_snap_val  = v;};
  void  adjustMode8Angle(float v) {m_mode8_angle  += v;};
  float getSnap()                 {return(m_snap_val);};
  void  reApplySnapToCurrent();   
  void  addCircle(XYCircle);
  void  drawCircles();
  void  drawCircle(int ix);
  void  addArc(XYArc);
  void  drawArcs();
  void  drawArc(XYPolygon&, int style=0);
  void  drawVector(double, double, double);

 protected:
  void  drawMode4();
  void  drawMode5();
  void  drawMode6();
  void  drawMode7();
  void  drawMode8();
  void  drawMode9();

 private:
  float      m_snap_val;
  int        m_active_poly;
  double     m_test_mx;
  double     m_test_my;
  double     m_test_mxx;
  double     m_test_myy;
  int        m_drop_mode;
  double     m_mode8_angle;
  int        counter;

  std::vector<XYCircle>   m_circle;
  std::vector<XYPolygon>  m_circle_poly;
  std::vector<XYArc>      m_arc;
  std::vector<XYPolygon>  m_arc_poly;
};

#endif 

