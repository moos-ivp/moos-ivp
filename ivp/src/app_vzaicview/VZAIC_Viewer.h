/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VZAIC_Viewer.h                                       */
/*    DATE: May 5th, 2010                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef VZAIC_VIEWER_HEADER
#define VZAIC_VIEWER_HEADER

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "ZAIC_Vector.h"
#include "IvPDomain.h"

class VZAIC_Viewer : public Fl_Gl_Window
{
 public:
  VZAIC_Viewer(int x,int y,int w,int h,const char *l=0);
  ~VZAIC_Viewer() {};
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  void  setZAIC(ZAIC_Vector *zaic);

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

  ZAIC_Vector *m_zaic;
  IvPDomain    m_domain;

  int    m_grid_block_size;
  double m_back_shade;
  double m_grid_shade;
  double m_line_shade;
  bool   m_draw_labels;

  int    x_offset;
  int    y_offset;
  int    m_grid_height;
  int    m_grid_width;
};
#endif 
