/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WormHole.h                                           */
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

#ifndef WORM_HOLE_HEADER
#define WORM_HOLE_HEADER

#include <string>
#include <vector>
#include <map>
#include "XYPolygon.h"

class WormHole
{
public:
  WormHole(std::string tag="");
  ~WormHole() {}

 public: // Setters
  bool   setMadridPoly(XYPolygon);
  bool   setWeberPoly(XYPolygon);  

  bool   setMadridPoly(double ctrx, double ctry_y);
  bool   setWeberPoly(double ctrx, double ctry_y);

  void   setTag(std::string s)       {m_tag=s;} 
  void   setDelay(double dval)       {m_delay = dval;}
  void   setIDChange(bool bval)      {m_id_change = bval;}
  void   setConnectionType(std::string);

 public: // Getters
  XYPolygon getMadridPoly() const  {return(m_madrid_poly);}
  XYPolygon getWeberPoly() const   {return(m_weber_poly);}

  std::vector<XYPolygon> getPolys() const;
  
  std::string getTag() const  {return(m_tag);}

  std::vector<std::string> getConfigSummary() const;

  std::string getConnectionType() const {return(m_connection);}
  
 public:
  void crossPositionWeberToMadrid(double wx, double wy, double& mx, double& my);
  void crossPositionMadridToWeber(double mx, double my, double& wx, double& wy);
  
 protected:
  void getCrossPosition(XYPolygon, XYPolygon, double, double, double&, double&);
  
protected:
  std::string m_tag;

  // Antipodes: Weber New Zealand, Madrid Spain
  XYPolygon m_madrid_poly;
  XYPolygon m_weber_poly;

  std::string m_connection;

  double      m_delay;
  bool        m_id_change;
};

#endif 






