/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LoadWatch.h                                          */
/*    DATE: Dec 24th, 2013                                       */
/*****************************************************************/

#ifndef U_LOAD_WATCH_HEADER
#define U_LOAD_WATCH_HEADER

#include <string>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class LoadWatch : public AppCastingMOOSApp
{
 public:
   LoadWatch();
   ~LoadWatch() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();
   
 protected:
   void registerVariables();

   void handleMailIterGap(std::string, double);
   void handleMailIterLen(std::string, double);
   bool handleConfigThresh(std::string);


 private: // Configuration variables
   std::map<std::string, double> m_map_thresh;

 private: // State variables
   std::map<std::string, double>       m_map_app_gap_total;
   std::map<std::string, double>       m_map_app_gap_max;
   std::map<std::string, unsigned int> m_map_app_gap_count;

   std::map<std::string, double>       m_map_app_len_total;
   std::map<std::string, double>       m_map_app_len_max;
   std::map<std::string, unsigned int> m_map_app_len_count;
};

#endif 
