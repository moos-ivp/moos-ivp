/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IterBlockBHV.h                                       */
/*    DATE: Apr 12th 2008                                        */
/*****************************************************************/

#ifndef ITER_BLOCK_BHV_HEADER
#define ITER_BLOCK_BHV_HEADER

#include <vector>
#include <string>

class IterBlockBHV
{
public:
  IterBlockBHV() {};
  virtual ~IterBlockBHV() {};
  
  void setState(char c) {m_state=c;};
  void setFailConditions(std::string str) {m_fail_conditions=str;};

  void setPieces(int v)       {m_pieces=v;};
  void setPriority(double v)  {m_priority=v;};
  void setTimeCPU(int v)      {m_cpu_time=v;};
  void setWosrstErr(double v) {m_worst_error=v;};
  void setAvgErr(double v)    {m_avg_error=v;};
  void setErrSamples(int v)   {m_err_samples=v;};

  void setBuildInfo(std::string str)  {m_build_info=str;};
  void setBuildPiece(std::string str) {m_build_piece=str;};

  void addErrorMsg(std::string);
  void addWarningMsg(std::string);
  void addPostingMsg(std::string);
  void addInfoMsg(std::string);

  unsigned int getErrorMsgCnt()    {return(m_error_msgs.size());};
  unsigned int getWarningMsgCnt()  {return(m_warning_msgs.size());};
  unsigned int getPostingMsgCnt()  {return(m_posting_msgs.size());};
  unsigned int getInfoMsgCnt()     {return(m_info_msgs.size());};

  int     getPieces()          {return(m_pieces);};
  double  getTimeCPU()         {return(m_cpu_time);};
  double  getWorstErr()        {return(m_worst_error);};
  double  getAvgErr()          {return(m_avg_error);};
  int     getErrSamples()      {return(m_err_samples);};

  std::string getErrorMsg(int);
  std::string getWarningMsg(int);
  std::string getPostingMsg(int);
  std::string getInfoMsg(int);



protected:
  char  m_state;

  std::string m_fail_conditions;
  
  int    m_pieces;
  double m_priority;
  double m_cpu_time;

  // IvPBuild Toolbox feedback
  double m_worst_error;
  double m_avg_error;
  int    m_err_samples;
  std::string m_build_info;
  std::string m_build_piece;
  
  std::vector<std::string> m_error_msgs;
  std::vector<std::string> m_warning_msgs;
  std::vector<std::string> m_posting_msgs;
  std::vector<std::string> m_info_msgs;  
};


#endif 






