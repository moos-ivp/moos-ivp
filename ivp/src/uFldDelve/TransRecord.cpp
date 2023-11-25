/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TransRecord.cpp                                      */
/*    DATE: Sep 2nd, 2022                                        */
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

