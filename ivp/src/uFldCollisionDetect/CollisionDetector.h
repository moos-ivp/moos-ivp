/*****************************************************************/
/*    NAME: Kyle Woerner, Michael Benjamin                       */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CollisionDetector.h                                  */
/*    DATE: 21 May 2013                                          */
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

#ifndef CollisionDetector_HEADER
#define CollisionDetector_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

#include <iterator>
#include <math.h>
#include <map>
#include "MBUtils.h"
#include "ACTable.h"
#include "NodeRecordUtils.h"
#include "XYRangePulse.h"
#include "CPAEngine.h"
#include "NodeRecord.h"
#include "CollisionDetector.h"
#include "CollisionRecord.h"

class CollisionDetector : public AppCastingMOOSApp
{
 public:
   CollisionDetector();
   ~CollisionDetector() {}

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void postAndUpdate(string, string, string);

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   std::map <std::string,NodeRecord> m_moos_map;  // holds the most recent NodeReport of a given vehicle
   std::map <std::pair<std::string,std::string>,CollisionRecord> m_col_bools; // holds the CollisionRecord associated with a given vehicle pair
   std::map <pair<std::string,std::string>,std::pair<std::string,std::string> > m_colregs_mode_map; //<v_os,v_cn>,<mode,submode>

   // make range pulses
   bool MakeRangePulse(double, double);
   bool MakeCollisionRangePulse(double, double);
   bool MakeNearMissRangePulse(double, double);
   bool MakeCPAViolationRangePulse(double, double);

   bool storeVehicleModes(CollisionRecord&, string, string); // stores vehicle modes for given vehicles (CR, v1, v2)
   
   double m_preferred_min_cpa_distance;
   double m_near_miss_distance;
   double m_collision_distance;
   
   double m_delay_time_to_clear;
   bool m_post_immediately;

   std::string m_post_string;
   std::string m_check_string;
   
   bool m_pulse_bool;
   double m_pulse_duration;
   double m_pulse_range;
   
   double m_start_checking_time;  // time to start checking for collisions
   double m_deploy_delay;   // time delay from deploy until start of checking
   
   bool m_check_collisions; // allows for no collision detection before certain time (TODO or manually enabling)
   
 private: // Configuration variables

 private: // State variables
   int m_total_collisions;
   int m_total_near_misses;
   int m_total_cpa_violations;
   bool m_start_running_by_clock;
};

#endif 



