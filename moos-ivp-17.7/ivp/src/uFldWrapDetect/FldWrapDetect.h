/*****************************************************************/
/*    NAME: Kyle Woerner, Michael Benjamin                       */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FldWrapDetect.h                                      */
/*    DATE:                                                      */
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







