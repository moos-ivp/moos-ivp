/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ModelHelmScope.h                                     */
/*    DATE: Oct0411                                              */
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

#ifndef HELM_SCOPE_MODEL_HEADER
#define HELM_SCOPE_MODEL_HEADER

#include <vector>
#include <string>
#include "HelmPlot.h"
#include "VarPlot.h"
#include "ALogDataBroker.h"

class ModelHelmScope
{
 public:
  ModelHelmScope();
  virtual ~ModelHelmScope() {}

  // Setters
  void   setDataBroker(ALogDataBroker, std::string vname);
  void   incrementIter(int amt);
  void   setTime(double tstamp);
  void   toggleHeadersBHV() {m_headers_bhv = !m_headers_bhv;}

  // Getters
  double        getCurrTime() const {return(m_curr_time);}
  bool          getHeadersBHV() const {return(m_headers_bhv);}
  unsigned int  getVPlotSize(std::string) const;
  
  std::string   getCurrMode() const;
  std::string   getCurrIter() const;
  std::string   getCurrDecision() const;
  std::string   getCurrHelmState() const;

  std::vector<std::string>  getActiveList() const;
  std::vector<std::string>  getNonActiveList(std::string) const;

  std::vector<std::string>  getWarnings() const;
  std::vector<std::string>  getErrors() const;
  std::vector<std::string>  getModes() const;
  std::vector<std::string>  getLifeEvents() const;

 protected:
  std::vector<std::string>  getErrWarnings(const VarPlot& vplot) const;

  std::string convertTimeUTC2TimeElapsed(std::string) const;

  void   setIterFromTime(double);
  void   setTimeFromIter(unsigned int);

  std::string formatHelmString(std::string src, std::string val) const; 
  
private:
  double         m_curr_time;
  double         m_utc_start;
  unsigned int   m_curr_iter;
  ALogDataBroker m_dbroker;

  // Vehicle name stays constant once it is set initially
  std::string m_vname; 

  bool         m_headers_bhv;

  // Plot of timestamp to helm summary
  HelmPlot     m_helm_plot;
  VarPlot      m_vplot_bhv_error;
  VarPlot      m_vplot_bhv_warning;
  VarPlot      m_vplot_helm_state;
  VarPlot      m_vplot_helm_mode;
  VarPlot      m_vplot_helm_modeset;
  VarPlot      m_vplot_life_event;
};

#endif





