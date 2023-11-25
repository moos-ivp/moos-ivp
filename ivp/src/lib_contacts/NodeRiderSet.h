/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeRiderSet.h                                       */
/*    DATE: May 7th 2022                                         */
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

#ifndef NODE_RIDER_SET_HEADER
#define NODE_RIDER_SET_HEADER

#include <string>
#include <vector>
#include "NodeRider.h"

class NodeRiderSet
{
public:
  NodeRiderSet() {};
  ~NodeRiderSet() {};

  bool addNodeRider(NodeRider);
  bool addNodeRider(std::string);

  bool updateRider(std::string var, std::string val, double utc);
  
  std::vector<std::string> getVars() const;
  std::vector<std::string> getRiderSpecs() const;

  std::string getRiderReports(double uct);
  
  NodeRider getNodeRider(unsigned int) const;
  
  unsigned int size() const {return(m_riders.size());}
  
 protected:  
  bool setPolicy(std::string);
  bool setFrequency(double);
    
 protected:
  std::vector<NodeRider>  m_riders;

};

#endif 

