/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ModelAppLogScope.h                                   */
/*    DATE: Oct 16th 2021                                        */
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

#ifndef APPLOG_SCOPE_MODEL_HEADER
#define APPLOG_SCOPE_MODEL_HEADER

#include <vector>
#include <string>
#include "AppLogPlot.h"

class ModelAppLogScope
{
 public:
  ModelAppLogScope();
  virtual ~ModelAppLogScope() {}

  // Setters
  void   setTime(double tstamp);
  void   setAppLogPlot(const AppLogPlot&);

  // Getters
  double        getCurrTime() const {return(m_curr_time);}
  //unsigned int  getAppLogPlotSize(std::string) const;

  std::vector<std::string>  getPrevLines() const;
  std::vector<std::string>  getFutureLines() const;

  std::vector<std::string>  getNowLines() const;
 
private:
  double      m_curr_time;
  AppLogPlot  m_alplot;
  
  // Vehicle name and app_name stay constant once it is set initially
  std::string m_vname; 
  std::string m_app_name;   
};

#endif




