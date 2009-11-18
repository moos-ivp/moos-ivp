/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmEngineBeta.h                                     */
/*    DATE: July 29th, 2009                                      */
/*    DATE: Mar 24th, 2005 (based on an older implementation)    */
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

#ifndef HELM_ENGINE_BETA_HEADER
#define HELM_ENGINE_BETA_HEADER

#include <vector>
#include "IvPDomain.h"
#include "HelmReport.h"
#include "MBTimer.h"

class InfoBuffer;
class IvPFunction;
class IvPProblem;
class BehaviorSet;
class HelmEngineBeta {
public:
  HelmEngineBeta(IvPDomain, InfoBuffer*);
  ~HelmEngineBeta();

  HelmReport determineNextDecision(BehaviorSet *bset, double curr_time);

protected:
  bool   checkOFDomains(std::vector<IvPFunction*>);

  bool   part1_PreliminaryBehaviorSetHandling();
  bool   part2_GetFunctionsFromBehaviorSet(int filter_level);
  bool   part3_VerifyFunctionDomains();
  bool   part4_BuildAndSolveIvPProblem(std::string phase="direct");
  bool   part6_FinishHelmReport();

protected:
  IvPDomain  m_ivp_domain;
  IvPDomain  m_sub_domain;

  // Intermediate structures while determining next decision
  int          m_iteration;
  HelmReport   m_helm_report;
  BehaviorSet *m_bhv_set;
  double       m_curr_time;
  IvPProblem  *m_ivp_problem;
  InfoBuffer  *m_info_buffer;

  std::vector<IvPFunction*> m_ivp_functions;

  MBTimer  m_create_timer;
  MBTimer  m_ipf_timer;
  MBTimer  m_solve_timer;
};

#endif




















