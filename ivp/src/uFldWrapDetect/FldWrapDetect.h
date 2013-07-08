/************************************************************/
/*    NAME: Kyle Woerner                                              */
/*    ORGN: MIT                                             */
/*    FILE: FldWrapDetect.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef FldWrapDetect_HEADER
#define FldWrapDetect_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <map>
#include "NodeRecord.h"
#include "XYSegment.h"

class FldWrapDetect : public AppCastingMOOSApp
{
public:
FldWrapDetect();
~FldWrapDetect();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();
   bool CheckForWrap(std::string);
   bool buildReport();


 private: // Configuration variables

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;

   bool m_deployed;
   std::map <std::string, std::list <XYSegment> > m_xysegments;
   std::map <std::string,NodeRecord> m_node_record;
   std::map <std::string,double> m_odometer;
   int m_max_num_segments;
   double m_max_trail_distance;
   int m_num_wraps;
};

#endif 
