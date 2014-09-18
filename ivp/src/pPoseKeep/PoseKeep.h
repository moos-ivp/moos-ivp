/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: PoseKeep.h                                           */
/*   DATE: Sep 15th 2014                                        */
/****************************************************************/

#ifndef P_POSE_KEEP_HEADER
#define P_POSE_KEEP_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "VarDataPair.h"

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
   bool handleMailFlag(std::string flag_type, std::string flag);

   bool handleConfigHoldTolerance(std::string);
   bool handleConfigHoldDuration(std::string);
   bool handleConfigHoldHeading(std::string);
   bool handleConfigFlag(std::string flag_type, std::string flag);

   bool addFlag(std::string flag_type, std::string flag);

 protected:
   void registerVariables();

   bool postFlags(std::string flagtype);

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

   std::string m_hold_source;

   double  m_start_time;
   
   double  m_osx; 
   double  m_osy;
   bool    m_osx_set;
   bool    m_osy_set;
   
   bool    m_hold_heading_set;
   double  m_curr_heading_tstamp;    

   std::list<std::string> m_summaries;

   std::vector<VarDataPair> m_active_flags;
   std::vector<VarDataPair> m_inactive_flags;
   std::vector<VarDataPair> m_end_flags;
};

#endif 
