/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AVD_Table.cpp                                        */
/*    DATE: Feb 8th, 2009                                        */
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
}

//------------------------------------------------------------------
// Procedure: initialize()

void AVD_Table::initialize(double max_heading_delta, double max_speed, 
			   int heading_points, int speed_points,
			   double default_radius)
{
  m_max_heading_delta  = max_heading_delta;
  m_max_speed          = max_speed;
  m_heading_points     = heading_points;
  m_speed_points       = speed_points;

  m_table.clear();

  m_table.reserve(heading_points);
  unsigned int i, j;
  for(i=0; i<m_heading_points; i++) {
    vector<double> ivector(speed_points, default_radius);
    m_table.push_back(ivector);
  }
}

//------------------------------------------------------------------
// Procedure: tsize

unsigned int AVD_Table::tsize()
{
  unsigned int vsize = m_table.size();
  unsigned int total = 0;
  
  for(unsigned int i=0; i<vsize; i++)
    total += m_table[i].size();

  return(total);
}

