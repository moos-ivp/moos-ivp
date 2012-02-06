/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "LogTPlot.h"
#include "VPlugTPlot.h"
#include "MarineViewer.h"
#include "PostingBuffer.h"

class NavPlotViewer : public MarineViewer
{
 public:
  NavPlotViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~NavPlotViewer() {};
  
  void   draw();
  bool   setParam(std::string p, std::string v);
  bool   setParam(std::string p, double v);  
  void   addLogPlotNAVX(const LogTPlot& lp);
  void   addLogPlotNAVY(const LogTPlot& lp); 
  void   addLogPlotHDG(const LogTPlot& lp); 
  void   addLogPlotStartTime(double); 
  void   addVPlugPlot(const VPlugTPlot& vp); 
  // void   addVName(const std::string& s) {m_vname.push_back
      
  
  void   setCenterView(std::string centering="ctr_of_bounding");
  void   setStepType(const std::string&);
  void   stepTime(double);
  void   setCurrTime(double);
  void   setIndex(unsigned int, unsigned int);
  double getCurrTime();
  double getStartTimeHint();

  std::map<std::string, unsigned int> getVIterMap();

 public: // Configuration parameters
  void   setFrame(std::string s)   {m_frame = s;};

protected:
  void  drawNavPlots();
  void  drawNavPlot(unsigned int ix);
  void  drawTrails();
  void  drawTrail(unsigned int ix);
  void  drawVPlugPlots();
  void  drawVPlugPlot(unsigned int ix);
  void  drawFrame();

  std::string shortenBehaviors(std::string);

 public:
  PostingBuffer m_pbuffer;

private:
  unsigned int  m_hplot_left_ix;
  unsigned int  m_hplot_right_ix;
  double        m_curr_time;
  std::string   m_frame;


  // Bounding box of all vehicle positions and timestamps
  double        m_min_time;
  double        m_max_time;
  double        m_min_xpos;
  double        m_min_ypos;
  double        m_max_xpos;
  double        m_max_ypos;

  // vectors - each index corresponds to one vehicle
  std::vector<LogTPlot>    m_navx_plot;
  std::vector<LogTPlot>    m_navy_plot;
  std::vector<LogTPlot>    m_hdg_plot;
  std::vector<VPlugTPlot>  m_vplug_plot;
  std::vector<double>      m_start_time;
  std::vector<std::string> m_vname;
  std::vector<std::string> m_vtype;
  std::vector<double>      m_vlength;

  std::string m_trails;
  int         m_trail_gap;
  bool        m_behaviors_verbose;
  bool        m_step_by_secs;
};

#endif 
