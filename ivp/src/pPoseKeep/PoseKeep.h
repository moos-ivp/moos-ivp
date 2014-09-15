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
   
   bool handleConfigHoldTolerance(std::string);

 protected:
   void registerVariables();

 private: // Configuration variables

 private: // State variables

   bool    m_active;

   double  m_curr_heading;
   double  m_hold_heading;

   double  m_thrust_l;
   double  m_thrust_r;
   double  m_heading_diff;
   double  m_adjustment;
   double  m_tolerance;

   bool    m_hold_heading_set;
   double  m_curr_heading_tstamp;    
};

#endif 
