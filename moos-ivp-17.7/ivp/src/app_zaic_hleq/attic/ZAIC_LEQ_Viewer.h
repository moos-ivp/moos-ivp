/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_LEQ_Viewer.h                                    */
/*    DATE: April 6th, 2008                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_LEQ_VIEWER_HEADER
#define ZAIC_LEQ_VIEWER_HEADER

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "ZAIC_LEQ.h"
#include "ZAIC_HEQ.h"
#include "IvPDomain.h"

class ZAIC_LEQ_Viewer : public Fl_Gl_Window
{
 public:
  ZAIC_LEQ_Viewer(int x,int y,int w,int h,const char *l=0);
  ~ZAIC_LEQ_Viewer();
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  void  moveX(double amt);
  void  currMode(int);
  void  setIPFunction();
  void  toggleHLEQ() {m_leq_mode = !m_leq_mode; setIPFunction();};

  double getSummit();
  double getSummitDelta();
  double getBaseWidth();
  double getMinUtil();
  double getMaxUtil();
  void   setParam(std::string, std::string);

 protected:
  void  setColorScheme(int index);
  void  initZAICs();
  void  drawAxes();
  void  drawLabels();
  void  drawPieces();
  void  drawPiece(const IvPBox*, int mode=0);
  void  drawText(int, int, std::string);

 protected:

  ZAIC_HEQ      *m_zaic_heq;
  ZAIC_LEQ      *m_zaic_leq;
  IvPFunction   *m_ipf;
  IvPDomain      m_domain;
  bool           m_leq_mode;

  int    m_curr_mode;
  int    m_grid_block_size;
  double m_back_shade;
  double m_grid_shade;
  double m_line_shade;

  int          m_color_scheme;
  std::string  m_label_color;
  std::string  m_line_color;
  std::string  m_display_color;
  std::string  m_point_color;

  int    x_offset;
  int    y_offset;
  int    y_grid_height;
  int    x_grid_width;
};
#endif 
