/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NavPlotElement.cpp                                   */
/*    DATE: Sep2711                                              */
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

#include <iostream>
#include <string>
#include "NavPlotElement.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

NavPlotElement::NavPlotElement()
{
  // Bounding box of all vehicle positions over all time
  m_min_xpos       = 0;
  m_min_ypos       = 0;
  m_max_xpos       = 0;
  m_max_ypos       = 0;

  m_min_time_local = 0;
  m_max_time_local = 0;
  m_min_time_utc   = 0;
  m_max_time_utc   = 0;
 
  m_vlength        = 0;
  m_logstart_utc   = 0;
}

//-------------------------------------------------------------
// Procedure: initialize
//            Find extremes across all TimePlots for 
//              o m_min_time_local
//              o m_max_time_local
//              o m_min_time_utc
//              o m_max_time_utc
//              o m_logstart_utc
//            Find extremes across all LogPlots for 
//              o m_min_xpos
//              o m_max_xpos
//              o m_min_ypos
//              o m_max_ypos

void NavPlotElement::initialize()
{
  // ----------------------------- m_min_xpos
  m_min_xpos = m_navx_plot.getMinVal();
  m_max_xpos = m_navx_plot.getMaxVal();
  m_min_ypos = m_navy_plot.getMinVal();
  m_max_ypos = m_navy_plot.getMaxVal();

  // ----------------------------- m_min_time_local
  vector<double> min_times_local;
  min_times_local.push_back(m_navx_plot.getMinTime());
  min_times_local.push_back(m_navy_plot.getMinTime());
  min_times_local.push_back(m_hdg_plot.getMinTime());
  min_times_local.push_back(m_vplug_plot.getMinTime());
  m_min_time_local = minElement(min_times_local);

  // ----------------------------- m_max_time_local
  vector<double> max_times_local;
  max_times_local.push_back(m_navx_plot.getMaxTime());
  max_times_local.push_back(m_navy_plot.getMaxTime());
  max_times_local.push_back(m_hdg_plot.getMaxTime());
  max_times_local.push_back(m_vplug_plot.getMaxTime());
  m_max_time_local = maxElement(max_times_local);

  // ----------------------------- m_min_time_utc
  vector<double> min_times_utc;
  min_times_utc.push_back(m_navx_plot.getMinTimeUTC());
  min_times_utc.push_back(m_navy_plot.getMinTimeUTC());
  min_times_utc.push_back(m_hdg_plot.getMinTimeUTC());
  min_times_utc.push_back(m_vplug_plot.getMinTimeUTC());
  m_min_time_utc = minElement(min_times_utc);

  // ----------------------------- m_max_time_utc
  vector<double> max_times_utc;
  max_times_utc.push_back(m_navx_plot.getMaxTimeUTC());
  max_times_utc.push_back(m_navy_plot.getMaxTimeUTC());
  max_times_utc.push_back(m_hdg_plot.getMaxTimeUTC());
  max_times_utc.push_back(m_vplug_plot.getMaxTimeUTC());
  m_max_time_utc = maxElement(max_times_utc);

  // ----------------------------- minimum logstart_utc
  vector<double> min_logstart_utc;
  min_logstart_utc.push_back(m_navx_plot.getMinTimeUTC());
  min_logstart_utc.push_back(m_navy_plot.getMinTimeUTC());
  min_logstart_utc.push_back(m_hdg_plot.getMinTimeUTC());
  min_logstart_utc.push_back(m_vplug_plot.getMinTimeUTC());
  m_logstart_utc = minElement(min_logstart_utc);

  cout << "NavPlotElement initialize:" << endl;
  cout << "  m_min_time_local = " << m_min_time_local << endl;
  cout << "  m_max_time_local = " << m_max_time_local << endl;
  cout << "    m_min_time_utc = " << doubleToString(m_min_time_utc) << endl;
  cout << "    m_max_time_utc = " << doubleToString(m_max_time_utc) << endl;

}
