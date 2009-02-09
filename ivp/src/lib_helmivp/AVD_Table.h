/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AVD_Table.h                                          */
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

#ifndef AVD_TABLE_HEADER
#define AVD_TABLE_HEADER

#include <vector>

class AVD_Table {
public:
  AVD_Table();
  ~AVD_Table() {};

  bool initialize(double max_heading_delta, double max_speed, 
		  int heading_points, int speed_points, 
		  double default_radius);

  bool addMeasurement(double hdelta, double speed, double radius);

  double getRadius(double hdelta, double speed);

  unsigned int  tsize();

 protected:
  std::vector<std::vector<double> > m_table_value;
  std::vector<std::vector<bool> >   m_table_guess;

  double m_max_heading_delta;
  double m_max_speed;
  int    m_heading_points;
  int    m_speed_points;
  bool   m_initialized;
};

#endif
