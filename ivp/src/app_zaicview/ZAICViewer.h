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
#include "ZAIC_LEQ.h"
#include "ZAIC_HEQ.h"
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
  void  toggleLEQ();
  void  toggleHEQ();
  void  toggleWRAP();
  void  currMode(int);
  void  setDomain(IvPDomain v) {domain=v;};
  void  setIPFunction();

  double getSummit()      {return(summit);};
  double getBaseWidth()   {return(base_width);};
  double getPeakWidth()   {return(peak_width);};
  double getSummitDelta() {return(summit_delta);};
  double getSummitLEQ()   {return(summit_leq);};
  double getSummitHEQ()   {return(summit_heq);};

 protected:
  void  setGridSize();
  void  drawAxes();
  void  drawPieces();
  void  drawPiece(double xl, double xh, double slope, double intcpt);
  void  drawPiece(const IvPBox*);
  
 protected:

  ZAIC_PEAK       *zaic_peak;
  ZAIC_LEQ        *zaic_leq;
  ZAIC_HEQ        *zaic_heq;
  IvPFunction     *ipf;
  IvPDomain        domain;

  double summit;
  double base_width;
  double peak_width;
  double summit_delta;
  bool   summit_leq;
  bool   summit_heq;
  bool   wrap;
  
  double xlow;
  double xhigh;
  double ylow; 
  double yhigh;

  int    curr_mode;

  int    x_offset;
  int    y_offset;
  int    y_grid_height;
  int    x_grid_width;
  int    grid_block_size;

};


#endif 

