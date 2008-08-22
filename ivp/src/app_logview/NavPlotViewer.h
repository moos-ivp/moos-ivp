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
#include "LogPlot.h"
#include "GridPlot.h"
#include "MarineViewer.h"

class NavPlotViewer : public MarineViewer
{
 public:
  NavPlotViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~NavPlotViewer() {};
  
  bool  setParam(std::string p, std::string v);
  bool  setParam(std::string p, double v);
  
  void addLogPlotNAVX(const LogPlot& lp) 
    {m_navx_plot.push_back(lp);};
  void addLogPlotNAVY(const LogPlot& lp) 
    {m_navy_plot.push_back(lp);};
  void addLogPlotHDG(const LogPlot& lp) 
    {m_hdg_plot.push_back(lp);};

  void addLogPlot(const LogPlot& lp, std::string vid, std::string type);

  void   draw();
  double getMetersX();
  double getMetersY();

  bool   setCurrIndex(int);
  bool   incCurrIndex(int);
  bool   jumpCurrIndex(unsigned int);
  void   setGridPlotIndex(unsigned int);
  void   setGlobalIndex(unsigned int);
  bool   vehicle(unsigned int);
  int    addGridPlot(const GridPlot&);
  double getCurrTime();
  double getAvgStepTime();

 public: // Configuration parameters
  void   setVehiBody(std::string s)  {m_vehibody = s;};
  void   setFrame(std::string s)     {m_frame = s;};
  void   toggleAllTrail()            {m_alltrail = !m_alltrail;};

protected:
  void  drawNavPlots();
  void  drawNavPlot(unsigned int ix);
  void  drawGridPlots();
  void  drawGridPlot(unsigned int ix);
  void  drawFrame();
  
private:
  unsigned int          m_local_ix;
  unsigned int          m_gridplot_ix;
  std::string           m_vehibody;
  std::string           m_frame;
  std::vector<GridPlot> m_gridplots;
  bool                  m_gridplots_draw;
  bool                  m_alltrail;

  std::vector<LogPlot>  m_navx_plot;
  std::vector<LogPlot>  m_navy_plot;
  std::vector<LogPlot>  m_hdg_plot;

  std::map<std::string, LogPlot> m_plotmap_navx;
  std::map<std::string, LogPlot> m_plotmap_navy;
  std::map<std::string, LogPlot> m_plotmap_hdg;

  int    m_global_ix;

  double m_shape_scale;
  bool   m_trails;
  int    m_trail_gap;
  double m_trail_size;
  bool   m_draw_vname;
};

#endif 


