/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IterBlockHelm.h                                      */
/*    DATE: Apr 12th 2008                                        */
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

#ifndef ITER_INFO_HEADER
#define ITER_INFO_HEADER

#include <vector>
#include <string>
#include <map>
#include "BehaviorRecord.h"

class IterBlockHelm
{
public:
  IterBlockHelm();
  virtual ~IterBlockHelm() {}
  
  void initialize(const IterBlockHelm&);
  void setActiveBHVs(const std::string&);
  void setRunningBHVs(const std::string&);
  void setIdleBHVs(const std::string&);
  void setCompletedBHVs(const std::string&);
  void addDecVarVal(const std::string&, const std::string&);
  void setModeString(std::string v);

  void setIteration(int v)     {m_iteration=v;}
  void setCountIPF(int v)      {m_count_ipf=v;}
  void setSolveTime(double v)  {m_solve_time=v;}
  void setCreateTime(double v) {m_create_time=v;}
  void setLoopTime(double v)   {m_loop_time=v;}
  void setHalted(bool v)       {m_halted=v;}

  void setUTCTime(double v)    {m_utc_time=v;}
  void setWarnings(int v)      {m_warning_cnt=v;}

  std::vector<std::string> getActiveBHV(double utc) const;
  std::vector<std::string> getRunningBHV(double utc) const;
  std::vector<std::string> getIdleBHV(double utc, bool=false) const;
  std::vector<std::string> getCompletedBHV(double utc, bool=false) const;

  std::string  getDecVar(unsigned int) const;
  std::string  getDecVal(unsigned int) const;
  bool         getDecChg(unsigned int) const;
  std::string  getModeStr() const    {return(m_modes);}
  double       getSolveTime() const  {return(m_solve_time);}
  double       getCreateTime() const {return(m_create_time);}
  double       getLoopTime() const   {return(m_loop_time);}
  double       getUTCTime() const    {return(m_utc_time);}
  bool         getHalted() const     {return(m_halted);}

  unsigned int getDecVarCnt()  const {return(m_decvar.size());}
  unsigned int getIteration() const  {return(m_iteration);}
  unsigned int getCountIPF() const   {return(m_count_ipf);}
  unsigned int getWarnings() const   {return(m_warning_cnt);}

  unsigned int getCountIdle() const {return(m_idle_bhv.size());}
  
  void print(int iter=0) const;

protected:
  std::vector<BehaviorRecord> m_active_bhv;
  std::vector<BehaviorRecord> m_running_bhv;
  std::vector<BehaviorRecord> m_idle_bhv;
  std::vector<BehaviorRecord> m_completed_bhv;

  // Below three share common index - one for each dec variable
  std::vector<std::string> m_decvar;
  std::vector<std::string> m_decval;
  std::vector<bool>        m_decchg;

  std::string  m_modes;

  unsigned int m_iteration;
  double       m_utc_time;
  double       m_solve_time;
  double       m_create_time;
  double       m_loop_time;
  bool         m_halted;
  
  unsigned int m_count_ipf;
  unsigned int m_warning_cnt;
  unsigned int m_error_cnt;
  unsigned int m_posting_cnt;
  unsigned int m_infomsg_cnt;
};

#endif 










