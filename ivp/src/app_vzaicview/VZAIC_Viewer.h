/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VZAIC_Viewer.h                                       */
/*    DATE: May 5th, 2010                                        */
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
  ~VZAIC_Viewer() {}
  
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





