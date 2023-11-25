/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
  void   setShowSeparator(bool v)   {m_show_separator=v;}
  void   setTruncateVal(bool v)     {m_truncate=v;}
  void   setWrapVal(bool v)         {m_wrap=v;}
  void   setFutureVal(bool v)       {m_future=v;}
  void   setGrepStr1(std::string s) {m_grep1=s;}
  void   setGrepStr2(std::string s) {m_grep2=s;}
  void   setGrepApply1(bool v)      {m_grep_apply1=v;}
  void   setGrepApply2(bool v)      {m_grep_apply2=v;}

  // Getters
  double      getCurrTime() const      {return(m_curr_time);}
  bool        getShowSeparator() const {return(m_show_separator);}
  bool        getTruncateVal() const   {return(m_truncate);}
  bool        getWrapVal() const       {return(m_wrap);}
  bool        getFutureVal() const     {return(m_future);}
  std::string getGrepStr1() const      {return(m_grep1);}
  std::string getGrepStr2() const      {return(m_grep2);}
  bool        getGrepApply1() const    {return(m_grep_apply1);}
  bool        getGrepApply2() const    {return(m_grep_apply2);}

  std::vector<std::string>  getLinesUpToNow();
  std::vector<std::string>  getLinesPastNow() const;

protected:
  std::vector<std::string>  processLines(std::vector<AppLogEntry>,
					 unsigned int start_ctr=0) const;
  
  unsigned int m_entries_upto_now_cnt;
  
private:
  double      m_curr_time;
  AppLogPlot  m_alplot;

  bool        m_show_separator;
  bool        m_truncate;
  bool        m_wrap;
  bool        m_future;
  bool        m_grep_apply1;
  bool        m_grep_apply2;
  
  std::string m_grep1;
  std::string m_grep2;
  
  
  // Vehicle name and app_name stay constant once it is set initially
  std::string m_vname; 
  std::string m_app_name;   
};

#endif


