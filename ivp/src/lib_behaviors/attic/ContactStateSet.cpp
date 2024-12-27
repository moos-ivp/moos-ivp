/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactStateSet.cpp                                  */
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

#include "ContactStateSet.h"
#include "MBUtils.h"
#include "MacroUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

ContactStateSet::ContactStateSet()
{
  m_max_history = 5;
  m_counter = 0;
}

//-----------------------------------------------------------
// Procedure: cnMacroExpand()

string ContactStateSet::cnMacroExpand(string sdata, string macro) const
{
  return(m_cnos_curr.cnMacroExpand(sdata, macro));
}
  
//-----------------------------------------------------------
// Procedure: archiveCurrent()

void ContactStateSet::archiveCurrent()
{
  // Special case: If cnutc is zero, then cnos_curr is empty
  // and should not be archived. This occurrs on the very first
  // iteration of a contact behavior.
  if(m_cnos_curr.cnutc() == 0)
    return;
  
  vector<ContactState> new_cnos_hist;

  new_cnos_hist.push_back(m_cnos_curr);
  m_counter++;
  m_cnos_curr.set_index(m_counter);
  
  for(unsigned int i=0; i<m_cnos_hist.size(); i++) {
    if(new_cnos_hist.size() < m_max_history)
      new_cnos_hist.push_back(m_cnos_hist[i]);
  }
    
  m_cnos_hist = new_cnos_hist;
}


//-----------------------------------------------------------
// Procedure: cnos()
//   Purpose: A convenience utility to obtain the ContatState
//            entry indicated by the given index.
//            IX = 0 indicates m_cnos_curr
//            IX = N indicates m_cnos_hist[N-1]
//            If N exceeds the size of cnos_hist, the last
//            element in the cnos_hist is used.
//
//                     0 1 2 3   hist.size=4
//            |a|     |b|c|d|e|
//         ix: 0       1 2 3 4

ContactState ContactStateSet::cnos(unsigned int ix) const
{
  if((ix == 0) || (m_cnos_hist.size() == 0))
    return(m_cnos_curr);

  if(ix > m_cnos_hist.size())
    return(m_cnos_hist.back());  

  return(m_cnos_hist[ix-1]);
}


//-----------------------------------------------------------
// Procedure: cnutc()
//   Purpose: Get the timestamp of the ContactState entry

double ContactStateSet::cnutc(unsigned int ix) const
{
  return(cnos(ix).cnutc());
}

//-----------------------------------------------------------
// Procedure: update_ok()
//   Purpose: Get the validity of this ContactState entry

bool ContactStateSet::update_ok(unsigned int ix) const
{
  return(cnos(ix).update_ok());
}

//-----------------------------------------------------------
// Procedure: helm_iter()

unsigned int ContactStateSet::helm_iter(unsigned int ix) const
{
  return(cnos(ix).helm_iter());
}

//-----------------------------------------------------------
// Procedure: os_fore_of_cn()

bool ContactStateSet::os_fore_of_cn(unsigned int ix) const
{
  return(cnos(ix).os_fore_of_cn());
}

//-----------------------------------------------------------
// Procedure: os_aft_of_cn()

bool ContactStateSet::os_aft_of_cn(unsigned int ix) const
{
  return(cnos(ix).os_aft_of_cn());
}

//-----------------------------------------------------------
// Procedure: os_port_of_cn()

bool ContactStateSet::os_port_of_cn(unsigned int ix) const
{
  return(cnos(ix).os_port_of_cn());
}

//-----------------------------------------------------------
// Procedure: os_star_of_cn()

bool ContactStateSet::os_star_of_cn(unsigned int ix) const
{
  return(cnos(ix).os_star_of_cn());
}


//-----------------------------------------------------------
// Procedure: cn_fore_of_os()

bool ContactStateSet::cn_fore_of_os(unsigned int ix) const
{
  return(cnos(ix).cn_fore_of_os());
}

//-----------------------------------------------------------
// Procedure: cn_aft_of_os()

