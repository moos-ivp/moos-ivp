/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmEngine.h                                         */
/*    DATE: Mar 24th, 2005                                       */
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

#ifndef HELM_ENGINE_HEADER
#define HELM_ENGINE_HEADER

#include <vector>
#include "IvPDomain.h"
#include "HelmReport.h"

class IvPFunction;
class BehaviorSet;
class HelmEngine {
public:
  HelmEngine(IvPDomain);
  ~HelmEngine() {};

  HelmReport determineNextDecision(BehaviorSet *bset, double curr_time);

protected:
  bool   checkOFDomains(std::vector<IvPFunction*>);

protected:
  IvPDomain  m_ivp_domain;
  IvPDomain  m_sub_domain;
  int        m_iteration;
};

#endif




















