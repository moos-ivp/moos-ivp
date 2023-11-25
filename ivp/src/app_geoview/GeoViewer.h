/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GeoViewer.h                                          */
/*    DATE: May 1st, 2005 (Major Modifications November 2007)    */
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

#ifndef POLY_VIEWER_HEADER
#define POLY_VIEWER_HEADER

// Defined to silence GL deprecation warnings in OSX 10.14+
#define GL_SILENCE_DEPRECATION

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "MarineViewer.h"
#include "VPlug_GeoShapes.h"

class GeoViewer : public MarineViewer
{
 public:
  GeoViewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtual that need to be defined
  void  modColorScheme() {};

  // Virtual defined
  void  draw();
  int   handle(int);
  void  handle_left_mouse(int, int);
  void  handle_right_mouse(int, int);
  bool  setParam(std::string param, std::string value);
  bool  setParam(std::string param, double value);

public:
  std::string getPolySpec();

  void   adjustActive(int);
  void   createNew(); 
  void   shiftHorzPoly(double);
  void   shiftVertPoly(double);
  void   rotatePoly(int);
  void   growPoly(int);
  void   reversePoly();
  void   duplicateActive();
  void   clearActivePoly();
  int    getMode()                 {return(m_drop_mode);}
  double getSnap()                 {return(m_snap_val);}
  void   reApplySnapToCurrent();   

 private:
  unsigned int  m_active_poly;

  double  m_snap_val;
  int     m_drop_mode;

  VPlug_GeoShapes  m_geoshapes;
};

#endif 











