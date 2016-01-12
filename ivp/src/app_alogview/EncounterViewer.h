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

#include <map>
#include <vector>
#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "Common_IPFViewer.h"
#include "ALogDataBroker.h"
#include "IPF_Plot.h"
#include "LogPlot.h"
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
  void   setIPF_Plots(std::vector<std::string> bhv_names);
  void   setHelmIterPlot();
  void   setBix(unsigned int bix);
  void   setVarPlotA(unsigned int mix);
  void   setVarPlotB(unsigned int mix);
  void   clearVarPlotA();
  void   clearVarPlotB();
  void   setClearColor(std::string s)  {m_clear_color.setColor(s);}

  void   setTime(double tstamp);
  void   setVName(std::string s) {m_vname=s;}
  void   setSource(std::string src);
  void   setMutableTextSize(int v) {m_mutable_text_size=v;}

 public: // Getters
  double getCurrTime() const;

  std::string getCurrPieces() const;
  std::string getCurrPriority() const;
  std::string getCurrDomain() const;
  std::string getCurrIteration() const;
  std::string getCurrPriority(std::string);

 protected:
  void   updateIPF();
  void   updateScope();
  void   addIPF_Plot(const IPF_Plot&);
  bool   buildCollectiveIPF(std::string ctype);
  bool   buildIndividualIPF(std::string source="");

private:
  double         m_curr_time;
  unsigned int   m_curr_iter;
  ALogDataBroker m_dbroker;
  int            m_mutable_text_size;

  // Vehicle name stays constant once it is set initially
  std::string m_vname; 

  // Source may change as user selects different options
  std::string m_source;

  LogPlot     m_iter_plot;
  VarPlot     m_var_plot_a;
  VarPlot     m_var_plot_b;

  std::string m_scope_a;
  std::string m_scope_b;

  // Map from a behavior (source) to: IPF_TPlot, scopvars
  std::map<std::string, IPF_Plot> m_map_ipf_plots;
  std::map<std::string, VarPlot>  m_map_scope_var_a;
  std::map<std::string, VarPlot>  m_map_scope_var_b;


  ColorPack    m_clear_color;
  ColorPack    m_frame_color;
  ColorPack    m_label_color;

};

#endif
