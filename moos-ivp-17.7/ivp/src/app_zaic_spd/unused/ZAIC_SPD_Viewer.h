/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_Viewer.h                                    */
/*    DATE: June 2nd, 2015                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_SPD_VIEWER_HEADER
#define ZAIC_SPD_VIEWER_HEADER

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "ZAIC_Model.h"
#include "IvPDomain.h"

class ZAIC_SPD_Viewer : public Fl_Gl_Window
{
 public:
  ZAIC_SPD_Viewer(int x,int y,int w,int h,const char *l=0);
  ~ZAIC_SPD_Viewer();
  
  // virtuals overloaded
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  void  setModel(ZAIC_Model*);
  void  setParam(std::string, std::string);
  void  setVerbose(bool v) {m_verbose=v;}
  void  toggleVerbose()    {m_verbose=!m_verbose;}


 protected:
  void  setColorScheme(int index);
  void  setCellDomainWidth();
  void  drawAxes();
  void  drawLabels();
  void  drawPieces();
  void  drawPiece(const IvPBox*, int mode=0);
  void  drawText(int, int, std::string);

 protected:
  ZAIC_Model* m_model;

  double m_back_shade;
  double m_grid_shade;
  double m_line_shade;
  bool   m_draw_labels;

  int    m_dompts;
  double m_pixels_per_dompt;

  bool   m_verbose;
  
  int          m_color_scheme;
  std::string  m_label_color;
  std::string  m_line_color;
  std::string  m_display_color;
  std::string  m_point_color;

  int    m_xoffset;       // Margin x
  int    m_yoffset;       // Margin y
  int    m_grid_hgt;      // overall grid height in pixels
  int    m_grid_wid;      // overall grid width  in pixels
  int    m_cell_pix_hgt;
  int    m_cell_pix_wid;
  int    m_cell_dom_wid;
};
#endif 
