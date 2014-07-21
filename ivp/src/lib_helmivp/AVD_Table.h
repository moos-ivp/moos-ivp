/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVD_Table.h                                          */
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

#ifndef AVD_TABLE_HEADER
#define AVD_TABLE_HEADER

#include <vector>

class AVD_Table {
public:
  AVD_Table();
  ~AVD_Table() {};

  bool initialize(double max_heading_delta, double max_speed, 
		  unsigned int hdg_pts, unsigned int spd_pts, 
		  double default_radius);

  bool addMeasurement(double hdelta, double speed, double radius);

  double getRadius(double hdelta, double speed);

  unsigned int  tsize();

 protected:
  std::vector<std::vector<double> > m_table_value;
  std::vector<std::vector<bool> >   m_table_guess;

  double        m_max_heading_delta;
  double        m_max_speed;
  unsigned int  m_heading_points;
  unsigned int  m_speed_points;
  bool          m_initialized;
};

#endif






