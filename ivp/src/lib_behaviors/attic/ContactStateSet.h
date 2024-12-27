/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactStateSet.h                                    */
/*    DATE: Sep 9th 2020                                         */
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
 
#ifndef CONTACT_STATE_SET_HEADER
#define CONTACT_STATE_SET_HEADER

#include <string>
#include <vector>
#include "ContactState.h"

class ContactStateSet {
public:
  ContactStateSet();
  ~ContactStateSet() {}

  //========================================================
  // Setters
  //========================================================
  void set_cnutc(double v)   {m_cnos_curr.set_cnutc(v);}
  void set_update_ok(bool v) {m_cnos_curr.set_update_ok(v);}
  void set_helm_iter(unsigned int v) {m_cnos_curr.set_helm_iter(v);}

  void set_range(double v)       {m_cnos_curr.set_range(v);}

  void set_os_fore_of_cn(bool v) {m_cnos_curr.set_os_fore_of_cn(v);}
  void set_os_aft_of_cn(bool v)  {m_cnos_curr.set_os_aft_of_cn(v);}
  void set_os_port_of_cn(bool v) {m_cnos_curr.set_os_port_of_cn(v);}
  void set_os_star_of_cn(bool v) {m_cnos_curr.set_os_star_of_cn(v);}
  
  void set_cn_fore_of_os(bool v) {m_cnos_curr.set_cn_fore_of_os(v);}
  void set_cn_aft_of_os(bool v)  {m_cnos_curr.set_cn_aft_of_os (v);}
  void set_cn_port_of_os(bool v) {m_cnos_curr.set_cn_port_of_os(v);}
  void set_cn_star_of_os(bool v) {m_cnos_curr.set_cn_star_of_os(v);}

  void set_cn_spd_in_os_pos(double v) {m_cnos_curr.set_cn_spd_in_os_pos(v);}

  void set_os_cn_rel_bng(double v)   {m_cnos_curr.set_os_cn_rel_bng(v);}
  void set_cn_os_rel_bng(double v)   {m_cnos_curr.set_cn_os_rel_bng(v);}
  void set_os_cn_abs_bng(double v)   {m_cnos_curr.set_os_cn_abs_bng(v);}
  void set_rate_of_closure(double v) {m_cnos_curr.set_rate_of_closure(v);}
  void set_bearing_rate(double v)    {m_cnos_curr.set_bearing_rate(v);}
  void set_contact_rate(double v)    {m_cnos_curr.set_contact_rate(v);}

  void set_range_gamma(double v)     {m_cnos_curr.set_range_gamma(v);}
  void set_range_epsilon(double v)   {m_cnos_curr.set_range_epsilon(v);}

  void set_os_passes_cn(bool v)      {m_cnos_curr.set_os_passes_cn(v);}
  void set_os_passes_cn_port(bool v) {m_cnos_curr.set_os_passes_cn_port(v);}
  void set_os_passes_cn_star(bool v) {m_cnos_curr.set_os_passes_cn_star(v);}
  void set_cn_passes_os(bool v)      {m_cnos_curr.set_cn_passes_os(v);}
  void set_cn_passes_os_port(bool v) {m_cnos_curr.set_cn_passes_os_port(v);}
  void set_cn_passes_os_star(bool v) {m_cnos_curr.set_cn_passes_os_star(v);}

  void set_os_crosses_cn(bool v)            {m_cnos_curr.set_os_crosses_cn(v);}
  void set_os_crosses_cn_stern(bool v)      {m_cnos_curr.set_os_crosses_cn_stern(v);}
  void set_os_crosses_cn_bow(bool v)        {m_cnos_curr.set_os_crosses_cn_bow(v);}
  void set_os_crosses_cn_bow_dist(double v) {m_cnos_curr.set_os_crosses_cn_bow_dist(v);}

  void set_cn_crosses_os(bool v)            {m_cnos_curr.set_cn_crosses_os(v);}
  void set_cn_crosses_os_bow(bool v)        {m_cnos_curr.set_cn_crosses_os_bow(v);}
  void set_cn_crosses_os_stern(bool v)      {m_cnos_curr.set_cn_crosses_os_stern(v);}
  void set_cn_crosses_os_bow_dist(double v) {m_cnos_curr.set_cn_crosses_os_bow_dist(v);}

