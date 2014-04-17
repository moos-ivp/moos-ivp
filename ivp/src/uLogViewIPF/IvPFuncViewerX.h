/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
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
#include "IPF_TPlot.h"
#include "IterTPlot.h"
#include "ULV_TimeStamp.h"
#include "PostingBuffer.h"

class IvPFuncViewerX : public Common_IPFViewer
{
 public:
  IvPFuncViewerX(int x,int y,int w,int h,const char *l=0);
  virtual ~IvPFuncViewerX() {};

  void   draw();
  int    handle(int event);

  void   addIPF_Plot(const IPF_TPlot&);
  void   setIterPlot(const IterTPlot& plot) {m_iter_plot=plot;};

  void   setMinMaxTimeUTC(double, double);
  void   setTimeFormat(const std::string&);

  void   incrementIter(int amt);
  void   setTime(const ULV_TimeStamp& tstamp);
  void   setTime(double tstamp);
  void   setSource(std::string src);

  std::string getCurrPieces() const;
  std::string getCurrPriority() const;
  std::string getCurrDomain() const;
  std::string getCurrIteration() const;

  double      getCurrTime() const;

  PostingBuffer getPostingBuffer(bool clear=true);

 protected:
  void   updateIPF();
  bool   buildCollectiveIPF(std::string ctype);
  void   buildIndividualIPF(std::string source="");

  void   setIterationFromTimeUTC(double);
  void   setTimeUTCFromIteration(unsigned int);

private:
  double        m_curr_time;
  unsigned int  m_curr_iter;

  double        m_min_time_utc;
  double        m_max_time_utc;
  bool          m_time_format_utc;

  PostingBuffer m_pbuffer;

  // Vehicle name stays constant once it is set initially
  std::string m_vname; 

  // Source may change as user selects different options
  std::string m_source;

  // Map from a behavior (source) to an IPF_TPlot
  std::map<std::string, IPF_TPlot> m_map_ipf_plots;

  // Map from timestamp to helm iteration
  IterTPlot m_iter_plot;
};

#endif





