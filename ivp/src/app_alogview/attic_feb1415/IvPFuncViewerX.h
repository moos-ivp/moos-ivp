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
#include "ALogDataBroker.h"
#include "IPF_Plot.h"
#include "LogPlot.h"

class IvPFuncViewerX : public Common_IPFViewer
{
 public:
  IvPFuncViewerX(int x,int y,int w,int h,const char *l=0);
  virtual ~IvPFuncViewerX() {};

  void   draw();
  int    handle(int event);
  void   setDataBroker(ALogDataBroker, std::string vname);

  void   setIPF_Plots(std::vector<std::string>);
  void   setHelmIterPlot();
  void   setBix(unsigned int bix);

  void   setTime(double tstamp);
  void   setVName(std::string s) {m_vname=s;};
  void   setSource(std::string src);
  double getCurrTime() const;

  std::string getCurrPieces() const;
  std::string getCurrPriority() const;
  std::string getCurrDomain() const;
  std::string getCurrIteration() const;


 protected:
  void   updateIPF();
  void   addIPF_Plot(const IPF_Plot&);
  bool   buildCollectiveIPF(std::string ctype);
  bool   buildIndividualIPF(std::string source="");

private:
  double         m_curr_time;
  unsigned int   m_curr_iter;
  ALogDataBroker m_dbroker;

  // Vehicle name stays constant once it is set initially
  std::string m_vname; 

  // Source may change as user selects different options
  std::string m_source;

  LogPlot     m_iter_plot;

  // Map from a behavior (source) to an IPF_TPlot
  std::map<std::string, IPF_Plot> m_map_ipf_plots;
};

#endif





