/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GeoViewer.h                                          */
/*    DATE: May 1st, 2005 (Major Modifications November 2007)    */
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

class GeoViewer : public MarineViewer
{
 public:
  GeoViewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  handle_left_mouse(int, int);
  void  handle_right_mouse(int, int);
  bool  setParam(std::string p, std::string v)
    {return(setCommonParam(p,v));};
  bool  setParam(std::string p, float v);
  //    {return(setCommonParam(p,v));};

public:
  std::string getPolySpec();

  void  adjustActive(int);
  void  createNew(); 
  void  shiftHorzPoly(double);
  void  shiftVertPoly(double);
  void  rotatePoly(unsigned int);
  void  growPoly(unsigned int);
  void  reversePoly();
  void  duplicateActive();
  void  clearActivePoly();
  void  clearPoly()               {m_poly.clear(); m_drop_mode=0;};
  int   getMode()                 {return(m_drop_mode);};
  float getSnap()                 {return(m_snap_val);};
  void  reApplySnapToCurrent();   
  void  addCircle(XYCircle);
  void  drawCircles();
  void  drawCircle(unsigned int ix);
  void  drawVector(double, double, double);

 private:
  unsigned int  m_active_poly;

  float   m_snap_val;
  int     m_drop_mode;

  std::vector<XYCircle>   m_circle;
  std::vector<XYPolygon>  m_circle_poly;
};

#endif 

