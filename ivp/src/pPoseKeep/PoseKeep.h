/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: PoseKeep.h                                           */
/*   DATE: Sep 15th 2014                                        */
/****************************************************************/

#ifndef P_POSE_KEEP_HEADER
#define P_POSE_KEEP_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class PoseKeep : public AppCastingMOOSApp
{
 public:
   PoseKeep();
   ~PoseKeep() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();
   void adjustHeading();
   void rotateVehicle(double);
   void checkForTimeOut();

   bool handleMailHoldPoint(std::string);
   bool handleConfigHoldTolerance(std::string);
   bool handleConfigHoldDuration(std::string);

 protected:
   void registerVariables();

 private: // Configuration variables
   double  m_tolerance;
   double  m_hold_heading;
   double  m_duration;

 private: // State variables
   bool    m_active;
   double  m_curr_heading;

   double  m_thrust_l;
   double  m_thrust_r;
   double  m_heading_diff;
   double  m_adjustment;

   double  m_hold_x;
   double  m_hold_y;

   double  m_start_time;
   
   double  m_osx; 
   double  m_osy;
   bool    m_osx_set;
   bool    m_osy_set;
   
   bool    m_hold_heading_set;
   double  m_curr_heading_tstamp;    

   std::list<std::string> m_summaries;
};

#endif 