bool ContactStateSet::cn_aft_of_os(unsigned int ix) const
{
  return(cnos(ix).cn_aft_of_os());
}

//-----------------------------------------------------------
// Procedure: cn_port_of_os()

bool ContactStateSet::cn_port_of_os(unsigned int ix) const
{
  return(cnos(ix).cn_port_of_os());
}


//-----------------------------------------------------------
// Procedure: cn_star_of_os()

bool ContactStateSet::cn_star_of_os(unsigned int ix) const
{
  return(cnos(ix).cn_star_of_os());
}

//-----------------------------------------------------------
// Procedure: cn_spd_in_os_pos()

double ContactStateSet::cn_spd_in_os_pos(unsigned int ix) const
{
  return(cnos(ix).cn_spd_in_os_pos());
}

//-----------------------------------------------------------
// Procedure: os_cn_rel_bng()

double ContactStateSet::os_cn_rel_bng(unsigned int ix) const
{
  return(cnos(ix).os_cn_rel_bng());
}

//-----------------------------------------------------------
// Procedure: cn_os_rel_bng()

double ContactStateSet::cn_os_rel_bng(unsigned int ix) const
{
  return(cnos(ix).cn_os_rel_bng());
}

//-----------------------------------------------------------
// Procedure: os_cn_abs_bng()

double ContactStateSet::os_cn_abs_bng(unsigned int ix) const
{
  return(cnos(ix).os_cn_abs_bng());
}

//-----------------------------------------------------------
// Procedure: rate_of_closure()

double ContactStateSet::rate_of_closure(unsigned int ix) const
{
  return(cnos(ix).rate_of_closure());
}

//-----------------------------------------------------------
// Procedure: bearing_rate()

double ContactStateSet::bearing_rate(unsigned int ix) const
{
  return(cnos(ix).bearing_rate());
}

//-----------------------------------------------------------
// Procedure: contact_rate()

double ContactStateSet::contact_rate(unsigned int ix) const
{
  return(cnos(ix).contact_rate());
}

//-----------------------------------------------------------
// Procedure: range_gamma()

double ContactStateSet::range_gamma(unsigned int ix) const
{
  return(cnos(ix).range_gamma());
}

//-----------------------------------------------------------
// Procedure: range_epsilon()

double ContactStateSet::range_epsilon(unsigned int ix) const
{
  return(cnos(ix).range_epsilon());
}

//-----------------------------------------------------------
// Procedure: os_passes_cn()

bool ContactStateSet::os_passes_cn(unsigned int ix) const
{
  return(cnos(ix).os_passes_cn());
}

//-----------------------------------------------------------
// Procedure: os_passes_cn_port()

bool ContactStateSet::os_passes_cn_port(unsigned int ix) const
{
  return(cnos(ix).os_passes_cn_port());
}

//-----------------------------------------------------------
// Procedure: os_passes_cn_star()

bool ContactStateSet::os_passes_cn_star(unsigned int ix) const
{
  return(cnos(ix).os_passes_cn_star());
}

//-----------------------------------------------------------
// Procedure: cn_passes_os()

bool ContactStateSet::cn_passes_os(unsigned int ix) const
{
  return(cnos(ix).cn_passes_os());
}

//-----------------------------------------------------------
// Procedure: cn_passes_os_port()

bool ContactStateSet::cn_passes_os_port(unsigned int ix) const
{
  return(cnos(ix).cn_passes_os_port());
}

//-----------------------------------------------------------
// Procedure: cn_passes_os_star()

bool ContactStateSet::cn_passes_os_star(unsigned int ix) const
{
  return(cnos(ix).cn_passes_os_star());
}

//-----------------------------------------------------------
// Procedure: os_crosses_cn_stern()

bool ContactStateSet::os_crosses_cn_stern(unsigned int ix) const
{
  return(cnos(ix).os_crosses_cn_stern());
}

//-----------------------------------------------------------
// Procedure: os_crosses_cn_bow()

