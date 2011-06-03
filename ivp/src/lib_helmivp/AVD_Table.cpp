/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVD_Table.cpp                                        */
/*    DATE: Feb 8th, 2009                                        */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
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

