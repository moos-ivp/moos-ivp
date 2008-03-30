/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmIvP.h                                            */
/*    DATE: Oct 12th 2004                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef HelmIvP_HEADER
#define HelmIvP_HEADER

#include <string>
#include <set>
#include <map>
#include "MOOSLib.h"
#include "InfoBuffer.h"
#include "IvPDomain.h"
#include "BehaviorSet.h"
#include "HelmEngine.h"

class HelmIvP : public CMOOSApp
{
public:
  HelmIvP();
  virtual ~HelmIvP();
  
  void cleanup();
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void addBehaviorFile(std::string);
  
protected:
  bool handleDomainEntry(const std::string&);
  bool updateInfoBuffer(CMOOSMsg &Msg);
  void postCharStatus();
  void postBehaviorMessages();
  void postInitialVariables();
  void postDefaultVariables();
  void postSummary();
  void registerVariables();
  void requestBehaviorLogging();

protected:
  InfoBuffer*   info_buffer;
  bool          has_control;
  bool          allow_overide;
  IvPDomain     ivp_domain;
  BehaviorSet*  bhv_set;
  bool          ylog_flag;
  std::string   verbose;
  int           iteration;
  double        ok_skew;
  bool          skews_matter;

  HelmEngine*   hengine;
  std::string   ownship;

  std::map<std::string, bool> optional_var;

  std::set<std::string> bhv_files;
};

#endif 









