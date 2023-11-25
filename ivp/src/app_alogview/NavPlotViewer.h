/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "LogPlot.h"
#include "VPlugPlot.h"
#include "MarineViewer.h"
#include "ALogDataBroker.h"

class NavPlotViewer : public MarineViewer
{
 public:
  NavPlotViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~NavPlotViewer() {}
  
  void   draw();
  bool   setParam(std::string p, std::string v);
  bool   setParam(std::string p, double v);  
  void   modColorScheme() {};

  void   setVerbose(bool v) {m_verbose=v;}
  void   setMinimalMem();
  void   setDataBroker(const ALogDataBroker& dbroker);

  void   initPlots();

  void   addLogPlotNAVX(const LogPlot& lp);
  void   addLogPlotNAVY(const LogPlot& lp); 
  void   addLogPlotHDG(const LogPlot& lp); 
  void   addLogPlotNAVX_GT(const LogPlot& lp);
  void   addLogPlotNAVY_GT(const LogPlot& lp); 
  void   addLogPlotHDG_GT(const LogPlot& lp); 

  void   addLogPlotStartTime(double); 
  void   addVPlugPlot(const VPlugPlot& vp); 

  void   setStreaming(bool v) {m_streaming=v;}

  void   setCenterView(std::string centering="ctr_of_bounding");
  void   setAltNavPrefix(std::string s)   {m_alt_nav_prefix=s;}
  void   setStepType(const std::string&);
  bool   stepTime(double);
  bool   setCurrTime(double);
  void   setCurrTimeStart() {m_curr_time=m_dbroker.getPrunedMinTime();}
  void   setCurrTimeEnd() {m_curr_time=m_dbroker.getPrunedMaxTime();}
  double getCurrTime();
  double getStartTimeHint();

protected:
  void  drawNavPlots();
  void  drawNavPlot(unsigned int ix, bool alt=false);
  void  drawTrails();
  void  drawTrail(unsigned int ix);
  void  drawVPlugPlots();
  void  drawVPlugPlot(unsigned int ix);

 protected:
  ALogDataBroker m_dbroker;

private:
  double      m_curr_time;
  bool        m_step_by_secs;
  bool        m_draw_geo;
  bool        m_verbose;
  
  // Bounding box of all vehicle positions and timestamps
  double      m_min_xpos;
  double      m_min_ypos;
  double      m_max_xpos;
  double      m_max_ypos;

  std::string m_trails;
  double      m_shape_scale;
  bool        m_center_refresh;
  bool        m_streaming;

  // vectors - each index corresponds to one vehicle
  std::vector<LogPlot>   m_navx_plot;
  std::vector<LogPlot>   m_navy_plot;
  std::vector<LogPlot>   m_hdg_plot;
  std::vector<LogPlot>   m_navx_gt_plot;
  std::vector<LogPlot>   m_navy_gt_plot;
  std::vector<LogPlot>   m_hdg_gt_plot;

  std::vector<VPlugPlot> m_vplug_plot;
  std::vector<double>    m_start_time;

  std::vector<std::string> m_vnames;
  std::vector<std::string> m_vtypes;
  std::vector<std::string> m_vcolors;
  std::vector<double>      m_vlengths;

  std::string m_alt_nav_prefix;
};

#endif 





