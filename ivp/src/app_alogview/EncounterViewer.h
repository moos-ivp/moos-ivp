/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: EncounterViewer.h                                    */
/*    DATE: Jan 11th, 2016                                       */
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

#ifndef ENCOUNTER_VIEWER_HEADER
#define ENCOUNTER_VIEWER_HEADER

#include <vector>
#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "ALogDataBroker.h"
#include "EncounterPlot.h"
#include "ColorPack.h"

class EncounterViewer : public Fl_Gl_Window
{
 public:
  EncounterViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~EncounterViewer() {}

  void   draw();
  int    handle(int event);
  void   resize(int, int, int, int);
  
 public: // Setters
  void   setDataBroker(ALogDataBroker, std::string vname);
  void   setEncounterPlot(std::string vname);
  void   setClearColor(std::string s)  {m_clear_color.setColor(s);}

  void   setDrawMinEff(bool v) {m_draw_mineff=v;}
  void   setDrawAvgEff(bool v) {m_draw_avgeff=v;}
  void   setDrawMinCPA(bool v) {m_draw_mincpa=v;}
  void   setDrawAvgCPA(bool v) {m_draw_avgcpa=v;}
  void   setShowAllPts(bool v) {m_show_allpts=v;}
  
  void   setTime(double tstamp);
  void   setVName(std::string s) {m_vname=s;}
  void   setMutableTextSize(int v) {m_mutable_text_size=v;}

 public: // Getters
  double getCurrTime() const;
  double getMinCPA() const {return(m_min_cpa);}
  double getMinEFF() const {return(m_min_eff);}
  
  std::string getTotalEncounters() const;

private:
  double         m_curr_time;
  ALogDataBroker m_dbroker;
  int            m_mutable_text_size;

  // Vehicle name stays constant once it is set initially
  std::string    m_vname; 

  bool   m_draw_mineff;
  bool   m_draw_avgeff;
  bool   m_draw_mincpa;
  bool   m_draw_avgcpa;
  bool   m_show_allpts;

  double m_cpa_collision;
  double m_cpa_near_miss;

  double m_min_cpa;
  double m_min_eff;
  
  EncounterPlot  m_encounter_plot;
  ColorPack      m_clear_color;
  ColorPack      m_label_color;
  ColorPack      m_mineff_color;
  ColorPack      m_avgeff_color;  
  ColorPack      m_mincpa_color;
  ColorPack      m_avgcpa_color;  
};

#endif
