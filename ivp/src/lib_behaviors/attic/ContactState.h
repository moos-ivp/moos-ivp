/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactState.h                                       */
/*    DATE: Sep 7th 2020                                         */
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
 
#ifndef CONTACT_STATE_HEADER
#define CONTACT_STATE_HEADER

#include <string>
#include <vector>

class ContactState {
public:
  ContactState();
  ~ContactState() {}

  void set_index(unsigned int ix) {m_index = ix;}
  unsigned int get_index() const {return(m_index);}
  
  // Setters
  void set_cnutc(double v)   {m_cnutc = v;}
  void set_update_ok(bool v) {m_update_ok = v;}
  void set_helm_iter(unsigned int v) {m_helm_iter = v;}

  void set_range(double v)       {m_range = v;}
  
  void set_os_fore_of_cn(bool v) {m_os_fore_of_cn = v;}
  void set_os_aft_of_cn(bool v)  {m_os_aft_of_cn  = v;}
  void set_os_port_of_cn(bool v) {m_os_port_of_cn = v;}
  void set_os_star_of_cn(bool v) {m_os_star_of_cn = v;}
  
  void set_cn_fore_of_os(bool v) {m_cn_fore_of_os = v;}
  void set_cn_aft_of_os(bool v)  {m_cn_aft_of_os  = v;}
  void set_cn_port_of_os(bool v) {m_cn_port_of_os = v;}
  void set_cn_star_of_os(bool v) {m_cn_star_of_os = v;}

  void set_cn_spd_in_os_pos(double v) {m_cn_spd_in_os_pos = v;}

  void set_os_cn_rel_bng(double v) {m_os_cn_rel_bng = v;}
  void set_cn_os_rel_bng(double v) {m_cn_os_rel_bng = v;}
  void set_os_cn_abs_bng(double v) {m_os_cn_abs_bng = v;}
  void set_rate_of_closure(double v) {m_rate_of_closure = v;}
  void set_bearing_rate(double v) {m_bearing_rate = v;}
  void set_contact_rate(double v) {m_contact_rate = v;}

  void set_range_gamma(double v) {m_range_gamma = v;}
  void set_range_epsilon(double v) {m_range_epsilon = v;}

  void set_os_passes_cn(bool v)      {m_os_passes_cn = v;}
  void set_os_passes_cn_port(bool v) {m_os_passes_cn_port = v;}
  void set_os_passes_cn_star(bool v) {m_os_passes_cn_star = v;}
  void set_cn_passes_os(bool v)      {m_cn_passes_os = v;}
  void set_cn_passes_os_port(bool v) {m_cn_passes_os_port = v;}
  void set_cn_passes_os_star(bool v) {m_cn_passes_os_star = v;}

  void set_os_crosses_cn(bool v)            {m_os_crosses_cn = v;}
  void set_os_crosses_cn_stern(bool v)      {m_os_crosses_cn_stern = v;}
  void set_os_crosses_cn_bow(bool v)        {m_os_crosses_cn_bow = v;}
  void set_os_crosses_cn_bow_dist(double v) {m_os_crosses_cn_bow_dist = v;}

  void set_cn_crosses_os(bool v)            {m_cn_crosses_os = v;}
  void set_cn_crosses_os_bow(bool v)        {m_cn_crosses_os_bow = v;}
  void set_cn_crosses_os_stern(bool v)      {m_cn_crosses_os_stern = v;}
  void set_cn_crosses_os_bow_dist(double v) {m_cn_crosses_os_bow_dist = v;}

  void set_os_curr_cpa_dist(double v) {m_os_curr_cpa_dist = v;}

  //========================================================
  // Getters
  //========================================================
  double cnutc() const     {return(m_cnutc);}
  bool   update_ok() const {return(m_update_ok);}
  unsigned int helm_iter() {return(m_helm_iter);}
  
  double range() const {return(m_range);}

  bool   os_fore_of_cn() const {return(m_os_fore_of_cn);}
  bool   os_aft_of_cn() const  {return(m_os_aft_of_cn);}
  bool   os_port_of_cn() const {return(m_os_port_of_cn);}
  bool   os_star_of_cn() const {return(m_os_star_of_cn);}
  
