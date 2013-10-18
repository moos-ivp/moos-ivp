/************************************************************/
/*    NAME: Mike Benjamin / Kyle Woerner                    */
/*    ORGN: MIT                                             */
/*    FILE: CollisionDetector.h                             */
/*    DATE: 21 May 2013                                     */
/************************************************************/

#ifndef CollisionDetector_HEADER
#define CollisionDetector_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <map>
#include "NodeRecord.h"


class CollisionDetector : public AppCastingMOOSApp
{
 public:
   CollisionDetector();
   ~CollisionDetector() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

   struct col_data {
     bool colliding;
     double min_dist;
     double detect_dist;  // distance of collision detection
     double display_clear_time; // time after no long colliding to display in appcasting
     bool posted; // true only if collision clear was posted but still being displayed on appcast
   };

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   std::map <std::string,NodeRecord> m_moos_map;
   std::map <std::pair<std::string,std::string>,col_data> m_col_bools;

   bool MakeRangePulse(double, double);

   double m_col_dist;
   double m_delay_time_to_clear;
   bool m_post_immediately;
   std::string m_post_string;
   bool m_pulse_bool;
   double m_pulse_duration;
   double m_pulse_range;

   bool m_publish_single;
   bool m_publish_pairs;

   double m_start_checking_time;  // time to start checking for collisions
   double m_deploy_delay;// time delay from deploy until start of checking
   bool m_check_collisions;
 private: // Configuration variables
 
 private: // State variables
   double m_total_collisions;
};

#endif 
