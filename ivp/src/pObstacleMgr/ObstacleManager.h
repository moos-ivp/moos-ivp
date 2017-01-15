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
   bool handleMailUpdatesRequest(std::string);
   void postConvexHullAlerts();
   void postConvexHullAlert(std::string obstacle_key);
   void postConvexHullUpdates();
   void postConvexHullUpdate(std::string obstacle_key);

   XYPolygon genPseudoHull(const std::vector<XYPoint>& pts, double radius);

   XYPolygon placeholderConvexHull(std::string obstacle_key);

   XYPoint customStringToPoint(std::string point_str);

 private: // Configuration variables
   std::string  m_point_var;            // incoming points
   std::string  m_updates_request_var;  // incoming update requests
   std::string  m_obstacle_alert_var;   // outgoing alerts

   std::string  m_key_field;
   std::string  m_x_field;
   std::string  m_y_field;

   double       m_hull_approx;
   
 private: // State variables
   unsigned int         m_points_total;

   // Each map is keyed on the obstacle_key, e.g. buoy-a, buoy-b etc
   //std::map<std::string, std::vector<XYPoint> > m_map_points;
   std::map<std::string, std::list<XYPoint> >   m_map_points;
   std::map<std::string, XYPolygon>             m_map_convex_hull;
   std::map<std::string, unsigned int>          m_map_points_total;
   std::map<std::string, bool>                  m_map_hull_changed_flag;
   std::map<std::string, bool>                  m_map_alerted_flag;
   std::map<std::string, std::string>           m_map_updates_var;
   std::map<std::string, unsigned int>          m_map_updates_total;
};

#endif 
