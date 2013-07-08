/************************************************************/
/*  NAME: Mike Benjamin                                     */
/*  ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA  */
/*  FILE: Sayer.h                                           */
/*  DATE: May 20th, 2013                                    */
/************************************************************/

#ifndef IVOICE_HEADER
#define IVOICE_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "Utterance.h"
#include "UtterQueue.h"

class Sayer : public AppCastingMOOSApp
{
 public:
  Sayer();
   ~Sayer() {};
   
 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   
 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();
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
   std::string   m_os_mode;

   std::vector<std::string> m_audio_dirs;
   

 private: // State variables
   UtterQueue           m_utter_pqueue;

   std::list<Utterance> m_utter_history;
   
   double       m_last_utter_time;
   std::string  m_isay_filter;

   unsigned int m_unhandled_audios;
};

#endif 
