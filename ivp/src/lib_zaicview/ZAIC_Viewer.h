/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_Viewer.h                                    */
/*    DATE: June 2nd, 2015                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef ZAIC_GENERAL_VIEWER_HEADER
#define ZAIC_GENERAL_VIEWER_HEADER

// Defined to silence GL deprecation warnings in OSX
#define GL_SILENCE_DEPRECATION

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "ZAIC_Model.h"
#include "IvPDomain.h"

class ZAIC_Viewer : public Fl_Gl_Window
{
 public:
  ZAIC_Viewer(int x,int y,int w,int h,const char *l=0);
  virtual ~ZAIC_Viewer();
  
  // virtuals overloaded
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  void  setModel(ZAIC_Model*);
  void  setParam(std::string, std::string);
  void  setVerbose(bool v) {m_verbose=v;}
  void  toggleVerbose()    {m_verbose=!m_verbose;}

  unsigned int getTotalPieces() const {return(m_pieces);}

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

  unsigned int m_pieces;
  
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




