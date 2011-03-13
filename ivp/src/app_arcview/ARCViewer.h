/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ARCViewer.h                                          */
/*    DATE: Jan 16th, 2011                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ARC_VIEWER_HEADER
#define ARC_VIEWER_HEADER

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "ZAIC_PEAK.h"
#include "IvPDomain.h"

class ARCViewer : public Fl_Gl_Window
{
 public:
  ARCViewer(int x,int y,int w,int h,const char *l=0);
  ~ARCViewer() {};
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  void  setARC(int);
  void  moveX(double amt);
  void  currMode(int);
  void  setIPFunctions();

  double getSummit();
  double getBaseWidth();
  double getPeakWidth();
  double getSummitDelta();
  double getMaxUtil();
  double getMinUtil();

  void   setParam(std::string, std::string);

 protected:
  void  drawAxes();
  void  drawLabels();
  void  drawPieces();
  void  drawPiece(const IvPBox*, int mode=0);
  void  drawText(int, int, std::string);

 protected:
  ZAIC_PEAK     *m_zaic;
  IvPFunction   *m_ipf;
  IvPDomain      m_domain;

  int    m_color_scheme;
  int    m_curr_mode;
  int    m_grid_block_size;
  double m_back_shade;
  double m_grid_shade;
  double m_line_shade;
  bool   m_wrap;

  int    x_offset;
  int    y_offset;
  int    y_grid_height;
  int    x_grid_width;
};
#endif 
