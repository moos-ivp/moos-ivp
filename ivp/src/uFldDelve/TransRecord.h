/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TransRecord.h                                        */
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

