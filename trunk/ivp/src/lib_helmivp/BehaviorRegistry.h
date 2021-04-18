/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorRegistry.h                                   */
/*    DATE: May 11th 2011                                        */
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

#ifndef BEHAVIOR_REGISTRY_HEADER
#define BEHAVIOR_REGISTRY_HEADER

#include <string>
#include <vector>
#include "IvPFunction.h"

class BehaviorRegistry {
public:
  BehaviorRegistry();
  virtual ~BehaviorRegistry() {}
  
  bool addIPF(IvPFunction*, unsigned int, std::string="");
  
  unsigned int size() const    {return(m_keys.size());}
  bool         isEmpty() const {return(m_keys.empty());}

  IvPFunction *getIPF(unsigned int index) const;
  IvPFunction *getIPF(std::string key) const;

 protected:
  std::vector<std::string>  m_keys;
  std::vector<IvPFunction*> m_ipfs;
  std::vector<unsigned int> m_iter;

  bool  m_keys_unique;
  
  unsigned int m_latest_iteration;
};

#endif 










