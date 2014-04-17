/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVD_Table.cpp                                        */
/*    DATE: Feb 8th, 2009                                        */
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

#include "AVD_Table.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------------
// Constructor

AVD_Table::AVD_Table()
{
  m_max_heading_delta  = 180;
  m_max_speed          = 4.0;
  m_heading_points     = 181;
  m_speed_points       = 41;
  m_initialized        = false;
}

//------------------------------------------------------------------
// Procedure: initialize()

bool AVD_Table::initialize(double max_heading_delta, double max_speed, 
			   unsigned int hdg_pts, unsigned int spd_pts,
			   double default_radius)
{
  if((max_heading_delta <= 0)  || (max_speed <= 0)      ||
     (hdg_pts == 0) || (spd_pts == 0) || (default_radius < 0)) 
    return(false);
  
  m_max_heading_delta  = max_heading_delta;
  m_max_speed          = max_speed;
  m_heading_points     = hdg_pts;
  m_speed_points       = spd_pts;

  m_table_value.clear();
  m_table_guess.clear();

  m_table_value.reserve(m_heading_points);
  m_table_guess.reserve(m_heading_points);
  unsigned int i;
  for(i=0; i<m_heading_points; i++) {
    vector<double> ivector(m_speed_points, default_radius);
    vector<bool>   bvector(m_speed_points, true);
    m_table_value.push_back(ivector);
    m_table_guess.push_back(bvector);
  }

  m_initialized = true;
  return(true);
}

//------------------------------------------------------------------
// Procedure: tsize

unsigned int AVD_Table::tsize()
{
  if(!m_initialized)
    return(0);

  unsigned int vsize = m_table_value.size();
  unsigned int total = 0;
  
  for(unsigned int i=0; i<vsize; i++)
    total += m_table_value[i].size();

  return(total);
}

//------------------------------------------------------------------
// Procedure: addMeasurement

bool AVD_Table::addMeasurement(double hdelta, double speed, double radius)
{
  if(!m_initialized)
    return(false);

  unsigned int hd_ix, spd_ix;

  if(m_heading_points == 0)
    return(false);

  double hdelta_delta = (m_max_heading_delta / m_heading_points);
  double dbl_hunits = (hdelta / hdelta_delta);
  hd_ix = (int)(dbl_hunits + 0.5);
  if(hd_ix >= m_heading_points)
    hd_ix = m_heading_points-1;

  double speed_delta = (m_max_speed / m_speed_points);
  double dbl_sunits = (speed / speed_delta);
  spd_ix = (int)(dbl_sunits + 0.5);
  if(spd_ix >= m_speed_points)
    spd_ix = m_speed_points-1;

  m_table_value[hd_ix][spd_ix] = radius;
  m_table_guess[hd_ix][spd_ix] = false;

  return(true);
}






