/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Utterance.h                                          */
/*    DATE: May 20th, 2013                                       */
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

#ifndef UTTERANCE_HEADER
#define UTTERANCE_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class Utterance
{
 public:
  Utterance(std::string);
  Utterance();
  ~Utterance() {}
   
  void init();
  bool initFromString(std::string);

  void setText(std::string s)    {m_text=s;}
  void setFile(std::string s)    {m_file=s;}
  void setRate(double v)         {m_rate=v;}
  void setPriority(double v)     {m_priority=v;}
  void setTimeReceived(double v) {m_time_received=v;}
  void setTimePosted(double v)   {m_time_posted=v;}
  void setSource(std::string s)  {m_source=s;}
  void setNode(std::string s)    {m_node=s;}
  bool setVoice(std::string);
  
  std::string getText() const      {return(m_text);}
  std::string getFile() const      {return(m_file);}
  std::string getVoice() const     {return(m_voice);}
  std::string getSource() const    {return(m_source);}
  std::string getNode() const      {return(m_node);}

  double      getRate() const         {return(m_rate);}
  double      getPriority() const     {return(m_priority);}
  double      getTimeReceived() const {return(m_time_received);}
  double      getTimePosted() const   {return(m_time_posted);}
  
  bool        isTopPriority() const   {return(m_top_priority);}
  bool        isEmpty() const         {return(m_text == "");}
  
  std::string getSpec() const;
  std::string getType() const;

 private:
  std::string m_text;
  std::string m_voice;
  std::string m_file;
  std::string m_source;
  std::string m_node;
  double      m_rate;
  double      m_priority;
  double      m_time_received;
  double      m_time_posted;
  bool        m_top_priority;
};

// Overload the < operator 
bool operator< (const Utterance& utter1, const Utterance& utter2);
bool operator> (const Utterance& utter1, const Utterance& utter2);

#endif 





