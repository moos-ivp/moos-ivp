/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPChecker.h                                         */
/*    DATE: April 18th, 2011                                     */
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

#ifndef IVP_CHECKER_HEADER
#define IVP_CHECKER_HEADER

#include <vector>
#include "IvPFunction.h"
#include "QuadSet.h"

class IvPChecker
{
 public:
  IvPChecker() {};
  ~IvPChecker() {};
  
  bool checkIvP(const std::vector<std::string>& ipf_strings);
  void checkBrute(const std::vector<std::string>& ipf_strings);
  void checkCollectiveQuadSet(const QuadSet&);
  
 protected:
  void buildIvPDomain();
  void buildIvPFunctions(const std::vector<std::string>& ipf_strings);
  void clearIvPFunctions();

 protected:
  IvPDomain                  m_ivp_domain;
  std::vector<IvPFunction*>  m_ivp_functions;
};

#endif


