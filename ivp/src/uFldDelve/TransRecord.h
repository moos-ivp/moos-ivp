/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: TransRecord.h                                   */
/*    DATE: Sep 2nd, 2022                                   */
/************************************************************/

#ifndef TRANS_RECORD_HEADER
#define TRANS_RECORD_HEADER

#include <string>
#include <list>

class TransRecord
{
 public:
  TransRecord(std::string varname="");
  ~TransRecord() {};

  void setCurrTime(double utc);
  bool setRateFrame(double);
  void addMsg(double utc, std::string src, unsigned int len=4);
  
  double getMsgCount() const  {return(m_msgs);}
  double getCharCount() const {return(m_chars);}

  double getMsgRate() const; 
  double getCharRate() const; 

  std::string getLatestSource() const {return(m_latest_src);}
  std::string getVarName() const {return(m_varname);}

protected:
  void pruneOldMsgHist();
  
 private: // Configuration variables

  double m_rate_frame;
  
 private: // State variables
  std::string m_varname;
  std::string m_latest_src;

  double  m_curr_utc;
  
  double  m_msgs;
  double  m_chars;

  double  m_msg_rate;
  double  m_char_rate;

  std::list<double>       m_msg_utc;
  std::list<unsigned int> m_msg_len;
};

#endif 
