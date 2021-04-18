/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT                                             */
/*    FILE: LoiterAssign.h                                  */
/*    DATE: August 9th, 2017                                */
/************************************************************/

#ifndef LOITER_ASSIGN_HEADER
#define LOITER_ASSIGN_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

#include <string>
#include <vector>
#include "XYPolygon.h"

class LoiterAssign : public AppCastingMOOSApp
{
 public:
   LoiterAssign();
   ~LoiterAssign() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   bool handleConfigOpRegion(std::string);   
   bool handleConfigVNames(std::string);   
   bool handleConfigLoiterPoly(std::string);   

   void handleFinalConfiguration();
   void conditionallyReassign();
   void derangeReassign();
   void rebuildReassign();
   
 protected:
   bool polyWithinOpRegion(XYPolygon);
   void postViewablePolys();
   
 private: // Configuration variables
   XYPolygon m_opregion;

   double    m_poly_rad;
   double    m_poly_sep;
   double    m_interval;

   bool      m_derange;
   
   std::string  m_moosvar_assign;
   
   std::string  m_opreg_poly_edge_color;
   std::string  m_opreg_poly_vert_color;
   std::string  m_loiter_poly_edge_color;
   std::string  m_loiter_poly_vert_color;

 private: // State variables

   std::vector<std::string> m_vnames;
   std::vector<XYPolygon>   m_polys;
   std::vector<bool>        m_poly_fixed;

   unsigned int  m_total_reassigns;
   double        m_time_prev_assign;
   
};

#endif 
