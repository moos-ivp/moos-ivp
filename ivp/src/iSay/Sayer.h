/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Sayer.h                                              */
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

#ifndef IVOICE_HEADER
#define IVOICE_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "Utterance.h"
#include "UtterQueue.h"

class Sayer : public AppCastingMOOSApp
{
 public:
  Sayer();
   ~Sayer() {}
   
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
   
 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
 protected: // Utilities 
  bool handleSetVolume(std::string);
  bool addUtterance(std::string, std::string);
  bool sayUtterance();

 protected:
  void registerVariables();
   
 private: // Configuration variables
  double        m_default_rate;
  std::string   m_default_voice;
  std::string   m_interval_policy;
  unsigned int  m_max_utter_queue_size;
  double        m_min_utter_interval;

  double        m_volume;
  
  std::string   m_os_mode;

  std::vector<std::string> m_audio_dirs;
   

 private: // State variables
  UtterQueue           m_utter_pqueue;
  
  std::list<Utterance> m_utter_history;
  
  double       m_last_utter_time;
  std::string  m_isay_filter;
  
  unsigned int m_unhandled_audios;
  
  unsigned int m_total_received;
};

#endif 






