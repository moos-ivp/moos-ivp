/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAICViewer.h                                         */
/*    DATE: June 17th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_VIEWER_HEADER
#define ZAIC_VIEWER_HEADER

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "ZAIC_PEAK.h"
#include "IvPDomain.h"

class ZAICViewer : public Fl_Gl_Window
{
 public:
  ZAICViewer(int x,int y,int w,int h,const char *l=0);
  ~ZAICViewer() {};
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  void  moveX(double amt);
  void  toggleWRAP();
  void  currMode(int);
  void  setDomain(IvPDomain v) {domain=v;};
  void  setIPFunction();
  void  alterIndex(int);
  void  toggleDrawTotal() {draw_total = !draw_total;};

  double getSummit();
  double getBaseWidth();
  double getPeakWidth();
  double getSummitDelta();
  double getSummitHeight();

 protected:
  void  setGridSize();
  void  drawAxes();
  void  drawPieces();
  void  drawPiece(const IvPBox*, int mode=0);
  
 protected:

  ZAIC_PEAK     *zaic;
  IvPFunction   *ipf_max;
  IvPFunction   *ipf_tot;
  IvPDomain      domain;

  double summit;
  double base_width;
  double peak_width;
  double summit_delta;
  bool   wrap;
  bool   draw_total;
  
  double xlow;
  double xhigh;
  double ylow; 
  double yhigh;

  int    curr_mode;

  int    zaic_index;

  int    x_offset;
  int    y_offset;
  int    y_grid_height;
  int    x_grid_width;
  int    grid_block_size;

};


#endif 
