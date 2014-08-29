/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ObstacleManager.h                                    */
/*    DATE: Aug 27th 2014                                        */
/*****************************************************************/

#ifndef P_OBSTACLE_MANAGER_HEADER
#define P_OBSTACLE_MANAGER_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYPolygon.h"

class ObstacleManager : public AppCastingMOOSApp
{
 public:
   ObstacleManager();
   ~ObstacleManager() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   bool handleMailNewPoint(std::string);
   void postConvexHulls();
   void postConvexHull(const std::string&, const std::vector<XYPoint>&);

 private: // Configuration variables
   std::string  m_point_var;          // incoming points
   std::string  m_obstacle_alert_var; // outgoing alerts

 private: // State variables
   std::vector<XYPoint> m_points;
   unsigned int         m_points_total;

   std::map<std::string, std::vector<XYPoint> > m_map_points;
   std::map<std::string, unsigned int>          m_map_points_total;
   std::map<std::string, bool>                  m_map_points_changed;

   std::string  m_most_recent_turn;
};

#endif 
