/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_Guide.h                                          */
/*    DATE: Oct 29 2022                                          */
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

#ifndef BHV_GUIDE_HEADER
#define BHV_GUIDE_HEADER

#include <vector>
#include <string>

class BHV_Guide
{
public:
  BHV_Guide() {m_action="web";};
  virtual ~BHV_Guide() {}

  bool setBehaviorName(std::string);
  bool setAction(std::string);
  
  void guide();
  
protected:

  std::string m_bhv_name;
  std::string m_action;
};

#endif










