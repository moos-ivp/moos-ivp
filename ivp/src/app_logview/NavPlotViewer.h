/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: NavPlotViewer.h                                      */
/*    DATE: May 31st, 2005                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef NAVPLOT_VIEWER_HEADER
#define NAVPLOT_VIEWER_HEADER

#include <vector>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "XYGrid.h"
#include "NavPlot.h"
#include "MarineViewer.h"

class NavPlotViewer : public MarineViewer
{
 public:
  NavPlotViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~NavPlotViewer() {};
  
  void   draw();
  float  getMetersX();
  float  getMetersY();

  bool   setCurrIndex(int);
  bool   incCurrIndex(int);
  bool   jumpCurrIndex(int);
  void   setGlobalIndex(int);
  bool   vehicle(int);
  void   addNavPlot(const NavPlot&);
  float  getCurrTime();
  float  getAvgStepTime();

 public: // Configuration parameters
  void   setVehiBody(std::string s)  {m_vehibody = s;};
  void   setFrame(std::string s)     {m_frame = s;};
  void   toggleAllTrail()            {m_alltrail = !m_alltrail;};

protected:
  void  drawNavPlots();
  void  drawNavPlot(int ix);
  void  drawFrame();
  
private:
  int                   m_local_ix;
  std::string           m_vehibody;
  std::string           m_frame;
  std::vector<NavPlot>  m_navplots;
  bool                  m_alltrail;
};

#endif 

