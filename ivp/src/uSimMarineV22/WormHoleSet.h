/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WormHoleSet.h                                        */
/*    DATE: Jan 21st 2021                                        */
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

#ifndef WORM_HOLE_SET_HEADER
#define WORM_HOLE_SET_HEADER

#include <string>
#include <vector>
#include "WormHole.h"

class WormHoleSet
{
public:
  WormHoleSet();
  ~WormHoleSet() {}

 public: // Setters
  bool   addWormHoleConfig(std::string);
  void   setTunnelTime(double);
  void   setTransparency(double);
  
 public: // Getters
  WormHole  getWormHole(unsigned int);
  WormHole  getWormHole(std::string);

  double getTransparency() const {return(m_transparency);}
  double getTunnelTime() const   {return(m_tunnel_time);}
  
  unsigned int size() const {return(m_worm_holes.size());}

  std::vector<std::string> getConfigSummary() const;

 public: // Application  

  bool apply(double curr_time, double osx, double osy,
	     double& newx, double& newy);

 protected:
  std::string findWormHoleEntry(double osx, double osy);
  
  
 protected: // Config vars
  std::vector<WormHole>    m_worm_holes;
  std::vector<std::string> m_worm_hole_tags;
  double      m_tunnel_time;

 protected: // state vars (for particular vehicle)
  std::string m_worm_hole_state;
  std::string m_worm_hole_tag;
  double      m_worm_hole_tstamp;
  double      m_transparency;

  double      m_inx;
  double      m_iny;
};

#endif 






