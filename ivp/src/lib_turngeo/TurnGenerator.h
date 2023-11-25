/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TurnGenerator.h                                      */
/*    DATE: July 27th, 2020                                      */
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

#ifndef TURN_GENERATOR_HEADER
#define TURN_GENERATOR_HEADER

#include <vector>
#include <string>
#include "XYPoint.h"

class TurnGenerator 
{
public:
  TurnGenerator();
  ~TurnGenerator() {};
  
  virtual void generate() {};
  virtual bool setTurnRadius(double radius);
  virtual void setEndPos(double endx, double endy);
  virtual void setEndHeading(double endh);
  
  void setStartPos(double osx, double osy, double osh);
  
  bool setPointGap(double gap);
  void setPortTurn(bool v)      {m_port_turn = v;}
  void clear()                  {m_points.clear();}
  
  std::vector<XYPoint> getPts() const {return(m_points);}
  std::vector<double>  getPtsX() const;
  std::vector<double>  getPtsY() const;

  double getTurnRadius() const {return(m_turn_radius);}
  double getPointGap() const {return(m_ptgap);}
  double getTurnLen() const;
  
  unsigned int size() const {return(m_points.size());}

  unsigned int mergePoints(double thresh);

  std::string getTurnDir() const;
  
  bool valid() const;
  
protected:

  double m_start_osx;
  double m_start_osy;
  double m_start_osh;

  double m_endx;
  double m_endy;
  double m_endh;
  double m_ptgap;
  double m_turn_radius;

  bool   m_port_turn;
  
  bool m_start_osx_set;
  bool m_start_osy_set;
  bool m_start_osh_set;

  bool m_endx_set;
  bool m_endy_set;
  bool m_endh_set;
  bool m_ptgap_set;
  bool m_turn_radius_set;

  std::vector<XYPoint> m_points;
};

#endif 

