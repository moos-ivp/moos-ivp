/***************************************************************/
/*    NAME: Michael Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                                  */
/*    FILE: VelocityFilter.cpp                                 */
/*    DATE: Apr 23rd, 2022                                     */
/*                                                             */
/* This is unreleased BETA code. No permission is granted or   */
/* implied to use, copy, modify, and distribute this software  */
/* except by the author(s), or those designated by the author. */
/***************************************************************/

#include <vector>
#include <iterator>
#include "MBUtils.h"
#include "VelocityFilter.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

VelocityFilter::VelocityFilter()
{
  m_min_spd = -1;
  m_max_spd = -1;
  m_min_spd_pct = -1;

  m_osv = -1;
  m_cnv = -1;
}

//---------------------------------------------------------
// Procedure: valid()

bool VelocityFilter::valid() const
{
  if((m_min_spd < 0) || (m_max_spd < 0) || (m_min_spd < 0))
    return(false);
  
  if(m_min_spd_pct > 100)
    return(false);
  
  if(m_min_spd > m_max_spd)
    return(false);
  
  return(true);
}

//---------------------------------------------------------
// Procedure: getFilterPct()

double VelocityFilter::getFilterPct() const
{
  // Evaluation spd is maximum of ownship and contact speeds
  double eval_spd = m_osv;
  if(eval_spd < m_cnv)
    eval_spd = m_cnv;

  // Easy cases where eval speed is above or below range
  if(eval_spd >= m_max_spd)
    return(100);
  if(eval_spd <= m_min_spd)
    return(m_min_spd_pct);

  double filter_range_pct = 100 - m_min_spd_pct;    //  [0,100]
  
  double filter_range_spd = m_max_spd - m_min_spd;
  if(filter_range_spd <= 0)
    return(100);

  double delta_spd = eval_spd - m_min_spd;
  double range_pct = delta_spd / filter_range_spd;  //  [0,1]

  double filter_pct = m_min_spd_pct;                //  [0,100]

  filter_pct += (range_pct * filter_range_pct);     //  [0,100]

  filter_pct = filter_pct / 100;                    //  [0,1]
  
  // Further outgoing sanity checks
  if(filter_pct > 1.0)
    return(1);
  if(filter_pct < 0)
    return(0);

  return(filter_pct);
}

//---------------------------------------------------------
// Procedure: stringToVelocityFilter()

VelocityFilter stringToVelocityFilter(std::string str)
{
  VelocityFilter null_filter;
  VelocityFilter filter;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    double dval  = atof(value.c_str());

    if((param == "min_spd") && isNumber(value))
      filter.setMinSpd(dval);
    else if((param == "max_spd") && isNumber(value))
      filter.setMaxSpd(dval);
    else if((param == "pct") && isNumber(value))
      filter.setMinSpdPct(dval);
  }

  if(!filter.valid())
    return(null_filter);
    
  return(filter);
}

