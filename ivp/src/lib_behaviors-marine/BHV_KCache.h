/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Timer.h                                          */
/*    DATE: Apr 29th 2015 (Collier dedication, Phoenix Talk)     */
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
 
#ifndef BHV_KCACHE_HEADER
#define BHV_KCACHE_HEADER

#include "IvPBehavior.h"

class BHV_KCache : public IvPBehavior {
public:
  BHV_KCache(IvPDomain);
  ~BHV_KCache() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

 protected:
  void    postVisuals();

 protected:
  double  m_hdg_gap;
  double  m_spd;

  double  m_seglr_vertex_size;
  double  m_seglr_edge_width;
  std::string m_seglr_vertex_color;
  std::string m_seglr_edge_color;

};

#endif

