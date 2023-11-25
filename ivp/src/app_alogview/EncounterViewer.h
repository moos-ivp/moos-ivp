/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

// Defined to silence GL deprecation warnings in OSX 10.14+
#define GL_SILENCE_DEPRECATION

#include <vector>
#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "ALogDataBroker.h"
#include "EncounterPlot.h"
#include "ColorPack.h"

class GUI_Encounters;

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
  void   setOwningGUI(GUI_Encounters*);
  void   setEncounterPlot(std::string vname);
  void   setClearColor(std::string s)  {m_clear_color.setColor(s);}

  void   setDrawMinEff(bool v) {m_draw_mineff=v;}
  void   setDrawAvgEff(bool v) {m_draw_avgeff=v;}
  void   setDrawMinCPA(bool v) {m_draw_mincpa=v;}
  void   setDrawAvgCPA(bool v) {m_draw_avgcpa=v;}
  void   setShowAllPts(bool v) {m_show_allpts=v;}

  void   setDrawPointSize(std::string);
  void   setTime(double tstamp);
  void   setVName(std::string s) {m_vname=s;}

 public: // Getters
  double getCurrTime() const;
  double getMinCPA() const {return(m_min_cpa);}
  double getMinEFF() const {return(m_min_eff);}
  double getAvgCPA() const {return(m_avg_cpa);}
  double getAvgEFF() const {return(m_avg_eff);}

  double getCollisionRange() const {return(m_collision_range);}
  double getNearMissRange() const  {return(m_near_miss_range);}
  double getEncounterRange() const {return(m_encounter_range);}

  double getCurrIndexTime() const;
  double getCurrIndexCPA() const;
  double getCurrIndexEFF() const;
  int    getCurrIndexID() const;
  std::string getCurrIndexContact() const;

  
  unsigned getTotalEncounters() const;
  unsigned getCurrEncounter() const {return(m_curr_buff_size);}
  
 protected: // Utility functions
  void   handleLeftMouse(int, int);
  void   handleRightMouse(int, int);
  void   refreshDrawBuffers();
  void   setCurrBuffIndex(double, double);
  
 private:
  double          m_curr_time;
  ALogDataBroker  m_dbroker;
  GUI_Encounters *m_owning_gui;
  
  // Vehicle name stays constant once it is set initially
  std::string    m_vname; 

  bool   m_draw_mineff;
  bool   m_draw_avgeff;
  bool   m_draw_mincpa;
  bool   m_draw_avgcpa;
  bool   m_show_allpts;

  double m_collision_range;
  double m_near_miss_range;
  double m_encounter_range;
  
  double m_min_cpa;
  double m_min_eff;
  double m_avg_cpa;
  double m_avg_eff;

  std::vector<double> m_buff_cpa_pix;
  std::vector<double> m_buff_eff_pix;
  unsigned int   m_curr_buff_ix;
  unsigned int   m_curr_buff_size;
  
  unsigned int   m_draw_pointsize;
  
  EncounterPlot  m_encounter_plot;
  ColorPack      m_clear_color;
  ColorPack      m_label_color;
  ColorPack      m_mineff_color;
  ColorPack      m_avgeff_color;  
  ColorPack      m_mincpa_color;
  ColorPack      m_avgcpa_color;  
};

#endif





