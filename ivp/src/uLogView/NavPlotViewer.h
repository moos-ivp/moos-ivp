/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NavPlotViewer.h                                      */
/*    DATE: May 31st, 2005                                       */
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

#ifndef NAVPLOT_VIEWER_HEADER
#define NAVPLOT_VIEWER_HEADER

#include <vector>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "LogTPlot.h"
#include "VPlugTPlot.h"
#include "MarineViewer.h"
#include "PostingBuffer.h"
#include "NavPlotElement.h"

class NavPlotViewer : public MarineViewer
{
 public:
  NavPlotViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~NavPlotViewer() {};
  
  void   draw();
  bool   setCurrTime(double, bool post=true);
  bool   setParam(std::string p, std::string v);
  bool   setParam(std::string p, double v);  
  void   setTimeFormat(const std::string&);

  void   addElement(const NavPlotElement&);
  void   setMinMaxTimeUTC(double, double);
  
  void   setCenterView(std::string centering="ctr_of_bounding");
  bool   stepTime(double);
  double getCurrTime();

  std::string getCurrTimeStr();

protected:
  void  drawNavPlots();
  void  drawNavPlot(unsigned int ix);
  void  drawTrails();
  void  drawTrail(unsigned int ix);
  void  drawVPlugPlots();
  void  drawVPlugPlot(unsigned int ix);

 public: // fix me
  PostingBuffer m_pbuffer;

private:
  double        m_curr_time;
  bool          m_time_format_utc;

  // Bounding box of all vehicle positions and timestamps
  double        m_min_time_utc;
  double        m_max_time_utc;
  double        m_min_xpos;
  double        m_min_ypos;
  double        m_max_xpos;
  double        m_max_ypos;

  // One for each vehicle (alog file)
  std::vector<NavPlotElement> m_elements;

  // Rendering preferences
  std::string m_trails;
  int         m_trail_gap;
};

#endif 




