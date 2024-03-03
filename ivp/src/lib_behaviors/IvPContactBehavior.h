/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: IvPContactBehavior.h                                 */
/*    DATE: Apr 3rd 2010 Separated/generalized from indiv. bhvs  */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef IVP_CONTACT_BEHAVIOR_HEADER
#define IVP_CONTACT_BEHAVIOR_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"
#include "CPAEngine.h"
#include "LinearExtrapolator.h"
#include "ExFilterSet.h"
#include "ContactStateSet.h"

class IvPDomain;
class IvPContactBehavior : public IvPBehavior {
public:
  IvPContactBehavior(IvPDomain);
  ~IvPContactBehavior() {}

  virtual void onSetParamComplete();
  
 public: // Override virtual funcions 
  bool  setParam(std::string, std::string);
  void  setCPAEngine(const CPAEngine& engine) {m_cpa_engine=engine;}
  void  onEveryState(std::string new_state);
  std::string expandMacros(std::string);
  
 protected:
  bool  updatePlatformInfo();
  void  postViewableBearingLine(bool active=true);

  bool  postingPerContactInfo() const {return(m_post_per_contact_info);}
  bool  platformUpdateOK() const {return(m_cnos.update_ok());}

  void  postFlag(const VarDataPair&, bool repeat=false);
  bool  addContactFlag(std::string);

  bool  filterCheckHolds() const;
  
  std::string getFilterSummary() const {return(m_filter_set.getSummary());}

  void  handleContactFlags();
  void  handleContactFlagRange(VarDataPair);
  
 protected: // Configuration Parameters
  
  bool   m_on_no_contact_ok;  // true if no trouble reported on no contact
  bool   m_extrapolate;
  double m_decay_start;
  double m_decay_end;
  double m_time_on_leg;

  bool   m_post_per_contact_info;

  ExFilterSet m_filter_set;
  bool m_exit_on_filter_vtype;
  bool m_exit_on_filter_group;
  bool m_exit_on_filter_region;
  
  bool                     m_bearing_line_show;
  std::vector<std::string> m_bearing_line_colors;
  std::vector<double>      m_bearing_line_thresh;
  std::string              m_bearing_line_info;
  bool                     m_bearing_line_label_show;

 protected:  // State Variables
  
  double m_cnx;   // Current contact x position (meters) 
  double m_cny;   // Current contact y position (meters) 
  double m_cnh;   // Current contact heading (degrees 0-359)
  double m_cnv;   // Current contact speed (meters) 
  double m_contact_range; // Current range to contact (meters) 

  std::string m_cn_group;
  std::string m_cn_vtype;

  double m_relevance;

  LinearExtrapolator m_extrapolator;

  CPAEngine m_cpa_engine;
  CPAEngine m_rcpa_engine;

  ContactStateSet m_cnos;

private:
  std::vector<VarDataPair> m_cnflags;
};

#endif