  void set_os_curr_cpa_dist(double v)        {m_cnos_curr.set_os_curr_cpa_dist(v);}

  unsigned int size() const {return(m_cnos_hist.size());}

  
  //========================================================
  // Getters: Bookkeeping
  //========================================================
  double cnutc(unsigned int ix=0) const;
  bool   update_ok(unsigned int ix=0) const;
  unsigned int helm_iter(unsigned int ix=0) const;

  //========================================================
  // Getters: Direct measurements about os/cn relative
  //          positions at the current instant in time
  //========================================================
 public:
  bool   os_fore_of_cn(unsigned int ix=0) const;
  bool   os_aft_of_cn(unsigned int ix=0) const;
  bool   os_port_of_cn(unsigned int ix=0) const;
  bool   os_star_of_cn(unsigned int ix=0) const; 
  
  bool   cn_fore_of_os(unsigned int ix=0) const;
  bool   cn_aft_of_os(unsigned int ix=0) const;
  bool   cn_port_of_os(unsigned int ix=0) const;
  bool   cn_star_of_os(unsigned int ix=0) const;

  double cn_spd_in_os_pos(unsigned int ix=0) const;

  double os_cn_rel_bng(unsigned int ix=0) const;
  double cn_os_rel_bng(unsigned int ix=0) const;
  double os_cn_abs_bng(unsigned int ix=0) const;
  double rate_of_closure(unsigned int ix=0) const;
  double bearing_rate(unsigned int ix=0) const;
  double contact_rate(unsigned int ix=0) const;

  double range_gamma(unsigned int ix=0) const;
  double range_epsilon(unsigned int ix=0) const;

  //========================================================
  // Analyzers (1) About what WILL happen if os/cn continue
  //               Values come from the CPAEngine(s)
  //========================================================
  bool   os_passes_cn(unsigned int ix=0) const;
  bool   os_passes_cn_port(unsigned int ix=0) const;
  bool   os_passes_cn_star(unsigned int ix=0) const;
  bool   cn_passes_os(unsigned int ix=0) const;
  bool   cn_passes_os_port(unsigned int ix=0) const;
  bool   cn_passes_os_star(unsigned int ix=0) const;

  bool   os_crosses_cn(unsigned int ix=0) const;
  bool   os_crosses_cn_bow_or_stern(unsigned int ix=0) const;
  bool   os_crosses_cn_stern(unsigned int ix=0) const;
  bool   os_crosses_cn_bow(unsigned int ix=0) const;
  double os_crosses_cn_bow_dist(unsigned int ix=0) const;

  bool   cn_crosses_os(unsigned int ix=0) const;
  bool   cn_crosses_os_bow_or_stern(unsigned int ix=0) const;
  bool   cn_crosses_os_bow(unsigned int ix=0) const;
  bool   cn_crosses_os_stern(unsigned int ix=0) const;
  double cn_crosses_os_bow_dist(unsigned int ix=0) const;

  double os_curr_cpa_dist(unsigned int ix=0) const;

  //========================================================
  // Analyzers (2) About what just happened on this step
  //               Values come from cnos and cnos_hist
  //========================================================

  bool cpa_reached(double thresh=0.05) const;
  bool rng_entered(double) const;
  bool rng_exited(double) const;

  bool os_passed_cn() const;
  bool os_passed_cn_port() const;
  bool os_passed_cn_star() const;
  bool cn_passed_os() const;
  bool cn_passed_os_port() const;
  bool cn_passed_os_star() const;

  bool os_crossed_cn() const;
  bool os_crossed_cn_stern() const;
  bool os_crossed_cn_bow() const;  
  bool cn_crossed_os() const;
  bool cn_crossed_os_bow() const;
  bool cn_crossed_os_stern() const;


  
  
protected:
  unsigned int m_max_history;
  
 public: 
  std::string cnMacroExpand(std::string, std::string) const;

  void archiveCurrent();
  
 protected:
  ContactState cnos(unsigned int) const;
  
 protected:  // State Variables
  ContactState m_cnos_curr;
  std::vector<ContactState> m_cnos_hist;

  unsigned int m_counter;
};

#endif


