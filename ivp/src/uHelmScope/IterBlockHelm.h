/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IterBlockHelm.h                                      */
/*    DATE: Apr 12th 2008                                        */
/*****************************************************************/

#ifndef ITER_INFO_HEADER
#define ITER_INFO_HEADER

#include <vector>
#include <string>
#include "IterBlockBHV.h"

class IterBlockHelm
{
public:
  IterBlockHelm();
  virtual ~IterBlockHelm() {};
  
  void addActiveBHVs(const std::string&);
  void addRunningBHVs(const std::string&);
  void addIdleBHVs(const std::string&);
  void addCompletedBHVs(const std::string&);
  void addDecVarVal(const std::string&, const std::string&);

  void setIteration(int v)     {m_iteration=v;};
  void setCountIPF(int v)      {m_count_ipf=v;};
  void setSolveTime(double v)  {m_solve_time=v;};
  void setCreateTime(double v) {m_create_time=v;};
  void setLoopTime(double v)   {m_loop_time=v;};
  void setUTCTime(double v)    {m_utc_time=v;};
  void setHalted(bool v)       {m_halted=v;};
  void setWarnings(int v)      {m_warning_cnt=v;};

  std::vector<std::string> getActiveBHV() const;
  std::vector<std::string> getRunningBHV() const;
  std::vector<std::string> getIdleBHV(bool=false) const;
  std::vector<std::string> getCompletedBHV(bool=false) const;

  int         getDecVarCnt()  const {return(m_decvar.size());};
  std::string getDecVar(unsigned int) const;
  std::string getDecVal(unsigned int) const;
  int         getIteration() const  {return(m_iteration);};
  int         getCountIPF() const   {return(m_count_ipf);};
  double      getSolveTime() const  {return(m_solve_time);};
  double      getCreateTime() const {return(m_create_time);};
  double      getLoopTime() const   {return(m_loop_time);};
  double      getUTCTime() const    {return(m_utc_time);};
  bool        getHalted() const     {return(m_halted);};
  int         getWarnings() const   {return(m_warning_cnt);};
  
protected:
  std::vector<std::string> m_active_bhv;
  std::vector<std::string> m_running_bhv;
  std::vector<std::string> m_idle_bhv;
  std::vector<std::string> m_completed_bhv;

  std::vector<std::string> m_decvar;
  std::vector<std::string> m_decval;

  int    m_iteration;
  int    m_count_ipf;
  double m_solve_time;
  double m_create_time;
  double m_loop_time;
  double m_utc_time;
  bool   m_halted;
  
  int    m_warning_cnt;
  int    m_error_cnt;
  int    m_posting_cnt;
  int    m_infomsg_cnt;

  std::vector<std::string>                m_behaviors;
  std::vector<std::vector<IterBlockBHV> > m_blocks_bhv;
};


#endif 






