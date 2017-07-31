/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPFuncViewerX.h                                     */
/*    DATE: Feb 24, 2007                                         */
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

#ifndef IVPFUNC_VIEWER_HEADER
#define IVPFUNC_VIEWER_HEADER

#include <map>
#include <vector>
#include <string>
#include "Common_IPFViewer.h"
#include "ALogDataBroker.h"
#include "IPF_Plot.h"
#include "LogPlot.h"
#include "QuadSet1D.h"

class IvPFuncViewerX : public Common_IPFViewer
{
 public:
  IvPFuncViewerX(int x,int y,int w,int h,const char *l=0);
  virtual ~IvPFuncViewerX() {}

  void   draw();
  int    handle(int event);

 public: // Setters
  void   setDataBroker(ALogDataBroker, std::string vname);
  void   setIPF_Plots(std::vector<std::string> bhv_names);
  void   setHelmIterPlot();
  void   setBix(unsigned int bix);
  void   setVarPlotA(unsigned int mix, std::string bname="");
  void   setVarPlotB(unsigned int mix, std::string bname="");
  void   setVarPlotA(std::string src, unsigned int mix);
  void   setVarPlotB(std::string src, unsigned int mix);
  void   clearVarPlotA();
  void   clearVarPlotB();

  void   setPiecesIPF(std::string s)   {m_active_ipf_pieces = s;}
  void   setPriorityIPF(std::string s) {m_active_ipf_priority = s;}
  void   setSubDomainIPF(IvPDomain v)  {m_active_ipf_subdomain = v;} 
  void   setIterIPF(std::string s)     {m_active_ipf_iter = s;}
  void   setLabelColor(std::string s)  {m_label_color.setColor(s);}
  
  void   setTime(double tstamp);
  void   setVName(std::string s)       {m_vname=s;}
  void   setSource(std::string src);
  void   setMutableTextSize(int v)     {m_mutable_text_size=v;}

 public: // Getters
  double getCurrTime() const;

  std::string getCurrPieces()    const;
  std::string getCurrPriority()  const;
  std::string getCurrDomain()    const;
  std::string getCurrIteration() const;
  std::string getCurrPriority(std::string);
  std::string getCurrScopeVarA();
  std::string getCurrScopeVarB();

  void   updateIPF();

 protected:
  void   updateScope();
  void   addIPF_Plot(const IPF_Plot&);
  bool   buildCollectiveIPF(std::string ctype);
  bool   buildIndividualIPF(std::string source="");

private:
  //QuadSet1D      m_quadset_1d;
  bool           m_showing_1d;
  
  double         m_curr_time;
  unsigned int   m_curr_iter;
  double         m_curr_heading;
  ALogDataBroker m_dbroker;
  int            m_mutable_text_size;

  // Vehicle name stays constant once it is set initially
  std::string m_vname; 

  // Source is either a behavior name or "collective"
  std::string m_source;

  LogPlot     m_iter_plot;
  LogPlot     m_hdg_plot;
  
  std::string m_scope_a;
  std::string m_scope_b;

  // Map from a behavior (source) to: IPF_TPlot, scopvars
  std::map<std::string, IPF_Plot> m_map_ipf_plots;
  std::map<std::string, VarPlot>  m_map_scope_var_a;
  std::map<std::string, VarPlot>  m_map_scope_var_b;

  // Information available for the GUI output fields
  std::string  m_active_ipf_pieces;
  std::string  m_active_ipf_priority;
  std::string  m_active_ipf_iter;
  IvPDomain    m_active_ipf_subdomain;

  ColorPack    m_label_color;
  
};

#endif




