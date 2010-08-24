/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CPAViewer.h                                          */
/*    DATE: Feb 12, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef CPA_VIEWER_HEADER
#define CPA_VIEWER_HEADER

#include <vector>
#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "MarineViewer.h"
#include "CPA_Model.h"
#include "XYPolygon.h"

class CPAViewer : public MarineViewer
{
 public:
  CPAViewer(CPA_Model*, int x,int y,int w,int h, const char *l=0);
  ~CPAViewer() {};

  // Pure virtuals that need to be defined
  void   draw();
  int    handle(int);
  void   handle_left_mouse(int, int);
  void   handle_right_mouse(int, int);

  bool  setParam(std::string p, std::string v)
    {return(MarineViewer::setParam(p,v));};
  bool  setParam(std::string p, double v)
    {return(MarineViewer::setParam(p,v));};

  void   resetPan();

 protected:
  void   draw_cn_vector(int, double=6, double=0.5, double=0.5, double=0.5);
  void   draw_os_point(double=6, double=0.5, double=0.5, double=0.5);

  void   configureRadials(unsigned int radials, double dist);

 private:
  CPA_Model* m_cpa_model;
  double     m_snap_val;

  double     m_start_pan_x;
  double     m_start_pan_y;

  std::vector<XYPolygon> m_radials;

};

#endif 
