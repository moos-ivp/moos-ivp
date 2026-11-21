/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT, Cambridge MA                                    */
/*    FILE: VelocityFilter.cpp                                   */
/*    DATE: Apr 23rd, 2022                                       */
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

#include <vector>
#include "MBUtils.h"
#include "VelocityFilter.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

VelocityFilter::VelocityFilter()
{
  m_min_spd = -1;
  m_max_spd = -1;
  m_max_discount = -1;

  m_enabled = true;
  
  m_cnv = 0;
}

//---------------------------------------------------------
// Procedure: setMinSpd()

bool VelocityFilter::setMinSpd(double min_spd)
{
  if(min_spd < 0)
    return(false);

  m_min_spd = min_spd;
  
  if(m_max_spd < m_min_spd)
    m_max_spd = m_min_spd;
  
  return(true);
}

//---------------------------------------------------------
// Procedure: setMaxSpd()

bool VelocityFilter::setMaxSpd(double max_spd)
{
  if(max_spd < 0)
    return(false);

  m_max_spd = max_spd;
  
  if(m_min_spd > m_max_spd)
    m_min_spd = m_max_spd;
  
  return(true);
}

//---------------------------------------------------------
// Procedure: setMaxDiscount()

bool VelocityFilter::setMaxDiscount(double max_discount)
{
  if((max_discount < 0) || (max_discount > 100))
    return(false);

  m_max_discount = max_discount;

  return(true);
}

//---------------------------------------------------------
// Procedure: setSpdCN()

bool VelocityFilter::setSpdCN(double cnv)
{
  if(cnv < 0)
    return(false);

  m_cnv = cnv;

  return(true);
}

//---------------------------------------------------------
// Procedure: valid()

bool VelocityFilter::valid() const
{
  if((m_min_spd < 0) || (m_max_spd < 0) || (m_min_spd > m_max_spd))
    return(false);
  
  if((m_max_discount < 0) || (m_max_discount > 100))
    return(false);
  
  if(m_min_spd > m_max_spd)
    return(false);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: spdRegulate()
//   Returns: A value between [0.0, 1.0] 
//  Examples: Given sreg_min=1, sreg_max=11, max_discount=50
//         a: given_spd=6,  rval=0.75  (50%  of 50% = 25%.  1-0.25=0.75)
//         b: given_spd=1,  rval=0.50  (100% of 50% = 50%.  1-0.50=0.50)
//         d: given_spd=12, rval=1.00  (0%   of 50% = 0%.   1-0.00=1.00)
//         d: given_spd=3,  rval=0.90  (80%  of 50% = 40%.  1-0.40=0.60)
//         e: given_spd=9,  rval=0.90  (20%  of 50% = 10%.  1-0.10=0.90)
// 
//      Note: A return value of 1 indicates there is NO speed
//            regulation.
// 
//      Note: It is assumed the caller will do something like:
//            standoff_distance *= spdRegulate(osv);


double VelocityFilter::spdRegulate(double given_spd) const
{
  // First off be clear: evaluated speed is the given speed +
  // the contact speed. If this is being applied to a stationary
  // contact/obstacle, then that will be zero.
  //
  // Note the contact speed can be set by the user of this class
  // to simply be the speed of the contact, OR, it can be set to
  // the speed of the contact in the direction of ownship. The
  // latter is perhaps better, but that is up to the discretion
  // of the user of this class. 
  given_spd += m_cnv;

  // Sanity check: Ensure spd regulation is enabled
  if(!isSpdRegulated())
    return(1);

  // Sanity check: Recheck the range is >= 0 
  double spd_range = m_max_spd - m_min_spd;
  if(spd_range <= 0) 
    return(1);

  // ----------------------------------------------------------------
  // Part 1: Determine the percentage [0,1] of the available discount
  // ----------------------------------------------------------------
  double pct_discount = 0;  // Be conservative, no discount
  
  if(given_spd > m_max_spd) {       // No discount
    pct_discount = 0;
  }
  else if(given_spd < m_min_spd) {  // Full discount
    pct_discount = 1;
  }
  else {
    // Stuff in between no and full discount (0,1)
    // eg sreg_max_spd=11 - given_spd=3. delta=8
    double delta = m_max_spd - given_spd;  

    // eg delta=8 / rng=10. pct_discount=0.8
    pct_discount = delta / spd_range;           
  }
  
  // ----------------------------------------------------------------
  // Part 2: Convert pct discount to pct of original full value
  // ----------------------------------------------------------------
  // eg pct=0.8 * 50% = 40%.
  double applied_discount = pct_discount * m_max_discount; 

  // eg 40% becomes 0.4
  applied_discount = applied_discount / 100;

  // eg a 10% (.1) discount meant 90% (0.9) of original value
  // eg 0.1 becomes 0.9
  double retval = 1 - applied_discount; 

  return(retval);
}


//---------------------------------------------------------
// Procedure: getSpec()

string VelocityFilter::getSpec() const
{
  string str = "min_spd=" + doubleToStringX(m_min_spd,2);
  str += ",max_spd=" + doubleToStringX(m_max_spd,2);
  str += ",max_discount=" + doubleToStringX(m_max_discount,2);
  str += ",enabled=" + boolToString(m_enabled);

  return(str);
}



//---------------------------------------------------------
// Procedure: stringToVelocityFilter()
//      Note: Example:
//            min_spd=3, max_spd=8, max_discount=85


VelocityFilter stringToVelocityFilter(std::string str)
{
  VelocityFilter null_filter;
  VelocityFilter filter;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    double dval  = atof(value.c_str());
    
    if(param == "pct")  // handle deprecated param name
      param = "max_discount";
    
    if((param == "min_spd") && isNumber(value))
      filter.setMinSpd(dval);
    else if((param == "max_spd") && isNumber(value))
      filter.setMaxSpd(dval);
    
    else if((param == "max_discount") && isNumber(value))
      filter.setMaxDiscount(dval);
  }

  if(!filter.valid())
    return(null_filter);
    
  return(filter);
}

