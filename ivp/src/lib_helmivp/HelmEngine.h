/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmEngine.h (formerly HelmEngineBeta)               */
/*    DATE: July 29th, 2009                                      */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifndef HELM_ENGINE_HEADER
#define HELM_ENGINE_HEADER

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
  unsigned int m_iteration;
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





















