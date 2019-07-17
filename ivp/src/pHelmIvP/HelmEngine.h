/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmEngine.h (formerly HelmEngineBeta)               */
/*    DATE: July 29th, 2009                                      */
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

#ifndef HELM_ENGINE_HEADER
#define HELM_ENGINE_HEADER

#include <map>
#include <string>
#include <vector>
#include "IvPDomain.h"
#include "HelmReport.h"
#include "MBTimer.h"

class InfoBuffer;
class IvPFunction;
class IvPProblem;
class BehaviorSet;
class HelmEngine {
public:
  HelmEngine(IvPDomain, InfoBuffer*);
  ~HelmEngine();

  HelmReport determineNextDecision(BehaviorSet *bset, double curr_time);

  unsigned long int size() const;
  
protected:
  bool   checkOFDomains(std::vector<IvPFunction*>);

  bool   part1_PreliminaryBehaviorSetHandling();
  bool   part2_GetFunctionsFromBehaviorSet(int filter_level);
  bool   part3_VerifyFunctionDomains();
  bool   part4_BuildAndSolveIvPProblem(std::string phase="direct");
  bool   part5_FreeMemoryIPFs();
  bool   part6_FinishHelmReport();

protected:
  IvPDomain  m_ivp_domain;
  IvPDomain  m_sub_domain;

  // Intermediate structures while determining next decision
  unsigned int m_iteration;
  HelmReport   m_helm_report;
  BehaviorSet *m_bhv_set;
  double       m_curr_time;
  unsigned int m_total_pcs_formed;
  unsigned int m_total_pcs_cached;
  IvPProblem  *m_ivp_problem;
  InfoBuffer  *m_info_buffer;

  double       m_max_create_time;
  double       m_max_solve_time;
  double       m_max_loop_time;

  std::map<std::string, IvPFunction*> m_map_ipfs;
  std::map<std::string, IvPFunction*> m_map_ipfs_prev;

  MBTimer  m_create_timer;
  MBTimer  m_ipf_timer;
  MBTimer  m_solve_timer;
};

#endif