bool ContactStateSet::os_crosses_cn_bow(unsigned int ix) const
{
  return(cnos(ix).os_crosses_cn_bow());
}

//-----------------------------------------------------------
// Procedure: os_crosses_cn()

bool ContactStateSet::os_crosses_cn(unsigned int ix) const
{
  return(cnos(ix).os_crosses_cn());
}

//-----------------------------------------------------------
// Procedure: os_crosses_cn_bow_or_stern()
//      Note: Same as os_crosses_cn() Depricated Sep 2020

bool ContactStateSet::os_crosses_cn_bow_or_stern(unsigned int ix) const
{
  return(cnos(ix).os_crosses_cn());
}

//-----------------------------------------------------------
// Procedure: os_crosses_cn_bow_dist()

double ContactStateSet::os_crosses_cn_bow_dist(unsigned int ix) const
{
  return(cnos(ix).os_crosses_cn_bow_dist());
}

//-----------------------------------------------------------
// Procedure: cn_crosses_os_bow()

bool ContactStateSet::cn_crosses_os_bow(unsigned int ix) const
{
  return(cnos(ix).cn_crosses_os_bow());
}

//-----------------------------------------------------------
// Procedure: cn_crosses_os_stern()

bool ContactStateSet::cn_crosses_os_stern(unsigned int ix) const
{
  return(cnos(ix).cn_crosses_os_stern());
}

//-----------------------------------------------------------
// Procedure: cn_crosses_os()

bool ContactStateSet::cn_crosses_os(unsigned int ix) const
{
  return(cnos(ix).cn_crosses_os());
}

//-----------------------------------------------------------
// Procedure: cn_crosses_os_bow_or_stern()
//      Note: Same as cn_crosses_os() Depricated Sep 2020

bool ContactStateSet::cn_crosses_os_bow_or_stern(unsigned int ix) const
{
  return(cnos(ix).cn_crosses_os());
}

//-----------------------------------------------------------
// Procedure: cn_crosses_os_bow_dist()

double ContactStateSet::cn_crosses_os_bow_dist(unsigned int ix) const
{
  return(cnos(ix).cn_crosses_os_bow_dist());
}

//-----------------------------------------------------------
// Procedure: os_curr_cpa_dist()

double ContactStateSet::os_curr_cpa_dist(unsigned int ix) const
{
  return(cnos(ix).os_curr_cpa_dist());
}


//===========================================================
// Analyzers (2) About what just happened on this step
//               Values come from cnos and cnos_hist
//===========================================================

//-----------------------------------------------------------
// Procedure: cpa_reached()
//   Purpose: Looking at current range, and recent history,
//            determine if CPA has just been witnessed
//      Note: The thresh parameter guards against sensor noise

bool ContactStateSet::cpa_reached(double thresh) const
{
  // Two conditions: (1) Recent history shows monotonically
  // decreasing range, (2) current range is greater than the
  // previous range by at least thresh meters

  // Sanity check: cnos history must be size=2 or more
  if(m_cnos_hist.size() < 2)
    return(false);

  // Condition (1) check if range has been steadily decreasing
  // recently. Recall m_cnos_hist index 0 is most recent.
  for(unsigned int i=0; i<m_cnos_hist.size()-1; i++) {
    if(m_cnos_hist[i].range() > (m_cnos_hist[i+1].range() + thresh))
      return(false);
  }

  // Condition (2) check if current range is now higher than
  // the most recent range (+thresh)
  if(m_cnos_curr.range() < (m_cnos_hist[0].range() + thresh))
    return(false);

  return(true);
}

//-----------------------------------------------------------
// Procedure: rng_entered()
//   Returns: true if previously out of range and now in.
//            false otherwise

bool ContactStateSet::rng_entered(double rng_thresh) const
{
  if((cnos(0).range() <= rng_thresh) && (cnos(1).range() > rng_thresh))
    return(true);
  return(false);
}

//-----------------------------------------------------------
// Procedure: rng_exited()
//   Returns: true if previously in range and now out.
//            false otherwise

