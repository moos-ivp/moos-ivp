/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: QuadViewer.h                                         */
/*    DATE: June 30th, 2005                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef QUAD_VIEWER_HEADER
#define QUAD_VIEWER_HEADER

#include <vector>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"

class QuadViewer : public Fl_Gl_Window
{
 public:
  QuadViewer(int x,int y,int w,int h,const char *l=0);
  ~QuadViewer() {};
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  std::string get_pstring(int);
  std::string get_coeff(int);
  double get_min_y() {return(min_y);};
  double get_min_x() {return(min_x);};
  double get_max_y() {return(max_y);};
  double get_max_x() {return(max_x);};
  void   move_x(int);
  void   move_y(int);
  void   curr_pt(int);

 protected:
  bool  fill_cache();
  void  set_coeffs(double, double, double, double, double, double, 
		   double&, double&, double&);
  void  set_bounds();
  void  set_xcoeffs();

  int   handle_left_mouse(int, int);
  int   handle_right_mouse(int, int);
  void  draw_plot();
  void  draw_axes();
  void  draw_points();
  
 protected:
  std::vector<double>  cache_x1;
  std::vector<double>  cache_y1;
  std::vector<double>  cache_x2;
  std::vector<double>  cache_y2;

  bool   valid_cache;
  int    curr_point;

  double px1;
  double py1;
  double px2;
  double py2;
  double px3;
  double py3;

  double qm;
  double qn;
  double qb;

  double qxm;
  double qxn;
  double qxb;

  double xlow;
  double xhigh;
  double ylow; 
  double yhigh;

  double min_y;
  double min_x;
  double max_y;
  double max_x;

};

#endif 





