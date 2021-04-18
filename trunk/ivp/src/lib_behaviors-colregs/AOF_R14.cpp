/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_R14.cpp                                          */
/*    DATE: Feb 9th, 2001 (CPA logic written)                    */
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

#include <cmath> 
#include "AOF_R14.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_R14::AOF_R14(IvPDomain gdomain) : AOF_Contact(gdomain)
{
  m_crs_ix = gdomain.getIndex("course");
  m_spd_ix = gdomain.getIndex("speed");

  m_initial_bng_rate = 0;  
}

//----------------------------------------------------------------
// Procedure: setParam

bool AOF_R14::setParam(const string& param, double param_val)
{
  return(AOF_Contact::setParam(param, param_val));
}

//----------------------------------------------------------------
// Procedure: initialize

bool AOF_R14::initialize()
{
  if(AOF_Contact::initialize() == false)
    return(false);

  if((m_crs_ix==-1) || (m_spd_ix==-1))
    return(false);

  m_initial_bng_rate = calculateInitialMaxBngRate();

  //cout << "================================================" << endl;
  //cout << "Initial Max Bearing Rate: " << m_initial_bng_rate << endl;
  //cout << "================================================" << endl;
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: calculateInitialMaxBngRate()

double AOF_R14::calculateInitialMaxBngRate() const
{
  double bng_os_to_cn = relAng(m_osx, m_osy, m_cnx, m_cny);
  double bng_cn_to_os = relAng(m_cnx, m_cny, m_osx, m_osy);

  double os_right_turn = angle360(bng_os_to_cn + 90);
  double cn_right_turn = angle360(bng_cn_to_os + 90);
  
  CPAEngine cpa_engine(m_cny, m_cnx, cn_right_turn, m_cnv, m_osy, m_osx);

  double initial_bng_rate = cpa_engine.bearingRate(os_right_turn, m_cnv);

  return(initial_bng_rate);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_R14::evalBox(const IvPBox *b) const
{
  if((m_osx == m_cnx) && (m_osy == m_cny))
    return(0);

  double eval_crs = 0;
  double eval_spd = 0;
  m_domain.getVal(m_crs_ix, b->pt(m_crs_ix), eval_crs);
  m_domain.getVal(m_spd_ix, b->pt(m_spd_ix), eval_spd);


  if(portOfContact()) {
    bool crosses_cn_bow = m_cpa_engine.crossesBow(eval_crs, eval_spd);
    if(crosses_cn_bow)
      return(0);
  }
  
  bool star_to_star = m_cpa_engine.passesStar(eval_crs,eval_spd);
  if(star_to_star)
    return(0);
  
  double cpa_dist  = m_cpa_engine.evalCPA(eval_crs, eval_spd, m_tol);
  
  double eval_dist = metric(cpa_dist);

  double eval_three = metric3(eval_crs, eval_spd);
  
  return(eval_dist + eval_three);
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_R14::metric(double eval_dist) const
{
  double min = m_collision_distance;
  double max = m_all_clear_distance;

  if(eval_dist < min) return(0);
  if(eval_dist > max) return(100);

  double tween = 25.0 + 75.0 * (eval_dist - min) / (max-min);
  return(tween); 
}


//----------------------------------------------------------------
// Procedure: metric3
//   Example: m_initial_bng_rate = -0.8
//            bng_rate = -0.6
//            range = 0.8
//            (bng_rate - m_initial_bng_rate) = -0.6 - (-0.8) = 0.2
//            pct = 0.2 / 0.8


double AOF_R14::metric3(double osh, double osv) const
{
  // Sanity check
  if(m_initial_bng_rate >= 0)
    return(0);

  double bng_rate = m_cpa_engine.bearingRate(osh, osv);
  
  if(bng_rate >= 0)
    return(0);
  if(bng_rate <= m_initial_bng_rate)
    return(100);

  double range = -m_initial_bng_rate;

  double pct = -bng_rate / range;

  return(pct * 100);
}




