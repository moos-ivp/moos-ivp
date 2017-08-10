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
   ~LoiterAssign();

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

   void checkFinalConfiguration();

 protected:
   bool polyWithinOpRegion(XYPolygon);
   
 private: // Configuration variables
   XYPolygon m_opregion;

   double    m_poly_rad;
   double    m_poly_sep;
   
 private: // State variables

   std::vector<std::string> m_vnames;
   std::vector<XYPolygon>   m_polys;
   std::vector<bool>        m_poly_fixed;

};

#endif 
