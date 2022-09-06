/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: TransRecord.cpp                                 */
/*    DATE: Sep 2nd, 2022                                   */
/************************************************************/

#include "TransRecord.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

TransRecord::TransRecord(string varname)
{
  m_varname = varname;

  m_curr_utc = 0;
  
  m_msgs  = 0;
  m_chars = 0;

  m_msg_rate  = 0;
  m_char_rate = 0;

  m_rate_frame = 10;
}

//---------------------------------------------------------
// Procedure: setCurrTime()

void TransRecord::setCurrTime(double curr_utc)
{
  m_curr_utc = curr_utc;
  pruneOldMsgHist();
}

//---------------------------------------------------------
// Procedure: setRateFrame()

bool TransRecord::setRateFrame(double rate_frame)
{
  if(rate_frame <= 0)
    return(false);

  m_rate_frame = rate_frame;

  return(true);	   
}

//---------------------------------------------------------
// Procedure: addMsg()

void TransRecord::addMsg(double utc, string src, unsigned int len)
{
  m_msgs++;
  m_chars += len;
  m_latest_src = src;

  if(m_curr_utc == 0)
    return;
  
  m_msg_utc.push_front(utc);
  m_msg_len.push_front(len);

  bool done=false;
  while(!done) {
    if(m_msg_utc.size() <= 1)
      done = true;
    else {
      double oldest_utc = m_msg_utc.back();
      if((m_curr_utc - oldest_utc) > m_rate_frame) {
	m_msg_utc.pop_back();
	m_msg_len.pop_back();
      }
      else
	done = true;
    }
  }
}

//---------------------------------------------------------
// Procedure: pruneOldMsgHist()

void TransRecord::pruneOldMsgHist()
{
  if(m_curr_utc == 0)
    return;
  
  bool done=false;
  while(!done) {
    if(m_msg_utc.size() == 0)
      done = true;
    else {
      double oldest_utc = m_msg_utc.back();
      if((m_curr_utc - oldest_utc) > m_rate_frame) {
	m_msg_utc.pop_back();
	m_msg_len.pop_back();
      }
      else
	done = true;
    }
  }
}

//---------------------------------------------------------
// Procedure: getMsgRate()

double TransRecord::getMsgRate() const
{
  double msgs = (double)(m_msg_utc.size());
  double rate = msgs / m_rate_frame;

  return(rate);
}

//---------------------------------------------------------
// Procedure: getCharRate()

double TransRecord::getCharRate() const
{
  unsigned int total_chars = 0;

  list<unsigned int>::const_iterator p;
  for(p=m_msg_len.begin(); p!=m_msg_len.end(); p++)
    total_chars += *p;

  double rate = ((double)(total_chars)) / m_rate_frame;

  return(rate);
}
