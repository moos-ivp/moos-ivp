/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAICViewer.h                                         */
/*    DATE: June 17th, 2006                                      */
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
  ~ZAICViewer() {}
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  resize(int, int, int, int);

 public:
  void  setZAIC(int);
  void  moveX(double amt);
  void  currMode(int);
  void  setIPFunctions();
  void  toggleIndex();

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

  ZAIC_PEAK     *m_zaic_1;
  ZAIC_PEAK     *m_zaic_2;
  ZAIC_PEAK     *m_zaic_C;
  ZAIC_PEAK     *m_zaic_H;

  IvPFunction   *ipf_cur;
  IvPFunction   *ipf_one;
  IvPFunction   *ipf_two;
  IvPFunction   *ipf_max;
  IvPFunction   *ipf_tot;
  IvPDomain      m_domain;

  // 1:zaic_1, 2:zaic_2, 3:zaic_both, 4:zaic_max, 5:zaic_sum
  int    m_draw_mode; 

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