  bool   cn_fore_of_os() const {return(m_cn_fore_of_os);}
  bool   cn_aft_of_os() const  {return(m_cn_aft_of_os);}
  bool   cn_port_of_os() const {return(m_cn_port_of_os);}
  bool   cn_star_of_os() const {return(m_cn_star_of_os);}

  double cn_spd_in_os_pos() const {return(m_cn_spd_in_os_pos);}

  double os_cn_rel_bng() const {return(m_os_cn_rel_bng);}
  double cn_os_rel_bng() const {return(m_cn_os_rel_bng);}
  double os_cn_abs_bng() const {return(m_os_cn_abs_bng);}
  double rate_of_closure() const {return(m_rate_of_closure);}
  double bearing_rate() const {return(m_bearing_rate);}
  double contact_rate() const {return(m_contact_rate);}

  double range_gamma() const {return(m_range_gamma);}
  double range_epsilon() const {return(m_range_epsilon);}

  bool   os_passes_cn() const      {return(m_os_passes_cn);}
  bool   os_passes_cn_port() const {return(m_os_passes_cn_port);}
  bool   os_passes_cn_star() const {return(m_os_passes_cn_star);}
  bool   cn_passes_os() const      {return(m_cn_passes_os);}
  bool   cn_passes_os_port() const {return(m_cn_passes_os_port);}
  bool   cn_passes_os_star() const {return(m_cn_passes_os_star);}

  bool   os_crosses_cn() const          {return(m_os_crosses_cn);}
  bool   os_crosses_cn_stern() const    {return(m_os_crosses_cn_stern);}
  bool   os_crosses_cn_bow() const      {return(m_os_crosses_cn_bow);}
  double os_crosses_cn_bow_dist() const {return(m_os_crosses_cn_bow_dist);}

  bool   cn_crosses_os() const          {return(m_cn_crosses_os);}
  bool   cn_crosses_os_bow() const      {return(m_cn_crosses_os_bow);}
  bool   cn_crosses_os_stern() const    {return(m_cn_crosses_os_stern);}
  double cn_crosses_os_bow_dist() const {return(m_cn_crosses_os_bow_dist);}

  double os_curr_cpa_dist() const   {return(m_os_curr_cpa_dist);}

 public: // Macro Handling
  std::string cnMacroExpand(std::string, std::string) const;
  
 protected:  // State Variables

  double       m_cnutc;
  bool         m_update_ok;
  unsigned int m_helm_iter;
  unsigned int m_index;

  
  // Static variables derived purely on ownship and contact
  // present position and trajectory  
  double m_range;

  bool   m_os_fore_of_cn;
  bool   m_os_aft_of_cn;
  bool   m_os_port_of_cn;
  bool   m_os_star_of_cn;

  bool   m_cn_fore_of_os;
  bool   m_cn_aft_of_os;
  bool   m_cn_port_of_os;
  bool   m_cn_star_of_os;

  double m_cn_spd_in_os_pos;
  
  double m_os_cn_rel_bng;
  double m_cn_os_rel_bng;
  double m_os_cn_abs_bng;
  
  double m_rate_of_closure;
  double m_bearing_rate;
  double m_contact_rate;

  double m_range_gamma;
  double m_range_epsilon;

  // State variables representing predictions if ownship and
  // contact stay on their current trajectories.
  
  bool   m_os_passes_cn;
  bool   m_os_passes_cn_port;
  bool   m_os_passes_cn_star;
  
  bool   m_cn_passes_os;
  bool   m_cn_passes_os_port;
  bool   m_cn_passes_os_star;
  
  bool   m_os_crosses_cn;
  bool   m_os_crosses_cn_stern;
  bool   m_os_crosses_cn_bow;
  double m_os_crosses_cn_bow_dist;

  bool   m_cn_crosses_os;
  bool   m_cn_crosses_os_bow;
  bool   m_cn_crosses_os_stern;
  double m_cn_crosses_os_bow_dist;

  double m_os_curr_cpa_dist;
};

#endif