bool ContactStateSet::rng_exited(double rng_thresh) const
{
  if((cnos(0).range() >= rng_thresh) && (cnos(1).range() < rng_thresh))
    return(true);
  return(false);
}

//-----------------------------------------------------------
// Procedure: os_passed_cn()
//   Returns: true if os was fore and now aft of cn
//   Returns: true if os was aft and now fore of cn
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::os_passed_cn() const
{
  if(os_aft_of_cn(0) != os_aft_of_cn(1))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: os_passed_cn_port()
//   Returns: true if os was fore and now aft, os is on cn's port
//   Returns: true if os was aft and now fore, os is on cn's port
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::os_passed_cn_port() const
{
  if((os_aft_of_cn(0) != os_aft_of_cn(1)) && os_port_of_cn(0))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: os_passed_cn_star()
//   Returns: true if os was fore and now aft, os is on cn's starboard
//   Returns: true if os was aft and now fore, os is on cn's starboard
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::os_passed_cn_star() const
{
  if((os_aft_of_cn(0) != os_aft_of_cn(1)) && os_star_of_cn(0))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: cn_passed_os()
//   Returns: true if cn was fore and now aft of os
//   Returns: true if cn was aft and now fore of os
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::cn_passed_os() const
{
  if(cn_aft_of_os(0) != cn_aft_of_os(1))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: cn_passed_os_port()
//   Returns: true if cn was fore and now aft, cn is on os's port
//   Returns: true if cn was aft and now fore, cn is on os's port
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::cn_passed_os_port() const
{
  if((cn_aft_of_os(0) != cn_aft_of_os(1)) && cn_port_of_os(0))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: cn_passed_os_star()
//   Returns: true if cn was fore and now aft, cn is on os's starboard
//   Returns: true if cn was aft and now fore, cn is on os's starboard
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::cn_passed_os_star() const
{
  if((cn_aft_of_os(0) != cn_aft_of_os(1)) && cn_star_of_os(0))
    return(true);
    
  return(false);
}



//-----------------------------------------------------------
//                                              os_crossed_cn
//-----------------------------------------------------------
// Procedure: os_crossed_cn()
//   Returns: true if os was on port side of cn, and now is not
//            true if os was on starboard side of cn, and now is not
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::os_crossed_cn() const
{
  if(os_port_of_cn(0) != os_port_of_cn(1))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: os_crossed_cn_stern()
//   Returns: true if os crossed behind the contact
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::os_crossed_cn_stern() const
{
  if(os_port_of_cn(0) != os_port_of_cn(1) && os_aft_of_cn())
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: os_crossed_cn_bow()
//   Returns: true if os crossed in front of the contact
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::os_crossed_cn_bow() const
{
  if(os_port_of_cn(0) != os_port_of_cn(1) && os_fore_of_cn())
    return(true);

  return(false);
}



//-----------------------------------------------------------
//                                              cn_crossed_os
//-----------------------------------------------------------
// Procedure: cn_crossed_os()
//   Returns: true if cn was on port side of os, and now is not
//            true if cn was on starboard side of os, and now is not
//            false otherwise

bool ContactStateSet::cn_crossed_os() const
{
  // Note: Index 0 is now, index 1 is most recent history 
  if(cn_port_of_os(0) != cn_port_of_os(1))
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: cn_crossed_os_stern()
//   Returns: true if cn crossed behind ownship
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::cn_crossed_os_stern() const
{
  if(cn_port_of_os(0) != cn_port_of_os(1) && cn_aft_of_os())
    return(true);

  return(false);
}

//-----------------------------------------------------------
// Procedure: cn_crossed_os_bow()
//   Returns: true if cn crossed in front of ownship
//            false otherwise
//      Note: Index 0 is now, index 1 is most recent history 

bool ContactStateSet::cn_crossed_os_bow() const
{
  if(cn_port_of_os(0) != cn_port_of_os(1) && cn_fore_of_os())
    return(true);

  return(false);
}




