/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Relayer.h                                            */
/*    DATE: Jun 26th 2008                                        */
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

#ifndef PXR_MOOSAPP_HEADER
#define PXR_MOOSAPP_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class PXR_MOOSApp : public CMOOSApp
{
 public:
  PXR_MOOSApp();
  virtual ~PXR_MOOSApp() {}

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  void setIncomingVar(std::string s) {m_incoming_var=s;}
  void setOutgoingVar(std::string s) {m_outgoing_var=s;}

 protected:
  void RegisterVariables();

 protected: // State variables
  unsigned long int m_tally_recd;
  unsigned long int m_tally_sent;
  unsigned long int m_iterations;

 protected: // Configuration variables
  std::string       m_incoming_var;
  std::string       m_outgoing_var;

  double            m_start_time_postings;
  double            m_start_time_iterations;
};

#endif 









