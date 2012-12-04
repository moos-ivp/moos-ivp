/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmScopeModel.h                                     */
/*    DATE: Oct0411                                              */
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

#ifndef HELM_SCOPE_MODEL_HEADER
#define HELM_SCOPE_MODEL_HEADER

#include <vector>
#include <string>
#include "HelmTPlot.h"
#include "VarTPlot.h"
#include "VarTPlotSet.h"
#include "ULV_TimeStamp.h"
#include "PostingBuffer.h"

class HelmScopeModel
{
 public:
  HelmScopeModel();
  virtual ~HelmScopeModel() {};

  // Setters
  void   setHelmPlot(const HelmTPlot&);
  void   addVarPlot(const VarTPlot&);
  void   setTimeFormat(const std::string&);
  void   incrementIter(int amt);
  void   setTime(const ULV_TimeStamp& tstamp);
  void   setTime(double tstamp);
  void   toggleHeadersBHV() {m_headers_bhv = !m_headers_bhv;};

  // Getters
  double        getCurrTime() const;
  double        getCurrTimeUTC() const;
  PostingBuffer getPostingBuffer(bool clear=true);
  bool          getHeadersBHV() const {return(m_headers_bhv);};
  
  std::string   getCurrMode() const;
  std::string   getCurrIter() const;
  std::string   getCurrDecision() const;

  std::string   getCurrEngaged() const;

  std::vector<std::string>  getActiveList() const;
  std::vector<std::string>  getNonActiveList(std::string) const;

  std::vector<std::string>  getWarningsList();


 protected:
  void   setIterationFromTimeUTC(double);
  void   setTimeUTCFromIteration(unsigned int);

  std::vector<std::string> parseVariables(std::string) const;
  std::vector<std::string> parseBehaviors(std::string) const;

  std::string convertTimeUTC2TimeElapsed(const std::string&) const;

private:
  double        m_curr_time;
  unsigned int  m_curr_iter;
  PostingBuffer m_pbuffer;
  bool          m_headers_bhv;

  // Plot of timestamp to helm summary
  HelmTPlot     m_helm_plot;
  VarTPlotSet   m_vplotset;
  VarTPlot      m_vplot_engaged;
};

#endif


