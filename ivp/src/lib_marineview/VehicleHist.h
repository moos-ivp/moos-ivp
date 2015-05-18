/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VehicleHist.h                                        */
/*    DATE: Oct 29th 2014                                        */
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

#ifndef VEHICLE_HISTORY_HEADER
#define VEHICLE_HISTORY_HEADER

#include <vector>
#include <string>
#include <map>
#include "NodeRecord.h"

class VehicleHist
{
 public:
  VehicleHist();
  ~VehicleHist() {}
    
  void addPosition(double x, double y, double hdg);

  
 private: 
  unsigned int getIndex(double timestamp);
  
  double getX(double timestamp, unsigned int index=0);
  double getY(double timestamp, unsigned int index=0);
  double getHDG(double timestamp, unsigned int index=0);

 private:
  std::vector<float> m_x;
  std::vector<float> m_y;
  std::vector<float> m_hdg;
  std::vector<float> m_time;
};

#endif 



