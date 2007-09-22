/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HexaViewer.h                                         */
/*    DATE: Oct 30, 2005                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef HEXA_VIEWER_HEADER
#define HEXA_VIEWER_HEADER

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "XYHexagon.h"
#include "MarineViewer.h"

class HexaViewer : public MarineViewer
{
 public:
  HexaViewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  handle_left_mouse(int, int);
  void  handle_right_mouse(int, int);
  bool  setParam(std::string p, std::string v)
    {return(setCommonParam(p,v));};
  bool  setParam(std::string p, float v)
    {return(setCommonParam(p,v));};


public:
  std::string getPolySpec(int);

  void  adjustActive(int); 
  void  shiftHorzPoly(double);
  void  shiftVertPoly(double);
  void  clearActivePoly();
  void  toggleView();
  void  clearPoly()              {m_poly.clear();};
  void  setSnap(float v)         {m_snap_val  = v;};
  float getSnap()                {return(m_snap_val);};
  void  reApplySnapToCurrent();   

 private:
  float      m_snap_val;
  int        m_active_poly;
  double     pdist;
  int        view_state;

  std::vector<XYHexagon>  m_hexa;

};

#endif 
