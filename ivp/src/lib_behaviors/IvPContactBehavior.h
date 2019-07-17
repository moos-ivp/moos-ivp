/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
#include "XYSegList.h"

class IvPDomain;
class IvPContactBehavior : public IvPBehavior {
public:
  IvPContactBehavior(IvPDomain);
  ~IvPContactBehavior() {}

  bool  setParam(std::string, std::string);
  void  setCPAEngine(const CPAEngine& engine) {m_cpa_engine=engine;}

 protected:
  bool  updatePlatformInfo();
  void  postViewableBearingLine();
  void  postErasableBearingLine();

  bool  checkContactGroupRestrictions();
  bool  checkContactTypeRestrictions();

  bool  handleSetParamMatchGroup(std::string);
  bool  handleSetParamIgnoreGroup(std::string);
  bool  handleSetParamMatchType(std::string);
  bool  handleSetParamIgnoreType(std::string);
  
  std::vector<std::string> getMatchGroup() const {return(m_match_group);}
  std::vector<std::string> getIgnoreGroup() const {return(m_ignore_group);}
  std::vector<std::string> getMatchType() const {return(m_match_type);}
  std::vector<std::string> getIgnoreType() const {return(m_ignore_type);}
  
  std::string getMatchGroupStr(std::string separator=":") const;
  std::string getIgnoreGroupStr(std::string separator=":") const;
  std::string getMatchTypeStr(std::string separator=":") const;
  std::string getIgnoreTypeStr(std::string separator=":") const;
  std::string getMatchIgnoreSummary() const;
  
 protected: // Configuration Parameters
  
  bool   m_on_no_contact_ok;  // true if no trouble reported on no contact
  bool   m_extrapolate;
  double m_decay_start;
  double m_decay_end;
  double m_time_on_leg;

  double m_complete_after_retired;
  
  XYSegList                m_bearing_line;
  bool                     m_bearing_line_show;
  std::vector<std::string> m_bearing_line_colors;
  std::vector<double>      m_bearing_line_thresh;
  std::string              m_bearing_line_info;

  std::string              m_match_contact_group;
  std::string              m_ignore_contact_group;

 protected:  // State Variables
  
  double m_osx;   // Current ownship x position (meters) 
  double m_osy;   // Current ownship y position (meters) 
  double m_osh;   // Current ownship heading (degrees 0-359)
  double m_osv;   // Current ownship speed (meters) 

  double m_cnx;   // Current contact x position (meters) 
  double m_cny;   // Current contact y position (meters) 
  double m_cnh;   // Current contact heading (degrees 0-359)
  double m_cnv;   // Current contact speed (meters) 
  double m_cnutc; // UTC time of last contact report

  std::string m_cn_group;
  std::string m_cn_vtype;

  double m_cn_not_retired_tstamp;
  bool   m_cn_retired;
  
  double m_contact_range; // Current range to contact (meters) 
  double m_relevance;

  LinearExtrapolator m_extrapolator;

  bool   m_os_fore_of_cn;
  bool   m_os_aft_of_cn;
  bool   m_os_port_of_cn;
  bool   m_os_starboard_of_cn;

  bool   m_cn_fore_of_os;
  bool   m_cn_aft_of_os;
  bool   m_cn_port_of_os;
  bool   m_cn_starboard_of_os;

  double m_cn_spd_in_os_pos;
  
  double m_os_cn_rel_bng;
  double m_cn_os_rel_bng;

  double m_os_cn_abs_bng;
  
  double m_rate_of_closure;
  double m_bearing_rate;
  double m_contact_rate;

  double m_range_gamma;
  double m_range_epsilon;

  bool   m_os_passes_cn;
  bool   m_os_passes_cn_port;
  bool   m_os_passes_cn_star;
  
  bool   m_cn_passes_os;
  bool   m_cn_passes_os_port;
  bool   m_cn_passes_os_star;
  
  bool   m_os_crosses_cn_stern;
  bool   m_os_crosses_cn_bow;
  bool   m_os_crosses_cn_bow_or_stern;
  double m_os_crosses_cn_bow_dist;

  bool   m_cn_crosses_os_bow;
  bool   m_cn_crosses_os_stern;
  bool   m_cn_crosses_os_bow_or_stern;
  double m_cn_crosses_os_bow_dist;

  double m_os_curr_cpa_dist;

  CPAEngine m_cpa_engine;
  CPAEngine m_rcpa_engine;

private:
  // Type and Group filters
  std::vector<std::string> m_match_type;
  std::vector<std::string> m_ignore_type;
  std::vector<std::string> m_match_group;
  std::vector<std::string> m_ignore_group;
};

#endif
