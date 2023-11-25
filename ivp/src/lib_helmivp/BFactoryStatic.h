/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BFactoryStatic.h                                     */
/*    DATE: Feb 11th, 2010                                       */
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

#ifndef BFACTORY_STATIC_HEADER
#define BFACTORY_STATIC_HEADER

#include <string>
#include "IvPDomain.h"
#include "IvPBehavior.h"

class BFactoryStatic {
 public:
  BFactoryStatic() {}
  virtual ~BFactoryStatic() {}

  // Configuring
  void setDomain(IvPDomain domain) {m_domain = domain;}

  // Building Behaviors
  bool         isKnownBehavior(std::string bhv_name) const;
  IvPBehavior* newBehavior(std::string behavior_type) const;

 private:
  IvPDomain m_domain;
};
#endif











